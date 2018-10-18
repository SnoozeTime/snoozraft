//
// Created by benoit on 18/10/16.
//

#include "zmq_helpers.h"


namespace snooz {

    std::string zmq_to_string(const zmq::message_t &msg) {
        auto buf = static_cast<const char *>(msg.data());
        return std::string(buf, msg.size());
    }

    zmq::message_t zmq_from_string(const std::string &content) {
        return zmq::message_t{content.c_str(), content.size()};
    }

    bool s_send(zmq::socket_t &socket, const std::string &content) {
        zmq::message_t msg = zmq_from_string(content);
        return socket.send(msg);
    }

    bool s_sendmore(zmq::socket_t &socket, const std::string &content) {
        zmq::message_t msg = zmq_from_string(content);
        return socket.send(msg, ZMQ_SNDMORE);
    }

    std::string s_recv(zmq::socket_t &socket) {
        zmq::message_t msg;
        socket.recv(&msg);
        return zmq_to_string(msg);
    }

}