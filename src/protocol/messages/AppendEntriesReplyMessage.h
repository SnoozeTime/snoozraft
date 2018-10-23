/// CLASS WAS AUTOGENERATED  - DO NOT MODIFY

#pragma once
#include <sstream>
#include <string>

#include "msg_handler.h"
#include "protocol/message.h"
#include "snooz_utils.h"

namespace snooz {
class AppendEntriesReplyMessage : public MessageData {
public:
  AppendEntriesReplyMessage() = default;

  AppendEntriesReplyMessage(int term, bool success)
      :

        term_(std::move(term)),

        success_(std::move(success)) {}

  // Default move because the types should be trivials..
  AppendEntriesReplyMessage(AppendEntriesReplyMessage &&other) noexcept =
      default;
  AppendEntriesReplyMessage &
  operator=(AppendEntriesReplyMessage &&other) = default;

  void dispatch(MessageHandler &handler) override { handler.on_message(*this); }

  MessageType message_type() override {
    return MessageType::APPEND_ENTRIES_REPLY;
  }

  void unpack(std::string &ss) override {

    size_t offset = 0;

    {
      auto oh = msgpack::unpack(ss.c_str(), ss.size(), offset);
      auto o = oh.get();
      term_ = o.as<int>();
    }

    {
      auto oh = msgpack::unpack(ss.c_str(), ss.size(), offset);
      auto o = oh.get();
      success_ = o.as<bool>();
    }
  }

  void pack(std::stringstream &ss) override {

    msgpack::pack(ss, term_);

    msgpack::pack(ss, success_);
  }

  // Then Getters and setters

  void set_term(const int &term) { term_ = term; }
  const int &term() const { return term_; }

  void set_success(const bool &success) { success_ = success; }
  const bool &success() const { return success_; }

  // double dispatch to avoid casting.
  //  void dispatch(MessageHandler* handler) override {
  //  handler->handle(*this);
  //}

private:
  int term_{};

  bool success_{};
};
}