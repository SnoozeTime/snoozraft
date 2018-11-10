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

RaftFSM::RaftFSM(Node* node, std::string store_filepath):
    log_(boost::log::keywords::channel = "RAFT"),
    node_(node),
    stored_state_{store_filepath} {

    UNUSED(last_applied_);
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


void RaftFSM::before_candidate() {
    BOOST_LOG(log_) << "RAFT -> transition to candidate";
    stored_state_.set_term(stored_state_.term() + 1);

    // TODO First approximation of cluster size. Does it represent the reality?
    cluster_size_ = node_->peers().size() + 1;
    nb_votes_ = 1;
    stored_state_.set_voted_for(node_->my_address());

    auto msg = make_message<RequestVoteRequestMessage>(
                    stored_state_.term(),
                    node_->my_address(),
                    0,
                    0);
    send_to_peers(msg.pack());

    // Start timer in case it takes too much time.
}


void RaftFSM::before_leader() {
    node_->loop().remove_timeout(raft_timer_);

    // me! --- State for the leader
    leader_addr_ = node_->my_address();
    for (auto& peer: node_->peers()) {
        next_indexes_[peer.first] = stored_state_.get_log().size() + 1;
        match_indexes_[peer.first] = 0; 
    }

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

        client_leader_addr_ = msg.leader_id();

        if (!msg.entries().empty()) {
            BOOST_LOG(log_) << "Received entries to append to log";
            // check the term.
            // Check previous log index.
            BOOST_LOG(log_) << "Term in msg " << msg.term();
            if (msg.term() < stored_state_.term()) {
                // Node that sent this message is not the leader anymore.
                send_nok_reply(from);
                return;
            }

            BOOST_LOG(log_) << "prev log index " << msg.prev_log_index();
            BOOST_LOG(log_) << "prev log term" << msg.prev_log_term();
            // Need to check if our previous log index match with what was sent.
            // if nothing, always accept what comes from the server?
            auto& entry_log = stored_state_.get_log();

            if (msg.prev_log_index() > 0) {
                // Now, if the prev  index an prev term are matching, we can
                // append from there. If not matching, we just send back 'no'
                if (entry_log.size() < msg.prev_log_index()) {
                    BOOST_LOG(log_) << "No entry at " << msg.prev_log_index();
                    send_nok_reply(from);
                    return;
                }

                if (entry_log[msg.prev_log_index()].term() != msg.prev_log_term()) {
                    BOOST_LOG(log_) << "Previous entry term does not match msg prev_term";
                    BOOST_LOG(log_) << entry_log[msg.prev_log_index()].term() << " VS " << msg.prev_log_term();
                    send_nok_reply(from);
                    return;
                }
            }

            entry_log.overwrite(msg.prev_log_index()+1, msg.entries());
            send_ok_reply(from);
            return;
        }

        send_ok_reply(from);
        return;
    } else if (state_ == RaftState::CANDIDATE) {
        leader_addr_ = from;
        BOOST_LOG(log_) << "Candidates receive AppendEntriesRequest from leader. Go back to being FOLLOWER";
        client_leader_addr_ = msg.leader_id();
        set_state(RaftState::FOLLOWER);
        return;
    }
}

void RaftFSM::on_message(const std::string& from, const AppendEntriesReplyMessage &msg) {
}

void RaftFSM::on_message(const std::string& from, const RequestVoteRequestMessage &msg) {

    BOOST_LOG(log_) << "ME: " << node_->my_address() << ":received request vote request message from " << msg.candidate_id() << " addr " << from << " for term " << msg.term();
    bool accept = stored_state_.voted_for().empty();
    // First draft, just send OK idecodedf haven't voted already.
    // TODO include term condition
    if (accept) {
        BOOST_LOG(log_) << "ME: " << node_->my_address() << "Will accept request from " << msg.candidate_id();
        stored_state_.set_voted_for(msg.candidate_id());
    } else {
        BOOST_LOG(log_) << "ME: " << node_->my_address() << "Reject request. Already voted for " << stored_state_.voted_for();
    }

    auto reply = make_message<RequestVoteReplyMessage>(
                    stored_state_.term(),
                    accept);
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
//   AppendEntriesRequestMessage(int term, std::string leader_id,
  //                            int prev_log_index, int prev_log_term,
    //                          std::vector<std::tuple<int, std::string>> entries,
      //                        int leader_commit)
    auto msg = make_message<AppendEntriesRequestMessage>(
            stored_state_.term(), // current term
            node_->my_client_address(), // Address of the public interface to the leader.
            0,
            0,
            std::vector<std::tuple<int, std::string>>(),
            0);
    send_to_peers(msg.pack());
}

std::string RaftFSM::leader_addr() {
    return leader_addr_;
}

const std::string& RaftFSM::leader_client_addr() const {
    return client_leader_addr_;
}

bool RaftFSM::append_to_log(const std::string &entry) {
    if (state_ != RaftState::LEADER) {
        BOOST_LOG(log_) << "Submitted entry to node, but is not leader...";
        return false;
    }

    // First add to the log !
    stored_state_.append_log_entry(LogEntry(stored_state_.term(), entry));

    // Then, send a message to everybody. Everybody should send either ok or not OK + reason
    auto log = stored_state_.get_log();
    for (auto& peer: node_->peers()) {
        // For each peers, need to check what we assume their last index is. If
        // not found, should default to lastLogIndex+1
        // Will update on AppendEntriesReply.
        std::vector<std::tuple<int, std::string>> entries;
        auto from_index = next_indexes_[peer.first];

        BOOST_LOG(log_) << "will send log to " << peer.first << " from index " << from_index;
        // btw, index starts at 1 and finish at size
        for (int i = from_index; i <= log.size(); i++) {
            entries.emplace_back(std::make_tuple(
                                    log[i].term(),
                                    log[i].content()));
                    }
        int prev_log_term = from_index > 1 ? log[from_index-1].term() : 0;
        int prev_log_index = from_index - 1;
        auto msg = make_message<AppendEntriesRequestMessage>(
                        stored_state_.term(), // current term
                        node_->my_client_address(),
                        prev_log_index,
                        prev_log_term,
                        entries,
                        commit_index_);
        peer.second.send(msg.pack());

    }

    return true;
}
inline void RaftFSM::send_ok_reply(const std::string& target) {
    auto msg = make_message<AppendEntriesReplyMessage>(stored_state_.term(), true);
    send_to_peer(target, msg.pack());
}

inline void RaftFSM::send_nok_reply(const std::string& target) {
    auto msg = make_message<AppendEntriesReplyMessage>(stored_state_.term(), false);
    send_to_peer(target, msg.pack());
}



// END OF NAMESPACE
}
