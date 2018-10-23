#include "message.h"
#include "msg_handler.h"


#include "messages/HeartbeatMessage.h"
#include "messages/PeerListMessage.h"
#include "messages/JoinMessage.h"

namespace snooz {
  std::unique_ptr<MessageData> create_data(const MessageType& type) {
    switch (type) {
      
    case MessageType::HEARTBEAT:
    return std::make_unique<HeartbeatMessage>();
    case MessageType::PEER_LIST:
    return std::make_unique<PeerListMessage>();
    case MessageType::JOIN:
    return std::make_unique<JoinMessage>();
    default:
      assert(false);
    }
  }

  void Message::dispatch(MessageHandler &handler) {
    if (data_) {
      data_->dispatch(handler);
    }
  }

  void MessageData::dispatch(MessageHandler &handler) {
    handler.on_message(*this);
  }
}