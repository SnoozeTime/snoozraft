//
// Created by benoit on 18/10/22.
//

#include "rpc.h"
#include <cassert>

namespace snooz {
std::unique_ptr<RaftMessage> parse_raft_message(const ZmqMessage& msg) {

    const auto& frames = msg.frames();
    assert(frames.size() > 3);
    assert(frames[1] == "RAFT");

    RaftMessage *raft_msg;
    if (frames[2] == "RequestVoteRequest") {
        raft_msg = new RequestVoteRequest();
    } else {
        // not implemented yet.
        assert(false);
    }

    raft_msg->from_message(msg);

    return std::unique_ptr<RaftMessage>(raft_msg);
}

// ---------------------------------------------------------
// RequestVote is sent by a candidate during an election
//
// ---------------------------------------------------------
void RequestVoteRequest::from_message(const snooz::ZmqMessage &msg) {
    const auto& frames = msg.frames();

    // Router addr | msg type | raft msg type | term | candidate_id
    assert(frames.size() == 5);

    term = std::stoi(frames[3]);
    candidate_id = frames[4];
}

ZmqMessage RequestVoteRequest::to_message() const {
    return ZmqMessage{"RequestVoteRequest",
                      std::to_string(term),
                      candidate_id};
}

}