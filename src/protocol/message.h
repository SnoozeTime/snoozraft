#pragma once

#include <iostream>
#include <msgpack.hpp>
#include <sstream>
#include <stddef.h>
#include <vector>

#include "zmq/message.h"

namespace snooz {
class MessageHandler;

enum class MessageType {

  HEARTBEAT,
  PEER_LIST,
  JOIN,
  REQUEST_VOTE_REQUEST,
  REQUEST_VOTE_REPLY,
  APPEND_ENTRIES_REQUEST,
  APPEND_ENTRIES_REPLY,
  NONE,
};

class MessageData {
public:
  virtual ~MessageData() = default;

  virtual MessageType message_type() = 0;
  virtual void unpack(std::string &ss) = 0;
  virtual void pack(std::stringstream &ss) = 0;
  virtual void dispatch(const std::string &from, MessageHandler &handler);
};

std::unique_ptr<MessageData> create_data(const MessageType &type);

class Message {
public:
  Message() = default;
  explicit Message(std::unique_ptr<MessageData> &&data)
      : data_{std::move(data)} {
    if (data_) {
      type_ = data_->message_type();
    }
  }

  MessageType message_type() const {
    if (data_) {
      return data_->message_type();
    }
    return MessageType::NONE;
  }
  void set_message_type(MessageType type) { type = type_; }

  // todo error handling when no data.
  const MessageData *data() const {
    if (data_) {
      return data_.get();
    } else {
      return nullptr;
    }
  }

  void set_data(std::unique_ptr<MessageData> data) {
    data_ = std::move(data);
    type_ = data_->message_type();
  }

  ///
  /// \param packet message that already contains the address!
  ZmqMessage pack() const {
    std::stringstream ss;
    data_->pack(ss);
    return ZmqMessage{std::to_string(static_cast<int>(type_)), ss.str()};
  }

  void unpack(const ZmqMessage &msg) {
    // first is addr. Second is message type. third is the binary data to
    // unpack.
    const auto &frames = msg.frames();
    assert(frames.size() == 3);

    auto msg_type_int = std::stoi(frames[1]);
    type_ = static_cast<MessageType>(msg_type_int);

    auto data_buf = frames[2];
    data_ = create_data(type_);
    data_->unpack(data_buf);
  }

  void dispatch(const std::string &from, MessageHandler &handler);

private:
  MessageType type_{MessageType::NONE};
  std::unique_ptr<MessageData> data_;
};

template <typename T, typename... Args> Message make_message(Args &&... args) {
  return Message{std::make_unique<T>(std::forward<Args>(args)...)};
}
}