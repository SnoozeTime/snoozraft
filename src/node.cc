//
// Created by benoit on 18/10/16.
//

#include "node.h"
#include "cassert"
#include <sstream>
#include <iostream>

#include "zmq_helpers.h"
#include "string_utils.h"

namespace snooz {

Node::Node(std::vector<std::string> bootstraps, std::string port):
    bootstrap_nodes_(std::move(bootstraps)),
    port_{std::move(port)} {
}

void Node::start() {
    std::stringstream ss;
    ss << "tcp://*:" << port_;
    std::cout << "Will bind to: " << ss.str() << std::endl;
    server_.bind(ss.str());

    // Now say hello to everybody.
    for (std::string bootstrap : bootstrap_nodes_) {
        // The key must be the same as the one the router will create. To do so, each bootstrap nodes have to set
        // an identity for their dealers.
        std::cout << "Will connect to bootstrap node: " << bootstrap << std::endl;
        peers_.emplace("BOOTSTRAP", Peer{zmq_context_, bootstrap});
        peers_.at("BOOTSTRAP").connect();
        peers_.at("BOOTSTRAP").send("tcp://localhost:" + port_);
    }

    bool should_continue = true;
    while (should_continue) {

        auto addr = snooz::s_recv(server_);
        auto content = snooz::s_recv(server_);

        if (peers_.find(addr) == peers_.end()) {
            // It's a new peer. We need to add it.
            // If the node is a bootstrap, we need to set the identity of the dealer socket so that the other nodes
            // that will receive the message can find it in their peers_ map.
            std::string identity = bootstrap_nodes_.empty() ? "BOOTSTRAP" : "";
            peers_.emplace(addr, Peer{zmq_context_, content, identity});
            peers_addresses_.push_back(content);
            peers_.at(addr).connect();

            // Need to tell all the peers that we received a new peer connection.
            for (auto& peers : peers_) {
                peers.second.send(snooz::join(peers_addresses_, ","));
            }

        } else {
            peers_.at(addr).reset_deadline();
        }

        std::cout << content << std::endl;
    }
}

}