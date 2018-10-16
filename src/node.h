//
// Created by benoit on 18/10/16.
//

#ifndef SNOOZRAFT2_NODE_H
#define SNOOZRAFT2_NODE_H

#include <map>
#include <nlohmann/json.hpp>

namespace snooz {

class Peer;

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
    explicit Node(nlohmann::json config);

    // Start polling messages + managing timeouts
    void start();
private:

    // Port of zmq router. Will bind to this port and listen incoming messages
    std::string port_;

    // If it is a bootstrap node, should not try to send message to another bootstrap or itself. Chill here.
    bool is_bootstrap_;

    // maintain a address -> Peer map of other peers in the network.
    std::map<std::string, Peer> peers_;

};

}

#endif //SNOOZRAFT2_NODE_H
