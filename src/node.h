//
// Created by benoit on 18/10/16.
//

#ifndef SNOOZRAFT2_NODE_H
#define SNOOZRAFT2_NODE_H

#include <map>
#include <vector>
#include <zmq.hpp>
#include "peer.h"
#include "config.h"

namespace snooz {

///
/// A node in the distributed network. Follow ZMQ pattern:
/// - a ROUTER socket that will listen to incoming messages
/// - one DEALER socket per other peer in the network to send message to.
///
/// A network has Bootstrap/Anchor nodes that have known address and to which other
/// node will connect to and send "Hello".
///
class Node {
public:
    explicit Node(JsonConfig config);

    // Start polling messages + managing timeouts
    void start();
private:
    zmq::context_t zmq_context_{1};
    zmq::socket_t server_{zmq_context_, ZMQ_ROUTER};

    JsonConfig conf_;

    // maintain a address -> Peer map of other peers in the network.
    std::map<std::string, Peer> peers_;
    std::vector<std::string> peers_addresses_;
};

}

#endif //SNOOZRAFT2_NODE_H
