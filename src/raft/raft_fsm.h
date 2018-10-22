//
// Created by benoit on 18/10/22.
//

#pragma once

#include "zmq/message.h"
#include <random>

namespace snooz {

class Node;

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
class RaftFSM {
public:

    explicit RaftFSM(Node* node);

    void handle(const ZmqMessage& msg);
private:
    enum class State {
        LEADER,
        FOLLOWER,
        CANDIDATE
    };

    /// transition before becoming a candidate.
    void before_candidate();

    std::random_device rd;     // only used once to initialise (seed) engine
    std::mt19937 rng{rd()};    // random-number engine used (Mersenne-Twister in this case)
    std::uniform_int_distribution<int> uni{1000, 3000}; // guaranteed unbiased

    Node* node_;

    State state_{State::FOLLOWER};

    int term_{0};
    // For election
    // empty or the one that we have voted for.
    std::string voted_for{};
    int nb_votes_{0};
};
}



