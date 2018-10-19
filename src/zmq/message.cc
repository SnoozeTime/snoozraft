//
// Created by benoit on 18/10/18.
//

#include "message.h"

namespace snooz {


ZmqMessage::ZmqMessage(std::initializer_list<std::string> init_list):
    frames_(init_list){

}

void ZmqMessage::add_frame(std::string frame) {
    frames_.emplace_back(frame);
}

void ZmqMessage::add_empty_frame() {
    frames_.emplace_back("");
}

const std::vector<std::string>& ZmqMessage::frames() const {
    return frames_;
}

// TODO really need to wrap the zmq::socket_t but meh.
void send_message(zmq::socket_t& socket, const ZmqMessage& message) {
    const auto& frames = message.frames();

    if (frames.empty()) {
        return;
    }

    if (frames.size() == 1) {
        s_send(socket, frames[0]);
        return;
    }

    for (int i = 0; i < frames.size() - 1; i++) {
        s_sendmore(socket, frames[i]);
    }
    s_send(socket, frames.back());
}

ZmqMessage receive_message(zmq::socket_t &socket) {

    ZmqMessage message;
    while (true) {
        int more;
        size_t size = sizeof(more);

        message.add_frame(s_recv(socket));

        socket.getsockopt(ZMQ_RCVMORE, &more, &size);
        if (!more) {
            break;
        }
    }

    return message;
}

}
