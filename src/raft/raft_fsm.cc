//
// Created by benoit on 18/10/22.
//

#include "raft_fsm.h"
#include "zmq/message.h"
#include "node.h"
#include <chrono>
#include <boost/log/trivial.hpp>

using namespace std::literals;

namespace snooz {

RaftFSM::RaftFSM(Node* node): node_(node) {

    auto random_integer = uni(rng);
    node_->loop().add_timeout(std::chrono::milliseconds{random_integer},
            [this] () { before_candidate();}, false); // non recurrent timer.
}

void RaftFSM::handle(const ZmqMessage& msg) {

}

void RaftFSM::before_candidate() {
    BOOST_LOG_TRIVIAL(info) << "RAFT -> transition to candidate";
    state_ = State::CANDIDATE;
    term_++;
    voted_for = "me!";

    for (auto& entry: node_->peers()) {

    }
}


}