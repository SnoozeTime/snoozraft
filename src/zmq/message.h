//
// Created by benoit on 18/10/18.
//

#pragma once

#include <vector>
#include <string>

#include "zmq_helpers.h"

namespace snooz {

class ZmqMessage {
public:

    ZmqMessage() = default;
    ZmqMessage(std::initializer_list<std::string> init_list);

    void add_frame(std::string frame);
    void add_empty_frame();

    const std::vector<std::string>& frames() const;
private:
    std::vector<std::string> frames_;
};

// TODO really need to wrap the zmq::socket_t but meh.
void send_message(zmq::socket_t& socket, const ZmqMessage& message);
ZmqMessage receive_message(zmq::socket_t &socket);

}



