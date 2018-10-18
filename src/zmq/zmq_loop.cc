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

void ZmqLoop::add_zmq_socket(zmq::socket_t &socket, SocketCallback cb) {
    registered_sockets_.insert(std::make_pair(
            (void*) socket,
            std::make_shared<SocketCallback>(std::move(cb))
    ));

    build_pollset();
}

void ZmqLoop::add_timeout(milliseconds timeout, TimerCallback cb, bool is_recurrent) {
    Timer t{timeout, std::move(cb), is_recurrent};

    if (std::find(timers_.begin(), timers_.end(), t) == timers_.end()) {
        timers_.push_back(std::move(t));
    } else {
        // TODO BOUM
    }
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
    if (timers_.empty()) {
        return -1ms;
    }

    std::vector<milliseconds> remaining_times;
    std::transform(timers_.begin(), timers_.end(), std::back_inserter(remaining_times), [] (const Timer& timer) {
        return timer.remaining();
    });


    return *std::min_element(remaining_times.begin(), remaining_times.end());
}

void ZmqLoop::run() {

    // TODO break this loop with a listening socket.
    while (true) {

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
            auto it = timers_.begin();
            while (it != timers_.end()) {
                if (it->is_elapsed()) {

                    it->execute();

                    if (it->is_recurrent()) {
                        it->reset();
                    } else {
                        it = timers_.erase(it);
                        continue;
                    }

                }

                it++;
            }

        } catch (const zmq::error_t& error) {
            std::cerr << error.what() << std::endl;
        }

    }
}
}
