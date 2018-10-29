//
// Created by benoit on 18/10/28.
//

#include <boost/log/trivial.hpp>
#include <zmq/message.h>
#include <sstream>
#include "frontend.h"

namespace snooz {
Frontend::Frontend(zmq::context_t &context, std::string port) :
    loop_{"frontend", &context},
    frontend_{context, ZMQ_ROUTER},
    backend_{context, ZMQ_DEALER},
    port_{std::move(port)} {
}

void Frontend::stop() {
    loop_.shutdown();
}

void Frontend::run() {
    BOOST_LOG_TRIVIAL(info) << "Start client frontend";

    // ------------------------------
    // First setup the frontend
    // Bind to a fixed port
    // ------------------------------
    BOOST_LOG_TRIVIAL(info) << "Connect frontend socket";
    std::stringstream addr_stream;
    addr_stream << "tcp://*:" << port_;
    frontend_.bind(addr_stream.str());
    loop_.add_zmq_socket(frontend_, [this]() {
        handle_client_request();
    });

    // -------------------------------------------
    // Then connect to the backend using the inproc
    // (same process) endpoint.
    // --------------------------------------------
    BOOST_LOG_TRIVIAL(info) << "Connect backend socket";
    addr_stream.str("");
    addr_stream << "inproc://request_backend";
    backend_.connect(addr_stream.str());
    loop_.add_zmq_socket(backend_, [this]() {
        handle_backend_response();
    });

    loop_.run();
}

void Frontend::handle_client_request() {
    ZmqMessage msg = receive_message(frontend_);

    BOOST_LOG_TRIVIAL(info) << "Received message from client";
    // for now just forward to the backend.
    send_message(backend_, msg);
}

void Frontend::handle_backend_response() {
    ZmqMessage msg = receive_message(backend_);

    BOOST_LOG_TRIVIAL(info) << "Send message to client";
    // First frame is the addr to send to.
    // Second frame is the response, so can send it as it is through the router.
    send_message(frontend_, msg);
}

}
