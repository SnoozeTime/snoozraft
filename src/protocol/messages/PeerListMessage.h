/// CLASS WAS AUTOGENERATED  - DO NOT MODIFY

#pragma once
#include <sstream>
#include <string>

#include "protocol/message.h"
#include "msg_handler.h"
namespace snooz {
class PeerListMessage : public MessageData {
public:
  PeerListMessage() = default;

  PeerListMessage(std::vector<std::string> peers)
      :

        peers_(std::move(peers)) {}


  void dispatch(MessageHandler& handler) override {
    handler.on_message(*this);
  }

  // Default move because the types should be trivials..
  PeerListMessage(PeerListMessage &&other) noexcept = default;
  PeerListMessage &operator=(PeerListMessage &&other) = default;

  MessageType message_type() override { return MessageType::PEER_LIST; }

  void unpack(std::string &ss) override {

    size_t offset = 0;

    {
      auto oh = msgpack::unpack(ss.c_str(), ss.size(), offset);
      auto o = oh.get();
      peers_ = o.as<std::vector<std::string>>();
    }
  }

  void pack(std::stringstream &ss) override { msgpack::pack(ss, peers_); }

  // Then Getters and setters

  void set_peers(const std::vector<std::string> &peers) { peers_ = peers; }
  const std::vector<std::string> &peers() const { return peers_; }

  // double dispatch to avoid casting.
  //  void dispatch(MessageHandler* handler) override {
  //  handler->handle(*this);
  //}

private:
  std::vector<std::string> peers_{};
};
}