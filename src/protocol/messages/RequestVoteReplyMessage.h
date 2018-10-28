/// CLASS WAS AUTOGENERATED  - DO NOT MODIFY

#pragma once
#include <sstream>
#include <string>

#include "protocol/message.h"
#include "protocol/msg_handler.h"
#include "snooz_utils.h"

namespace snooz {
class RequestVoteReplyMessage : public MessageData {
public:
  RequestVoteReplyMessage() = default;

  RequestVoteReplyMessage(int term, bool vote_granted)
      :

        term_(std::move(term)),

        vote_granted_(std::move(vote_granted)) {}

  // Default move because the types should be trivials..
  RequestVoteReplyMessage(RequestVoteReplyMessage &&other) noexcept = default;
  RequestVoteReplyMessage &operator=(RequestVoteReplyMessage &&other) = default;

  void dispatch(const std::string &from, MessageHandler &handler) override {
    handler.on_message(from, *this);
  }

  MessageType message_type() override {
    return MessageType::REQUEST_VOTE_REPLY;
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
      vote_granted_ = o.as<bool>();
    }
  }

  void pack(std::stringstream &ss) override {

    msgpack::pack(ss, term_);

    msgpack::pack(ss, vote_granted_);
  }

  // Then Getters and setters

  void set_term(const int &term) { term_ = term; }
  const int &term() const { return term_; }

  void set_vote_granted(const bool &vote_granted) {
    vote_granted_ = vote_granted;
  }
  const bool &vote_granted() const { return vote_granted_; }

  // double dispatch to avoid casting.
  //  void dispatch(MessageHandler* handler) override {
  //  handler->handle(*this);
  //}

private:
  int term_{};

  bool vote_granted_{};
};
}