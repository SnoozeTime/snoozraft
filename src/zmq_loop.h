//
// Created by benoit on 18/10/17.
//

#ifndef SNOOZRAFT2_ZMQ_LOOP_H
#define SNOOZRAFT2_ZMQ_LOOP_H

#include <vector>
#include "zmq_helpers.h"

namespace snooz {

class ZmqLoop;

/// Class that wrap the low-level zmq socket. This is used with the ZmqLoop that
/// will provide reactor-like functionality to hide the complexitiy behind poll.
class ZmqSocket {
public:

    // Create the socket and add itself to the loop.
    ZmqSocket(zmq::context_t context, ZmqLoop *loop, int type);

    // Remove itself from the loop.
    ~ZmqSocket();

private:
    zmq::socket_t socket_;
    ZmqLoop *loop_;
};

/// Reactor to hide zmq::poll and timer logic
class ZmqLoop {
public:

    // Non copyable.
    ZmqLoop(const ZmqLoop& loop) = delete;
    ZmqLoop& operator=(const ZmqLoop& loop) = delete;

    /// Run the loop. It is a blocking call
    void run();

    /// add socket to the listening loop
    void add_listener(ZmqSocket *sock);

    /// Remove socket from the listening loop
    void remove_listener(ZmqSocket *sock);
private:

    void build_pollset();

    // The pollset will increase/decrease when we add/remove listeners.
    std::vector<zmq::pollitem_t> items_;

    // socket we listen. The pointers are non-owning so when adding a listner, the caller
    // should be careful that the pointer is not deleted...
    std::vector<ZmqSocket*> listeners_;

};
}


#endif //SNOOZRAFT2_ZMQ_LOOP_H
