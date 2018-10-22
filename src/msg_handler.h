//
// Double-dispatch for Message so that we don't switch + static cast.
//

#pragma once

#include "protocol/message.h"

namespace snooz {

class PeerListMessage;

class MessageHandler {
public:
    void dispatch(Message& message) {
        message.dispatch(*this);
    }

    // Fallback method - need to override
    virtual void on_message(const MessageData &msg) = 0;
    virtual void on_message(const PeerListMessage &msg) = 0;
};
}
