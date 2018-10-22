#pragma once

#include <stddef.h>
#include <sstream>
#include <msgpack.hpp>
#include <vector>
#include <iostream>

#include "zmq/message.h"

namespace snooz {


  enum class MessageType {
    {% for enum_name in enums %}
	  {{ enum_name}},{% endfor %}
	  NONE,
    };

    class MessageData {
    public:
        virtual ~MessageData() = default;

        virtual MessageType message_type() = 0;
        virtual void unpack(std::string& ss) = 0;
        virtual void pack(std::stringstream& ss) = 0;
    };

    std::unique_ptr<MessageData> create_data(const MessageType& type);

    class Message {
    public:
        Message() = default;
        Message(Message&& other) noexcept = default;
        Message& operator=(Message&& other) = default;
        virtual ~Message() = default;

        MessageType message_type() const {
            if (data_) {
                return data_->message_type();
            }
            return MessageType::NONE;
        }
        void set_message_type(MessageType type) {
            type = type_;
        }

        // todo error handling when no data.
        const MessageData* data() const {
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
	void pack(ZmqMessage &packet) const {
	  // First element is the message type.
	  packet.add_frame(std::to_string(static_cast<int>(type_)));
	  std::stringstream ss;
	  data_->pack(ss);
	  packet.add_frame(ss.str());
	}

	void unpack(const ZmqMessage& msg) {
	  // first is addr. Second is message type. third is the binary data to unpack.
	  const auto& frames = msg.frames();
	  assert(frames.size() == 3);

	  auto msg_type_int = std::stoi(frames[1]);
	  type_ = static_cast<MessageType>(msg_type_int);

	  auto data_buf = frames[2];
	  data_ = create_data(type_);
	  data_->unpack(data_buf);
	}
    private:
        MessageType type_{MessageType::NONE};
        std::unique_ptr<MessageData> data_;
    };


}