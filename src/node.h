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
#include "zmq/message.h"

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

    // Will route message to correct handler.
    void handle_message(const ZmqMessage& message);

    // Handle the list of peers :)
    void handle_peer_list(const ZmqMessage& message);

    // Add a peer to the list of peers if it is not there already.
    void add_peer(const std::string& addr);

    // This is sent to all peers to say "I AM ALIVE"
    void send_heartbeat();

    // Garbage collection for dead peers.
    void reap_dead_bodies();

    zmq::context_t zmq_context_{1};
    zmq::socket_t server_{zmq_context_, ZMQ_ROUTER};

    JsonConfig conf_;
    std::string my_address_;

    // maintain a address -> Peer map of other peers in the network.
    std::map<std::string, Peer> peers_;
    // local router address to global address
    std::map<std::string, std::string> local_to_addr_;

    std::vector<std::string> peers_addresses_;
};

}

#endif //SNOOZRAFT2_NODE_H
