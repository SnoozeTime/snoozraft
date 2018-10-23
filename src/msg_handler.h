//
// Double-dispatch for Message so that we don't switch + static cast.
//

#pragma once

#include "protocol/message.h"

namespace snooz {

class PeerListMessage;
class HeartbeatMessage;
class JoinMessage;
class AppendEntriesRequestMessage;
class AppendEntriesReplyMessage;
class RequestVoteRequestMessage;
class RequestVoteReplyMessage;

class MessageHandler {
public:
    void dispatch(Message& message) {
        message.dispatch(*this);
    }

    // Fallback method - need to override
    virtual void on_message(const MessageData &msg) {};
    virtual void on_message(const PeerListMessage &msg) {};
    virtual void on_message(const HeartbeatMessage &msg) {};
    virtual void on_message(const JoinMessage &msg) {};
    virtual void on_message(const AppendEntriesRequestMessage &msg) {};
    virtual void on_message(const AppendEntriesReplyMessage &msg) {};
    virtual void on_message(const RequestVoteRequestMessage &msg) {};
    virtual void on_message(const RequestVoteReplyMessage &msg) {};
};
}
