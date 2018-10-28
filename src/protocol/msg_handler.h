//
// Double-dispatch for Message so that we don't switch + static cast.
//

#pragma once

#include "protocol/message.h"

namespace snooz {
class HeartbeatMessage;
class PeerListMessage;
class JoinMessage;
class RequestVoteRequestMessage;
class RequestVoteReplyMessage;
class AppendEntriesRequestMessage;
class AppendEntriesReplyMessage;

class MessageHandler {
public:
  void dispatch(const std::string &from, Message &message) {
    message.dispatch(from, *this);
  }

  // Fallback method - need to override
  virtual void on_message(const std::string &from, const MessageData &msg){};

  // Specialization for children classes
  virtual void on_message(const std::string &from,
                          const HeartbeatMessage &msg){};
  virtual void on_message(const std::string &from,
                          const PeerListMessage &msg){};
  virtual void on_message(const std::string &from, const JoinMessage &msg){};
  virtual void on_message(const std::string &from,
                          const RequestVoteRequestMessage &msg){};
  virtual void on_message(const std::string &from,
                          const RequestVoteReplyMessage &msg){};
  virtual void on_message(const std::string &from,
                          const AppendEntriesRequestMessage &msg){};
  virtual void on_message(const std::string &from,
                          const AppendEntriesReplyMessage &msg){};
};
}