//
// Created by benoit on 18/10/22.
//

#pragma once

#include "zmq/message.h"
#include <random>
#include "protocol/msg_handler.h"
#include "handle.h"

namespace snooz {

class Node;

enum class RaftState {
    WAITING, // Wait for a complete cluster
    LEADER,
    FOLLOWER,
    CANDIDATE
};

/// Finite state machine for the RAFT consensus.
/// 3 possible states
/// - Leader
/// - Follower
/// - Candidate
///
/// 3 nodes is the minimum.
///
/// The RAFT state machine is listening for events/messages
/// Messages can come from:
/// - Another RAFT peer is sending a message
/// - Node layer is adding or removing a peer (so we do not share state between node and RaftFSM)
/// - Timeout events (for example, didn't receive message from LEADER in a long time)
///
/// The RaftFSM will communicate with Node via ZMQ dealer to dealer sockets.
class RaftFSM: public MessageHandler {
public:

    explicit RaftFSM(Node* node);

    void handle(const ZmqMessage& msg);

    void set_state(RaftState state);

    // -----------------------------------
    // HANDLERS FOR SPECIFIC MESSAGES
    // -----------------------------------
    void on_message(const std::string& from, const AppendEntriesRequestMessage &msg) override;
    void on_message(const std::string& from, const AppendEntriesReplyMessage &msg) override;
    void on_message(const std::string& from, const RequestVoteRequestMessage &msg) override;
    void on_message(const std::string& from, const RequestVoteReplyMessage &msg) override;

    std::string leader_addr();

private:

    // -----------------------------------
    // STATE TRANSITIONS
    // ------------------------------------
    void before_candidate();
    void after_candidate();

    void before_leader();
    void after_leader();

    void before_follower();
    void after_follower();

    void after_waiting();

    // ------------------------------------
    // HELPERS TO SEND MESSAGES
    // ------------------------------------
    void send_to_peers(const ZmqMessage& msg);
    void send_to_peer(const std::string& peer_id, const ZmqMessage& msg);

    void send_hearbeat();

    std::random_device rd;     // only used once to initialise (seed) engine
    std::mt19937 rng{rd()};    // random-number engine used (Mersenne-Twister in this case)
    std::uniform_int_distribution<int> uni{1000, 3000}; // guaranteed unbiased

    Node* node_;

    RaftState state_{RaftState::WAITING};

    int cluster_size_{0};
    int term_{0};
    // For election
    // empty or the one that we have voted for.
    std::string voted_for_{};
    int nb_votes_{0};

    std::string leader_addr_;

    constexpr static int minimum_nb_peers_{3}
    ;
    // ------------------------
    // Keep a track of timers to be able to cancel them if needed
    // TODO use a container if become too wild

    // Can be used independently at different state of the FSM
    // - waiting -> Wait for minimum number of peers in the network
    // - following -> Timeout if not heartbeat ot start new election
    // - leader -> to send heartbeat if haven't sent anything yet.
    Handle raft_timer_;
};
}



