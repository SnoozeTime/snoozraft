//
// Double-dispatch for Message so that we don't switch + static cast.
//

#pragma once

#include "protocol/message.h"

namespace snooz {
  {% for msg_class in classes %}class {{ msg_class.name }}Message;
  {% endfor %}

class MessageHandler {
public:
    void dispatch(const std::string& from, Message& message) {
        message.dispatch(from, *this);
    }

    // Fallback method - need to override
    virtual void on_message(const std::string& from, const MessageData& msg) {};

    // Specialization for children classes
    {% for msg_class in classes %}virtual void on_message(const std::string& from, const {{ msg_class.name }}Message &msg) {};
    {% endfor %}

};
}
