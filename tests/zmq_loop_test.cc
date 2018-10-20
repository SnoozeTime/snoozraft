//
// Created by benoit on 18/10/20.
//

#include <zmq/zmq_loop.h>
#include <gtest/gtest.h>
#include <thread>

TEST(zmq_loop, basic_usage) {

    zmq::context_t context{1};
    snooz::ZmqLoop loop{&context};

    std::thread t{[&loop] {
        loop.run();
    }};
    loop.shutdown();
    t.join();
}