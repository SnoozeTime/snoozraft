#include "message.h"
#include "msg_handler.h"

{% for msg in msgs %}
#include "messages/{{ msg.name }}Message.h"{% endfor %}

namespace snooz {
  std::unique_ptr<MessageData> create_data(const MessageType& type) {
    switch (type) {
      {% for msg in msgs %}
    case MessageType::{{ msg.message_type }}:
    return std::make_unique<{{ msg.name}}Message>();{% endfor %}
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
