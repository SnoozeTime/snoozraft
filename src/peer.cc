//
// Created by benoit on 18/10/16.
//

#include "peer.h"

namespace snooz {

Peer::Peer(zmq::context_t& ctx, std::string address):
    deadline_{std::chrono::system_clock::now() + std::chrono::seconds{DISCONNECTED_TIMEOUT_SEC}},
    address_{std::move(address)},
    dealer_(ctx, ZMQ_DEALER)
{};

void Peer::connect() {
    dealer_.connect(address_.c_str());
}

bool Peer::is_alive() const {
    return std::chrono::system_clock::now() <= deadline_;
}

void Peer::reset_deadline(int seconds) {
    deadline_ = std::chrono::system_clock::now() + std::chrono::seconds{seconds};
}

}