//
// Created by benoit on 18/10/16.
//

#ifndef SNOOZRAFT2_NODE_H
#define SNOOZRAFT2_NODE_H

#include <map>
#include <vector>
#include <zmq.hpp>
#include <zmq/zmq_loop.h>
#include <frontend/frontend.h>
#include <thread>
#include "peer.h"
#include "config.h"
#include "zmq/message.h"
#include "raft/raft_fsm.h"
#include "protocol/msg_handler.h"

namespace snooz {

///
/// A node in the distributed network. Follow ZMQ pattern:
/// - a ROUTER socket that will listen to incoming messages
/// - one DEALER socket per other peer in the network to send message to.
///
/// A network has Bootstrap/Anchor nodes that have known address and to which other
/// node will connect to and send "Hello".
///
class Node: public MessageHandler {
public:
    explicit Node(Config config);
    ~Node();

    // Start polling messages + managing timeouts
    void start();

    // Give access to the loop. Used in our Raft
    ZmqLoop& loop();

    std::map<std::string, Peer>& peers();

    // ----------------------------------------------
    // Handlers for the message
    // ----------------------------------------------
    void on_message(const std::string& from, const PeerListMessage &msg) override;
    void on_message(const std::string& from, const HeartbeatMessage &msg) override;
    void on_message(const std::string& from, const JoinMessage &msg) override;

    const std::string& my_address() const;

private:

    // Will route message to correct handler.
    void handle_message(const ZmqMessage& message);

    // This is sent to all peers to say "I AM ALIVE"
    void send_heartbeat();

    // Garbage collection for dead peers.
    void reap_dead_bodies();

    // Add a peer to the list of peers if it is not there already.
    void add_peer(const std::string& addr);

    void handle_client_request();


    Config conf_;
    zmq::context_t zmq_context_{1};

    ZmqLoop loop_{"node", &zmq_context_};

    zmq::socket_t server_{zmq_context_, ZMQ_ROUTER};
    // Receives request router from front end.
    zmq::socket_t client_backend_{zmq_context_, ZMQ_DEALER};

    std::string my_address_;
    RaftFSM raft_;
    Frontend frontend_;
    std::thread frontend_thread_;

    // maintain a address -> Peer map of other peers in the network.
    std::map<std::string, Peer> peers_;


    std::vector<std::string> peers_addresses_;
};

}

#endif //SNOOZRAFT2_NODE_H
