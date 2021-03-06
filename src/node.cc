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
#include "zmq/message.h"
#include <boost/log/trivial.hpp>
#include "protocol/messages/HeartbeatMessage.h"
#include "protocol/messages/JoinMessage.h"
#include "protocol/messages/PeerListMessage.h"

using std::literals::operator""ms;

namespace snooz {

Node::Node(Config conf):
    log_(boost::log::keywords::channel = "NODE"),
    conf_{std::move(conf)},
    my_address_{"tcp://" + conf_.host() + ":" + conf_.port()},
    raft_{this, "store"+conf_.host()+conf_.port()},
    frontend_{zmq_context_, conf_.client_port()},
    frontend_thread_([this] { frontend_.run();})
    {

}

Node::~Node() {
    frontend_.stop();
    frontend_thread_.join();
}

void Node::start() {
    std::stringstream ss;
    ss << "tcp://*:" << conf_.port();
    BOOST_LOG(log_) << "Node will bing to " << ss.str();
    server_.bind(ss.str());

    // Now say hello to everybody.
    if (!conf_.is_bootstrap()) {
        BOOST_LOG(log_) << "Will connect to bootstrap peers";
        for (const auto& bootstrap : conf_.bootstrap_nodes()) {
            // The key must be the same as the one the router will create. To do so, each bootstrap nodes have to set
            // an identity for their dealers.
            add_peer(bootstrap);
            Message join_message{std::make_unique<JoinMessage>(my_address_)};
            BOOST_LOG(log_) << "connect to " << bootstrap;
            peers_.at(bootstrap).send(join_message.pack());
        }
    }

    loop_.add_zmq_socket(server_, [this]() {
        auto message = receive_message(server_);
        handle_message(message);
    });

    // --------------------------------------------
    // Then bind the client backend socket
    // --------------------------------------------
    client_backend_.bind("inproc://request_backend");
    loop_.add_zmq_socket(client_backend_, [this] {
        handle_client_request();
    });


    // TODO need to make this configurable
    loop_.add_timeout(5000ms, [this]() {
        BOOST_LOG(log_) << "Heartbeat timeout";
        send_heartbeat();
    });
    loop_.add_timeout(10000ms, [this]() {
        BOOST_LOG(log_) << "Peer garbage collector timeout";
        reap_dead_bodies();
    });

    loop_.run();
}


// Will route message to correct handler.
void Node::handle_message(const ZmqMessage& message) {

    BOOST_LOG(log_) << "Received message";
    // Address is always first.
    const auto& frames = message.frames();

    /// ADDRESS | TYPE | CONTENT+
    assert(frames.size() >= 3);
    auto addr = frames[0];

    // Always do that. If the peer already exists, we do nothing. If it does not, we will add it
    if (peers_.find(addr) == peers_.end()) {

        BOOST_LOG(log_) << "will add new peer. Address is " << addr << " : " << frames[2];
        // It's a new peer. We need to add it.
        // If the node is a bootstrap, we need to set the identity of the dealer socket so that the other nodes
        // that will receive the message can find it in their peers_ map.
        add_peer(addr);
    }

    // Dispatch to the handlers
    Message decoded;
    decoded.unpack(message);
    dispatch(addr, decoded);
    raft_.dispatch(addr, decoded);

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

void Node::send_heartbeat() {
    Message hearbeat{std::make_unique<HeartbeatMessage>()};
    for (auto &entry : peers_) {
        entry.second.send(hearbeat.pack());
    }
}

void Node::reap_dead_bodies() {
    BOOST_LOG(log_) << "IT IS TIME TO REAP!";
    std::vector<std::string> to_reap;
    for (auto& entry: peers_) {
        if (!entry.second.is_alive()) {
            to_reap.push_back(entry.first);
        }
    }

    for (auto& addr: to_reap) {
        BOOST_LOG(log_) << "Will reap " << addr << "... Sayonara.";
        peers_.erase(addr);
    }

}


void Node::on_message(const std::string& from, const snooz::PeerListMessage &msg) {
    for (const auto& addr: msg.peers()) {
        if (addr != my_address_
            && peers_.find(addr) == peers_.end()) {
            BOOST_LOG(log_) << "will add peer" << addr << std::endl;
            add_peer(addr);
        }
    }
}

void Node::on_message(const std::string& from, const snooz::HeartbeatMessage &msg) {
}

void Node::on_message(const std::string& from, const snooz::JoinMessage &msg) {
    BOOST_LOG(log_) << "Send peers to everybody";
    // Need to tell all the peers that we received a new peer connection.

    std::vector<std::string> addresses;
    addresses.reserve(peers_.size());
    for (auto& peer: peers_) {
        addresses.push_back(peer.first);
    }

    Message peer_list{std::make_unique<PeerListMessage>(addresses)};
    auto msg_to_send = peer_list.pack();
    for (auto &peer : peers_) {
        BOOST_LOG(log_) << "Send peers list to " << peer.first;
        peer.second.send(msg_to_send);
    }
}

void Node::handle_client_request() {
    auto msg = receive_message(client_backend_);
    auto frames = msg.frames();
    assert(frames.size() == 2);

    // need to save this and put it as first frame in the return message
    auto addr = frames[0];

    // Are we leader? Raft knows it.
    if (raft_.leader_addr() == my_address()) {
        // process request
        ZmqMessage ok_i_am_leader{addr, "LEADER"};

        raft_.append_to_log(frames[1]);
        // We need to keep a list of addr -> requests (index/term pair)
        // in order to know when to send the answer to the client...
        send_message(client_backend_, ok_i_am_leader);
    } else {
        ZmqMessage not_leader{addr, "NOT_LEADER", raft_.leader_client_addr()};
        send_message(client_backend_, not_leader);
    }

}

// TODO Maybe give loop directly to RaftFSM
ZmqLoop& Node::loop() { return loop_;}
std::map<std::string, Peer>& Node::peers() { return peers_; }

const std::string &Node::my_address() const {
    return my_address_;
}

std::string Node::my_client_address() const {
    return "tcp://" + conf_.host() + ":" + conf_.client_port();
}

}
