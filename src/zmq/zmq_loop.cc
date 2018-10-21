//
// Created by benoit on 18/10/17.
//

#include "zmq_loop.h"
#include <iostream>

namespace snooz {
using namespace std::literals;
using std::chrono::milliseconds;


Timer::Timer(milliseconds timeout, snooz::TimerCallback cb, bool is_recurrent):
    timeout_(timeout),
    deadline_(std::chrono::system_clock::now() + timeout_),
    cb_(std::move(cb)),
    is_recurrent_(is_recurrent){

}


void Timer::reset() {
    deadline_ = std::chrono::system_clock::now() + timeout_;
}

bool Timer::is_elapsed() const {
    return std::chrono::system_clock::now() > deadline_;
}

milliseconds Timer::remaining() const {
    auto remaining = std::chrono::duration_cast<milliseconds>(deadline_ - std::chrono::system_clock::now());

    if (remaining < 0ms) {
        return 0ms;
    } else {
        return remaining;
    }
}

void Timer::execute() const {
    cb_();
}

bool Timer::operator==(const Timer& other) const {
    return timeout_ == other.timeout_;
}

bool Timer::is_recurrent() const {
    return is_recurrent_;
}

ZmqLoop::ZmqLoop(zmq::context_t *context):
    context_(context),
    shutdown_socket_{*context, ZMQ_PULL}{
    // Hum what happens if multiple loop?
    shutdown_socket_.bind("inproc://zmq_loop");

    add_zmq_socket(shutdown_socket_, [this] () {
        should_run = false;
    });
}

void ZmqLoop::shutdown() {
    zmq::socket_t to_shutdown{*context_, ZMQ_PUSH};
    to_shutdown.connect("inproc://zmq_loop");
    s_send(to_shutdown, "bye bye");
}

void ZmqLoop::add_zmq_socket(zmq::socket_t &socket, SocketCallback cb) {
    registered_sockets_.insert(std::make_pair(
            (void*) socket,
            std::make_shared<SocketCallback>(std::move(cb))
    ));

    build_pollset();
}

Handle ZmqLoop::add_timeout(milliseconds timeout, TimerCallback cb, bool is_recurrent) {
    auto h = timer_manager_.add(std::make_unique<Timer>(timeout, std::move(cb), is_recurrent));
    timer_handles_.push_back(h);
    return h;
}

void ZmqLoop::remove_timeout(const Handle& to_remove) {
    handles_to_remove_.push_back(to_remove);
}

void ZmqLoop::build_pollset() {
    poll_items_.clear();
    callbacks_.clear();

    for (auto& r : registered_sockets_) {
        //
        poll_items_.push_back(zmq::pollitem_t{r.first, 0, ZMQ_POLLIN, 0});
        callbacks_.push_back(r.second);
    }
}

milliseconds ZmqLoop::next_timeout() const {
    if (timer_handles_.empty()) {
        return -1ms;
    }

    std::vector<milliseconds> remaining_times;
    std::for_each(timer_handles_.begin(), timer_handles_.end(), [&remaining_times, this] (const auto& h) {

        auto* t = timer_manager_.get(h);
        if (t != nullptr) {
            remaining_times.push_back(t->remaining());
        }

    });

    return *std::min_element(remaining_times.begin(), remaining_times.end());
}

void ZmqLoop::run() {

    while (should_run) {

        try {
            int count = zmq::poll(&poll_items_[0], poll_items_.size(), next_timeout());

            // now see what item has ZMQ_POLLIN
            for (int i = 0; i < poll_items_.size(); i++) {
                auto item = poll_items_[i];
                if (item.revents & ZMQ_POLLIN) {
                    auto cb = callbacks_[i];
                    (*cb)(); // weeeird
                }
            }

            // see if need to process any timer
            std::for_each(timer_handles_.begin(), timer_handles_.end(), [this] (const auto& h) {

                auto* t = timer_manager_.get(h);
                if (t != nullptr) {

                    if (t->is_elapsed()) {
                        t->execute();

                        if (t->is_recurrent()) {
                            t->reset();
                        } else {
                            handles_to_remove_.push_back(h);
                        }
                    }

                }
            });

            // TODO how efficient is that???
            auto it = timer_handles_.begin();
            while (it != timer_handles_.end()) {
                if (std::find(handles_to_remove_.begin(), handles_to_remove_.end(), *it) != handles_to_remove_.end()) {
                    timer_manager_.remove(*it);
                    timer_handles_.erase(it);
                } else {
                    it++;
                }
            }
            handles_to_remove_.clear();

        } catch (const zmq::error_t& error) {
            std::cerr << error.what() << std::endl;
        }

    }
}
}
