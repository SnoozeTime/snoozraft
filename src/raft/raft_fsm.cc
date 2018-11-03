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

RaftFSM::RaftFSM(Node* node):
    log_(boost::log::keywords::channel = "RAFT"),
    node_(node) {

    //auto random_integer = uni(rng);
    //BOOST_LOG(log_)(info) << "Will wait " << random_integer << "before timeout";
    raft_timer_ = node_->loop().add_timeout(1s,
            [this] () {
                // If there are enough peers, start raft consensus.
                // -1 to say I am included
                if (node_->peers().size() < minimum_nb_peers_ - 1) {
                    BOOST_LOG(log_) << "Less than 3 nodes. Will wait more before starting";
                } else {
                    BOOST_LOG(log_) << "Will start RAFT";
                    // node_->loop().remove_timeout(start_timer_);
                    set_state(RaftState::FOLLOWER);
                }
            });

}

void RaftFSM::handle(const ZmqMessage& msg) {

}

void RaftFSM::before_candidate() {
    BOOST_LOG(log_) << "RAFT -> transition to candidate";
    term_++;

    // TODO First approximation of cluster size. Does it represent the reality?
    cluster_size_ = node_->peers().size() + 1;
    nb_votes_ = 1;
    voted_for_ = node_->my_address();

    auto msg = make_message<RequestVoteRequestMessage>(term_, node_->my_address(), 0, 0);
    send_to_peers(msg.pack());

    // Start timer in case it takes too much time.
}


void RaftFSM::before_leader() {
    node_->loop().remove_timeout(raft_timer_);

    // me!
    leader_addr_ = node_->my_address();

    // Send heartbeat to tell the world I am new leader
    send_hearbeat();

    // Set up the timeout for sending heartbeat.
    raft_timer_ = node_->loop().add_timeout(500ms, [this]() { send_hearbeat(); });
}

void RaftFSM::before_follower() {
    // Timeout for next election. This can be canceled when receiving from leader.
    BOOST_LOG(log_) << "Before follower transition";
    // Remove whatever timeout was used before

    auto random_integer = uni(rng);
    BOOST_LOG(log_) << "Timeout for follower is " << random_integer << " milliseconds";
    raft_timer_ = node_->loop().add_timeout(std::chrono::milliseconds{random_integer}, [this] {
        BOOST_LOG(log_) << "Follower timeout. Will start new election";
        // should be candidate here. Just for testing
        set_state(RaftState::CANDIDATE);
    });
}

void RaftFSM::send_to_peers(const ZmqMessage &msg) {
    for (auto& peer: node_->peers()) {
        peer.second.send(msg);
    }
}

void RaftFSM::after_waiting() {
    node_->loop().remove_timeout(raft_timer_);
}

void RaftFSM::after_candidate() {
    node_->loop().remove_timeout(raft_timer_);
}

void RaftFSM::after_leader() {

}

void RaftFSM::after_follower() {
    node_->loop().remove_timeout(raft_timer_);
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
            after_waiting();
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

    state_ = state;
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

void RaftFSM::on_message(const std::string& from, const AppendEntriesRequestMessage &msg) {

    /*
     * Receiver implementation:
        1.  Reply false if term < currentTerm (§5.1)
        2.  Reply false if log doesn’t contain an entry at prevLogIndex
        whose term matches prevLogTerm (§5.3)
        3.  If an existing entry conflicts with a new one (same index
        but different terms), delete the existing entry and all that
        follow it (§5.3)
        4.  Append any new entries not already in the log
        5.  If leaderCommit > commitIndex, set commitIndex =
        min(leaderCommit, index of last new entry)

     */
    // When follower, reset the timer and add entries. We reset the
    // timer so that a follower does not try to start a new election
    // when the leader is obviously not dead.
    if (state_ == RaftState::FOLLOWER) {

        // TODO is it true though? Maybe compare with current term...
        leader_addr_ = from;

        BOOST_LOG(log_) << "Follower received AppendEntriesRequest";
        auto* t = node_->loop().get_timer(raft_timer_);
        assert(t != nullptr);
        t->reset();

        // check the term.

        // Check previous log index.

        
    } else if (state_ == RaftState::CANDIDATE) {
        leader_addr_ = from;
        BOOST_LOG(log_) << "Candidates receive AppendEntriesRequest from leader. Go back to being FOLLOWER";
        set_state(RaftState::FOLLOWER);
    }
}

void RaftFSM::on_message(const std::string& from, const AppendEntriesReplyMessage &msg) {
}

void RaftFSM::on_message(const std::string& from, const RequestVoteRequestMessage &msg) {

    BOOST_LOG(log_) << "ME: " << node_->my_address() << ":received request vote request message from " << msg.candidate_id() << " addr " << from << " for term " << msg.term();
    bool accept = voted_for_.empty();
    // First draft, just send OK idecodedf haven't voted already.
    // TODO include term condition
    if (accept) {
        BOOST_LOG(log_) << "ME: " << node_->my_address() << "Will accept request from " << msg.candidate_id();
        voted_for_ = msg.candidate_id();
    } else {
        BOOST_LOG(log_) << "ME: " << node_->my_address() << "Reject request. Already voted for " << voted_for_;
    }

    auto reply = make_message<RequestVoteReplyMessage>(term_, accept);
    send_to_peer(msg.candidate_id(), reply.pack());
}

void RaftFSM::on_message(const std::string& from, const RequestVoteReplyMessage &msg) {

    BOOST_LOG(log_) << "ME: " << node_->my_address() << ": received RequestVoteReply message from " << from;
    if (state_ == RaftState::CANDIDATE) {
        if (msg.vote_granted()) {
            nb_votes_++;
            // majority.
            auto minimum = ceil(static_cast<double>(cluster_size_) / 2);
            BOOST_LOG(log_) << " Will need at least " << minimum << " votes";
            if (nb_votes_ >= minimum) {
                // I AM THE GREAT LEADER
                BOOST_LOG(log_) << "ME: " << node_->my_address() << ": Becoming leader";
                set_state(RaftState::LEADER);
            } else {
                BOOST_LOG(log_) << "Have " << nb_votes_ << " but need at least " << minimum;
            }
        } else {
            BOOST_LOG(log_) << "Request was not granted *sad face*";
        }
    } else {
        BOOST_LOG(log_) << "Do not process as I AM NOT A CANDIDATE";
    }

}

void RaftFSM::send_to_peer(const std::string& peer_id, const ZmqMessage &msg) {
    BOOST_LOG(log_) << "Will send to " << peer_id;
    auto peer = node_->peers().find(peer_id);
    if (peer != node_->peers().end()) {
        (*peer).second.send(msg);
    } else {
        BOOST_LOG(log_) << " Cannot find peer: " << peer_id;
    }
}

void RaftFSM::send_hearbeat() {
    BOOST_LOG(log_) << "Leader sends heartbeat.";
    auto msg = make_message<AppendEntriesRequestMessage>(
            term_, // current term
            node_->my_client_address(), // Address of the public interface to the leader.
            0,
            0,
            std::vector<std::string>(),
            0);
    send_to_peers(msg.pack());
}

std::string RaftFSM::leader_addr() {
    return leader_addr_;
}

bool RaftFSM::submit_entry(const std::string &entry) {
    if (state_ != RaftState::LEADER) {
        BOOST_LOG(log_) << "Submitted entry to node, but is not leader...";
        return false;
    }


    return true;
}


}