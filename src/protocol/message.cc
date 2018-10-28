#include "message.h"
#include "msg_handler.h"


#include "messages/HeartbeatMessage.h"
#include "messages/PeerListMessage.h"
#include "messages/JoinMessage.h"
#include "messages/RequestVoteRequestMessage.h"
#include "messages/RequestVoteReplyMessage.h"
#include "messages/AppendEntriesRequestMessage.h"
#include "messages/AppendEntriesReplyMessage.h"

namespace snooz {
  std::unique_ptr<MessageData> create_data(const MessageType& type) {
    switch (type) {
      
    case MessageType::HEARTBEAT:
    return std::make_unique<HeartbeatMessage>();
    case MessageType::PEER_LIST:
    return std::make_unique<PeerListMessage>();
    case MessageType::JOIN:
    return std::make_unique<JoinMessage>();
    case MessageType::REQUEST_VOTE_REQUEST:
    return std::make_unique<RequestVoteRequestMessage>();
    case MessageType::REQUEST_VOTE_REPLY:
    return std::make_unique<RequestVoteReplyMessage>();
    case MessageType::APPEND_ENTRIES_REQUEST:
    return std::make_unique<AppendEntriesRequestMessage>();
    case MessageType::APPEND_ENTRIES_REPLY:
    return std::make_unique<AppendEntriesReplyMessage>();
    default:
      assert(false);
    }
  }

  void Message::dispatch(const std::string& from, MessageHandler &handler) {
    if (data_) {
      data_->dispatch(from, handler);
    }
  }

  void MessageData::dispatch(const std::string& from, MessageHandler &handler) {
    handler.on_message(from, *this);
  }
}