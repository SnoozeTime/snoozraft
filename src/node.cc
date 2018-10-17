//
// Created by benoit on 18/10/16.
//

#include "node.h"
#include "cassert"
#include <sstream>
#include <iostream>
#include <chrono>

#include "zmq_helpers.h"
#include "string_utils.h"

using std::literals::operator""ms;

namespace snooz {

Node::Node(JsonConfig conf):
    conf_{std::move(conf)} {
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
            peers_.emplace(bootstrap.name, Peer{zmq_context_, bootstrap.address});
            peers_.at(bootstrap.name).connect();
            peers_.at(bootstrap.name).send("tcp://localhost:" + conf_.port());
        }
    }

    bool should_continue = true;
    zmq::pollitem_t items [] = {
            {(void*) server_, 0, ZMQ_POLLIN, 0}
    };
    while (should_continue) {

        zmq::poll(items, 1, 1000ms);

        if (items[0].revents & ZMQ_POLLIN) {
            auto addr = snooz::s_recv(server_);
            auto content = snooz::s_recv(server_);

            if (peers_.find(addr) == peers_.end()) {
                // It's a new peer. We need to add it.
                // If the node is a bootstrap, we need to set the identity of the dealer socket so that the other nodes
                // that will receive the message can find it in their peers_ map.
                std::string identity = conf_.is_bootstrap() ? conf_.bootstrap_name().value() : ""; // Btw, this should always be o.k. as we do the validation in config object
                peers_.emplace(addr, Peer{zmq_context_, content, identity});
                peers_addresses_.push_back(content);
                peers_.at(addr).connect();

                // Need to tell all the peers that we received a new peer connection.
                for (auto &peers : peers_) {
                    peers.second.send(snooz::join(peers_addresses_, ","));
                }

            } else {
                peers_.at(addr).reset_deadline();
            }

            std::cout << content << std::endl;
        } else {
            std::cout << "timeout\n";
        }
    }
}

}