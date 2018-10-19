//
// Created by benoit on 18/10/16.
//

#include <iostream>
#include "peer.h"

namespace snooz {

Peer::Peer(zmq::context_t& ctx, std::string address, std::string my_address):
    deadline_{std::chrono::system_clock::now() + std::chrono::seconds{DISCONNECTED_TIMEOUT_SEC}},
    address_{std::move(address)},
    dealer_(ctx, ZMQ_DEALER)
{

    dealer_.setsockopt(ZMQ_IDENTITY, my_address.c_str(), my_address.size());

};

void Peer::connect() {
    std::cout << "Connect dealer to " << address_ << std::endl;
    dealer_.connect(address_.c_str());
}

void Peer::send(std::string msg) {
    s_send(dealer_, msg);
}

void Peer::send(const ZmqMessage& msg) {
    send_message(dealer_, msg);
}

std::string Peer::address() const {
    return address_;
}

bool Peer::is_alive() const {
    return std::chrono::system_clock::now() <= deadline_;
}

void Peer::reset_deadline(int seconds) {
    deadline_ = std::chrono::system_clock::now() + std::chrono::seconds{seconds};
}

}