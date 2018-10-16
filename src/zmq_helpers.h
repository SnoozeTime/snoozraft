//
// Created by benoit on 18/10/16.
//

#ifndef SNOOZRAFT2_ZMQ_HELPERS_H
#define SNOOZRAFT2_ZMQ_HELPERS_H

#include <string>
#include <zmq.hpp>

namespace snooz {

    std::string zmq_to_string(const zmq::message_t& msg);

    zmq::message_t zmq_from_string(const std::string& content);

    bool s_send(zmq::socket_t& socket, const std::string& content);
    bool s_sendmore(zmq::socket_t& socket, const std::string& content);

    std::string s_recv(zmq::socket_t& socket);

}



#endif //SNOOZRAFT2_ZMQ_HELPERS_H
