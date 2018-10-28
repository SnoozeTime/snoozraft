/// CLASS WAS AUTOGENERATED  - DO NOT MODIFY

#pragma once
#include <sstream>
#include <string>

#include "msg_handler.h"
#include "protocol/message.h"
#include "snooz_utils.h"

namespace snooz {
class RequestVoteRequestMessage : public MessageData {
public:
  RequestVoteRequestMessage() = default;

  RequestVoteRequestMessage(int term, std::string candidate_id,
                            int last_log_index, int last_log_term)
      :

        term_(std::move(term)),

        candidate_id_(std::move(candidate_id)),

        last_log_index_(std::move(last_log_index)),

        last_log_term_(std::move(last_log_term)) {}

  // Default move because the types should be trivials..
  RequestVoteRequestMessage(RequestVoteRequestMessage &&other) noexcept =
      default;
  RequestVoteRequestMessage &
  operator=(RequestVoteRequestMessage &&other) = default;

  void dispatch(const std::string& from, MessageHandler &handler) override { handler.on_message(from, *this); }

  MessageType message_type() override {
    return MessageType::REQUEST_VOTE_REQUEST;
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
      candidate_id_ = o.as<std::string>();
    }

    {
      auto oh = msgpack::unpack(ss.c_str(), ss.size(), offset);
      auto o = oh.get();
      last_log_index_ = o.as<int>();
    }

    {
      auto oh = msgpack::unpack(ss.c_str(), ss.size(), offset);
      auto o = oh.get();
      last_log_term_ = o.as<int>();
    }
  }

  void pack(std::stringstream &ss) override {

    msgpack::pack(ss, term_);

    msgpack::pack(ss, candidate_id_);

    msgpack::pack(ss, last_log_index_);

    msgpack::pack(ss, last_log_term_);
  }

  // Then Getters and setters

  void set_term(const int &term) { term_ = term; }
  const int &term() const { return term_; }

  void set_candidate_id(const std::string &candidate_id) {
    candidate_id_ = candidate_id;
  }
  const std::string &candidate_id() const { return candidate_id_; }

  void set_last_log_index(const int &last_log_index) {
    last_log_index_ = last_log_index;
  }
  const int &last_log_index() const { return last_log_index_; }

  void set_last_log_term(const int &last_log_term) {
    last_log_term_ = last_log_term;
  }
  const int &last_log_term() const { return last_log_term_; }

  // double dispatch to avoid casting.
  //  void dispatch(MessageHandler* handler) override {
  //  handler->handle(*this);
  //}

private:
  int term_{};

  std::string candidate_id_{};

  int last_log_index_{};

  int last_log_term_{};
};
}