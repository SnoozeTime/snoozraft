//
// Created by benoit on 18/10/16.
//

#include "node.h"
#include "cassert"
#include <sstream>
#include <iostream>
#include <chrono>

#include "zmq/zmq_helpers.h"
#include "string_utils.h"
#include "zmq/zmq_loop.h"
#include "zmq/message.h"
#include <boost/log/trivial.hpp>

using std::literals::operator""ms;

namespace snooz {

Node::Node(Config conf):
    conf_{std::move(conf)},
    my_address_{"tcp://" + conf_.host() + ":" + conf_.port()}{
}

void Node::start() {
    std::stringstream ss;
    ss << "tcp://*:" << conf_.port();
    BOOST_LOG_TRIVIAL(info) << "Node will bing to " << ss.str();
    server_.bind(ss.str());

    // Now say hello to everybody.
    if (!conf_.is_bootstrap()) {
        BOOST_LOG_TRIVIAL(info) << "Will connect to bootstrap peers";
        for (const auto& bootstrap : conf_.bootstrap_nodes()) {
            // The key must be the same as the one the router will create. To do so, each bootstrap nodes have to set
            // an identity for their dealers.
            add_peer(bootstrap);
            ZmqMessage join_message{"JOIN", my_address_};
            BOOST_LOG_TRIVIAL(info) << "connect to " << bootstrap;
            peers_.at(bootstrap).send(join_message);
        }
    }

    ZmqLoop loop{&zmq_context_};
    loop.add_zmq_socket(server_, [this]() {
        auto message = receive_message(server_);
        handle_message(message);
    });

    // TODO need to make this configurable
    loop.add_timeout(5000ms, [this]() {
        BOOST_LOG_TRIVIAL(debug) << "Heartbeat timeout";
        send_heartbeat();
    });
    loop.add_timeout(10000ms, [this]() {
        BOOST_LOG_TRIVIAL(debug) << "Peer garbage collector timeout";
        reap_dead_bodies();
    });

    loop.run();
}


// Will route message to correct handler.
void Node::handle_message(const ZmqMessage& message) {

    BOOST_LOG_TRIVIAL(debug) << "Received message";
    // Address is always first.
    const auto& frames = message.frames();

    /// ADDRESS | TYPE | CONTENT+
    assert(frames.size() >= 3);
    auto addr = frames[0];
    auto type = frames[1];

    BOOST_LOG_TRIVIAL(debug) << "Handle message type " << type;

    // Always do that. If the peer already exists, we do nothing. If it does not, we will add it
    if (peers_.find(addr) == peers_.end()) {

        BOOST_LOG_TRIVIAL(debug) << "will add new peer. Address is " << addr << " : " << frames[2];
        // It's a new peer. We need to add it.
        // If the node is a bootstrap, we need to set the identity of the dealer socket so that the other nodes
        // that will receive the message can find it in their peers_ map.
        add_peer(addr);

        if (type == "JOIN") {
            BOOST_LOG_TRIVIAL(debug) << "Send peers to everybody";
            // Need to tell all the peers that we received a new peer connection.
            for (auto &peers : peers_) {
                BOOST_LOG_TRIVIAL(debug) << "Send peers list to " << peers.second.address();
                ZmqMessage list_message{"LIST_PEERS"};
                for (auto& paddr: peers_addresses_) {
                    list_message.add_frame(paddr);
                }
                peers.second.send(list_message);
            }
        }
    }

    if (type == "LIST_PEERS") {
        handle_peer_list(message);
    } else if (type == "HEARTBEAT") {
        BOOST_LOG_TRIVIAL(info) << "Received peer heartbeat from " << addr;
    }

    peers_.at(addr).reset_deadline();
}


void Node::add_peer(const std::string& address) {

    // It's a new peer. We need to add it.
    // If the node is a bootstrap, we need to set the identity of the dealer socket so that the other nodes
    // that will receive the message can find it in their peers_ map.
    peers_.emplace(address, Peer{zmq_context_, address, my_address_});
    peers_addresses_.push_back(address);
    peers_.at(address).connect();
}

void Node::handle_peer_list(const snooz::ZmqMessage &message) {

    const auto& frames = message.frames();
    for (int i = 2; i < frames.size(); i++) {
        if (frames[i] != my_address_
            && peers_.find(frames[i]) == peers_.end()) {
            BOOST_LOG_TRIVIAL(info) << "will add peer" << frames[i] << std::endl;
            add_peer(frames[i]);
        }
    }
}

void Node::send_heartbeat() {
    ZmqMessage hb{"HEARTBEAT", "Hi!"};

    for (auto &entry : peers_) {
        entry.second.send(hb);
    }
}

void Node::reap_dead_bodies() {
    BOOST_LOG_TRIVIAL(info) << "IT IS TIME TO REAP!";
    std::vector<std::string> to_reap;
    for (auto& entry: peers_) {
        if (!entry.second.is_alive()) {
            to_reap.push_back(entry.first);
        }
    }

    for (auto& addr: to_reap) {
        BOOST_LOG_TRIVIAL(info) << "Will reap " << addr << "... Sayonara.";
        peers_.erase(addr);
    }

}


}