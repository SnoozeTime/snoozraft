//
// Created by benoit on 18/10/28.
//

#pragma once

#include "zmq/zmq_helpers.h"
#include "zmq/zmq_loop.h"

namespace snooz {

/// Front-end is the client facing interface.
/// Will bind a router to a port to listen to incoming connections.
/// Will connect to the node in order to issue requests.
class Frontend {
public:
    Frontend(zmq::context_t &context, int port);

    /// Will connect the sockets and run the event loop. Blocking so should
    /// be run in a different thread.
    void run();

    /// Stop the loop.
    void stop();
private:

    /// Incoming message from the client
    void handle_client_request();

    /// Response from raft backend
    void handle_backend_response();

    ZmqLoop loop_;
    zmq::socket_t frontend_;
    zmq::socket_t backend_;
    int port_;

};

}



