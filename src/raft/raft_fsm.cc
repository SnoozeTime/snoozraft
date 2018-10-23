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

    //auto random_integer = uni(rng);
    //BOOST_LOG_TRIVIAL(info) << "Will wait " << random_integer << "before timeout";
    start_timer_ = node_->loop().add_timeout(1s,
            [this] () {
                // If there are enough peers, start raft consensus.
                if (node_->peers().size() < 3) {
                    BOOST_LOG_TRIVIAL(info) << "Less than 3 nodes. Will wait more before starting";
                } else {
                    BOOST_LOG_TRIVIAL(info) << "Will start RAFT";
                    node_->loop().remove_timeout(start_timer_);
                    set_state(RaftState::FOLLOWER);
                }
            });

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
    // This is super manual right now. To update if more states in the future
    if (state_ == state) {
        return;
    }

    // Transition to do after a state. This will clean up whatever
    // state was used during the current state.
    switch (state_) {
        case RaftState ::WAITING:
            // do nothing
            break;
        case RaftState::CANDIDATE:
            after_candidate();
            break;
        case RaftState::FOLLOWER:
            after_follower();
            break;
        case RaftState::LEADER:
            after_leader();
            break;
        default:
            assert(false);
    }

    // Setup up next state
    switch (state) {
        case RaftState::WAITING:
            // Should not go back to waiting.
            assert(false);
        case RaftState::FOLLOWER:
            before_follower();
            break;
        case RaftState::LEADER:
            before_leader();
            break;
        case RaftState::CANDIDATE:
            before_candidate();
            break;
        default:
            assert(false);
    }
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