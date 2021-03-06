/// CLASS WAS AUTOGENERATED  - DO NOT MODIFY

#pragma once
#include <sstream>
#include <string>

#include "protocol/message.h"
#include "protocol/msg_handler.h"
#include "snooz_utils.h"

namespace snooz {
class HeartbeatMessage : public MessageData {
public:
  HeartbeatMessage() = default;

  // Default move because the types should be trivials..
  HeartbeatMessage(HeartbeatMessage &&other) noexcept = default;
  HeartbeatMessage &operator=(HeartbeatMessage &&other) = default;

  void dispatch(const std::string &from, MessageHandler &handler) override {
    handler.on_message(from, *this);
  }

  MessageType message_type() override { return MessageType::HEARTBEAT; }

  void unpack(std::string &ss) override { UNUSED(ss); }

  void pack(std::stringstream &ss) override { UNUSED(ss); }

  // Then Getters and setters

  // double dispatch to avoid casting.
  //  void dispatch(MessageHandler* handler) override {
  //  handler->handle(*this);
  //}

private:
};
}