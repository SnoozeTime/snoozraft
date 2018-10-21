//
// Created by benoit on 18/10/17.
//

#pragma once

#include <vector>
#include "zmq_helpers.h"
#include <functional>
#include <memory>
#include <unordered_map>

#include "../handle.h"

namespace snooz {

using ZmqSocketPtr = void*;
using SocketCallback = std::function<void ()>;
using SocketCallbackPtr = std::shared_ptr<std::function<void ()>>;
using TimerCallback = std::function<void ()>;

/// Timer is used internally by the loop.
class Timer {
public:

    ///
    /// \param timeout wait time before callback
    /// \param cb callback to execute.
    /// \param is_recurrent true if needs cb to be executed every timeout
    Timer(std::chrono::milliseconds timeout, TimerCallback cb, bool is_recurrent);

    /// Reset the deadline
    void reset();

    ///
    /// \return true if deadline is already elapsed
    bool is_elapsed() const;

    /// Will calculate how much time is left before the deadline. If it is passed, will
    /// return 0
    /// \return Time before deadline or 0
    std::chrono::milliseconds remaining() const;


    /// Equal operator for std::find
    /// \param other
    /// \return true if same timeout value.
    bool operator==(const Timer& other) const;

    /// Execute callback
    void execute() const;

    ///
    /// \return true if timer will be rescheduled after execution.
    bool is_recurrent() const;

private:

    std::chrono::milliseconds timeout_;
    std::chrono::system_clock::time_point deadline_;
    TimerCallback cb_;
    bool is_recurrent_;

};


/// Reactor for the ZMQ sockets. It will poll the sockets and execute callbacks assotiated
/// to them. It also supports timers (recurrent or not)
class ZmqLoop {
public:

    explicit ZmqLoop(zmq::context_t *context);

    /// Add a listener (zmq::socket_t) to the loop
    ///
    /// \param socket Socket to listen. Should not be deleted before removed from the loop
    /// \param cb callback to execute when the socket has some pending messages
    void add_zmq_socket(zmq::socket_t& socket, SocketCallback cb);


    /// Add a timer to the loop
    ///
    /// \param timeout
    /// \param cb
    /// \param is_recurrent
    /// \return timer that was created
    Handle add_timeout(std::chrono::milliseconds timeout, TimerCallback cb, bool is_recurrent=true);

    /// Remove timer from the loop. It will only be removed at the end of a loop so the callback
    /// might be executed once before it is actually removed from the loop.
    ///
    /// TODO Fix that. Can add a "is_active" field to the timer object
    ///
    /// \param to_remove handle to the timer to remove
    void remove_timeout(const Handle& handle);

    /// Main function of the loop. The loop itself. It is a blocking call
    void run();

    // Stop the loop. It is connecting to the shutdown socket and BAM.
    void shutdown();

private:

    void build_pollset();
    std::chrono::milliseconds next_timeout() const;

    zmq::context_t *context_;
    zmq::socket_t shutdown_socket_;
    bool should_run{true};


    // The real mapping of socket to listen TO callback to call
    std::unordered_map<ZmqSocketPtr, SocketCallbackPtr> registered_sockets_;

    // Next two are built by the build_pollset function
    // pollset to listen to.
    std::vector<zmq::pollitem_t> poll_items_;
    // callback to call for the pollitem which has the same index
    std::vector<SocketCallbackPtr> callbacks_;

    HandleManager<Timer> timer_manager_;
    std::vector<Handle> timer_handles_;
    std::vector<Handle> handles_to_remove_;
};

}
