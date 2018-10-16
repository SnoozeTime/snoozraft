//
// Created by benoit on 18/10/16.
//

#include <iostream>
#include "peer.h"

namespace snooz {

Peer::Peer(zmq::context_t& ctx, std::string address, std::string bootstrap_name):
    deadline_{std::chrono::system_clock::now() + std::chrono::seconds{DISCONNECTED_TIMEOUT_SEC}},
    address_{std::move(address)},
    dealer_(ctx, ZMQ_DEALER)
{
    if (!bootstrap_name.empty()) {
        dealer_.setsockopt(ZMQ_IDENTITY, bootstrap_name.c_str(), bootstrap_name.size());
    }
};

void Peer::connect() {
    std::cout << "Connect dealer to " << address_ << std::endl;
    dealer_.connect(address_.c_str());
}

void Peer::send(std::string msg) {
    s_send(dealer_, msg);
}

void Peer::send_multipart(const std::vector<std::string> &msg) {
    if (msg.empty()) {
        return;
    }

    if (msg.size() == 1) {
        send(msg[0]);
    } else {
        for (int i = 0; i < msg.size() - 1; i++) {
            s_sendmore(dealer_, msg[i]);
        }
        s_send(dealer_, msg.back());
    }
}

bool Peer::is_alive() const {
    return std::chrono::system_clock::now() <= deadline_;
}

void Peer::reset_deadline(int seconds) {
    deadline_ = std::chrono::system_clock::now() + std::chrono::seconds{seconds};
}

}