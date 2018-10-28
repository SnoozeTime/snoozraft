/// CLASS WAS AUTOGENERATED  - DO NOT MODIFY

#pragma once
#include <sstream>
#include <string>

#include "msg_handler.h"
#include "protocol/message.h"
#include "snooz_utils.h"

namespace snooz {
class JoinMessage : public MessageData {
public:
  JoinMessage() = default;

  JoinMessage(std::string address)
      :

        address_(std::move(address)) {}

  // Default move because the types should be trivials..
  JoinMessage(JoinMessage &&other) noexcept = default;
  JoinMessage &operator=(JoinMessage &&other) = default;

    void dispatch(const std::string& from, MessageHandler &handler) override { handler.on_message(from, *this); }

  MessageType message_type() override { return MessageType::JOIN; }

  void unpack(std::string &ss) override {

    size_t offset = 0;

    {
      auto oh = msgpack::unpack(ss.c_str(), ss.size(), offset);
      auto o = oh.get();
      address_ = o.as<std::string>();
    }
  }

  void pack(std::stringstream &ss) override { msgpack::pack(ss, address_); }

  // Then Getters and setters

  void set_address(const std::string &address) { address_ = address; }
  const std::string &address() const { return address_; }

  // double dispatch to avoid casting.
  //  void dispatch(MessageHandler* handler) override {
  //  handler->handle(*this);
  //}

private:
  std::string address_{};
};
}