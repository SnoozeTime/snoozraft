//
// Created by benoit on 18/10/22.
//

#pragma once

#include "zmq/message.h"
#include <memory>

namespace snooz {

struct RaftMessage {
    virtual ~RaftMessage() = default;
    virtual void from_message(const ZmqMessage& msg) = 0;
    virtual ZmqMessage to_message() const = 0;
};

std::unique_ptr<RaftMessage> parse_raft_message(const ZmqMessage& msg);

// ---------------------------------------------------------
// RequestVote is sent by a candidate during an election
//
// ---------------------------------------------------------
struct RequestVoteRequest: public RaftMessage {

    void from_message(const ZmqMessage& msg) override;
    ZmqMessage to_message() const override;

    // current term of the election
    int term;

    // candidate requesting the vote.
    std::string candidate_id;
};

}



