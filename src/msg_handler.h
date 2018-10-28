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
    void dispatch(const std::string& from, Message& message) {
        message.dispatch(from, *this);
    }

    // Fallback method - need to override
    virtual void on_message(const std::string& from, const MessageData &msg) {};
    virtual void on_message(const std::string& from, const PeerListMessage &msg) {};
    virtual void on_message(const std::string& from, const HeartbeatMessage &msg) {};
    virtual void on_message(const std::string& from, const JoinMessage &msg) {};
    virtual void on_message(const std::string& from, const AppendEntriesRequestMessage &msg) {};
    virtual void on_message(const std::string& from, const AppendEntriesReplyMessage &msg) {};
    virtual void on_message(const std::string& from, const RequestVoteRequestMessage &msg) {};
    virtual void on_message(const std::string& from, const RequestVoteReplyMessage &msg) {};
};
}
