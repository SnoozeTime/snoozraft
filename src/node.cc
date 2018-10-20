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

using std::literals::operator""ms;

namespace snooz {

Node::Node(JsonConfig conf):
    conf_{std::move(conf)},
    my_address_{"tcp://" + conf_.host() + ":" + conf_.port()}{
}

void Node::start() {
    std::stringstream ss;
    ss << "tcp://*:" << conf_.port();
    std::cout << "Will bind to: " << ss.str() << std::endl;
    server_.bind(ss.str());

    // Now say hello to everybody.
    if (!conf_.is_bootstrap()) {
        for (const auto& bootstrap : conf_.bootstrap_nodes()) {
            // The key must be the same as the one the router will create. To do so, each bootstrap nodes have to set
            // an identity for their dealers.
            add_peer(bootstrap.address);
            ZmqMessage join_message{"JOIN", my_address_};
            peers_.at(bootstrap.address).send(join_message);
        }
    }

    ZmqLoop loop{&zmq_context_};
    loop.add_zmq_socket(server_, [this]() {
        auto message = receive_message(server_);
        handle_message(message);
    });

    // TODO need to make this configurable
    loop.add_timeout(5000ms, [this]() {
        send_heartbeat();
    });
    loop.add_timeout(10000ms, [this]() {
        reap_dead_bodies();
    });

    loop.run();
}


// Will route message to correct handler.
void Node::handle_message(const ZmqMessage& message) {

    // Address is always first.
    const auto& frames = message.frames();

    /// ADDRESS | TYPE | CONTENT+
    assert(frames.size() >= 3);
    auto addr = frames[0];
    auto type = frames[1];

    std::cout << "Handle message type " << type << std::endl;

    // Always do that. If the peer already exists, we do nothing. If it does not, we will add it
    if (peers_.find(addr) == peers_.end()) {

        std::cout << "will add new peer. Address is " << addr << " : " << frames[2] << std::endl;
        // It's a new peer. We need to add it.
        // If the node is a bootstrap, we need to set the identity of the dealer socket so that the other nodes
        // that will receive the message can find it in their peers_ map.
        add_peer(addr);

        if (type == "JOIN") {
            // Need to tell all the peers that we received a new peer connection.
            for (auto &peers : peers_) {
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
        std::cout << "Received hb from " << addr << std::endl;
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

    std::cout << "HANDLE PEER LIST\n";
    const auto& frames = message.frames();
    for (int i = 2; i < frames.size(); i++) {
        if (frames[i] != my_address_
            && peers_.find(frames[i]) == peers_.end()) {
            std::cout << "will add " << frames[i] << std::endl;
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
    std::cout << "IT IS TIME TO REAP!\n";
    std::vector<std::string> to_reap;
    for (auto& entry: peers_) {
        if (!entry.second.is_alive()) {
            to_reap.push_back(entry.first);
        }
    }

    for (auto& addr: to_reap) {
        std::cout << "Will reap " << addr << "... Sayonara.\n";
        peers_.erase(addr);
    }

}


}