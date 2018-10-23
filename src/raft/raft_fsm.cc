//
// Created by benoit on 18/10/22.
//

#include "raft_fsm.h"
#include "zmq/message.h"
#include "node.h"
#include <chrono>
#include <boost/log/trivial.hpp>


#include <protocol/messages/AppendEntriesRequestMessage.h>
#include <protocol/messages/AppendEntriesReplyMessage.h>
#include <protocol/messages/RequestVoteReplyMessage.h>
#include <protocol/messages/RequestVoteRequestMessage.h>

using namespace std::literals;

namespace snooz {

RaftFSM::RaftFSM(Node* node): node_(node) {

    auto random_integer = uni(rng);
    BOOST_LOG_TRIVIAL(info) << "Will wait " << random_integer << "before timeout";
    node_->loop().add_timeout(std::chrono::milliseconds{random_integer},
            [this] () { before_candidate();}, false); // non recurrent timer.
}

void RaftFSM::handle(const ZmqMessage& msg) {

}

void RaftFSM::before_candidate() {
    BOOST_LOG_TRIVIAL(info) << "RAFT -> transition to candidate";
    state_ = RaftState::CANDIDATE;
    term_++;
    voted_for = "me!";

    for (auto& entry: node_->peers()) {

    }
}

void RaftFSM::before_leader() {
    // Set up the timeout for sending heartbeat.

}

void RaftFSM::before_follower() {
    // Timeout for next election. This can be canceled when receiving from leader.
}

void RaftFSM::send_to_peers(const ZmqMessage &msg) {

}

void RaftFSM::after_candidate() {

}

void RaftFSM::after_leader() {

}

void RaftFSM::after_follower() {

}

void RaftFSM::set_state(RaftState state) {

}

void RaftFSM::on_message(const AppendEntriesRequestMessage &msg) {
    MessageHandler::on_message(msg);
}

void RaftFSM::on_message(const AppendEntriesReplyMessage &msg) {
    MessageHandler::on_message(msg);
}

void RaftFSM::on_message(const RequestVoteRequestMessage &msg) {
    MessageHandler::on_message(msg);
}

void RaftFSM::on_message(const RequestVoteReplyMessage &msg) {
    MessageHandler::on_message(msg);
}


}