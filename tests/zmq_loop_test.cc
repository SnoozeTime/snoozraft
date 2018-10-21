//
// Created by benoit on 18/10/20.
//

#include <zmq/zmq_loop.h>
#include <gtest/gtest.h>
#include <thread>
#include <chrono>

using namespace std::literals;

TEST(zmq_loop, basic_usage) {

    zmq::context_t context{1};
    snooz::ZmqLoop loop{&context};

    std::thread t{[&loop] {
        loop.run();
    }};
    loop.shutdown();
    t.join();
}

TEST(zmq_loop, recurrent_timer) {
    zmq::context_t context{1};
    snooz::ZmqLoop loop{&context};

    int i = 0;
    loop.add_timeout(200ms, [&i] { i++; });
    std::thread t{[&loop] { loop.run(); }};

    std::this_thread::sleep_for(1100ms);
    loop.shutdown();

    t.join();
    ASSERT_EQ(5, i);
}

TEST(zmq_loop, remove_timer) {
    zmq::context_t context{1};
    snooz::ZmqLoop loop{&context};

    int i = 0;
    snooz::Handle to_cancel = loop.add_timeout(200ms, [&i] { i++; });

    loop.add_timeout(1100ms, [&to_cancel, &loop] {
        std::cout << to_cancel.idx << " " << to_cancel.version << std::endl;
        loop.remove_timeout(to_cancel);
    }, false);

    std::thread t{[&loop] { loop.run(); }};

    std::this_thread::sleep_for(2s);
    loop.shutdown();

    t.join();
    ASSERT_EQ(5, i);
}