//
// Created by benoit on 18/10/20.
//

#include <zmq/zmq_loop.h>
#include <zmq/message.h>
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

/// Test that we can add and remove a zmq socket to the loop.
TEST(zmq_loop, add_and_remove_socket) {

    zmq::context_t context{1};
    zmq::socket_t worker{context, ZMQ_PULL};
    worker.bind("tcp://*:44444");

    snooz::ZmqLoop loop{&context};

    int count = 0;


    // Need to process the message otherwise we will receive it forever :D
    loop.add_zmq_socket(worker, [&worker, &count] {
        count++;
        snooz::receive_message(worker);
    });

    loop.add_timeout(1000ms, [&worker, &loop] { loop.remove_zmq_socket(worker);});
    std::thread t{[&loop] { loop.run(); }};

    // Give some work to increase count.
    zmq::socket_t dealer{context, ZMQ_PUSH};
    dealer.connect("tcp://localhost:44444");

    snooz::s_send(dealer, "hi");
    snooz::s_send(dealer, "hi");

    std::this_thread::sleep_for(1.1s);

    // At this point, the socket should be removed from the loop so the handler will not increase count
    // anymore

    snooz::s_send(dealer, "hi");
    snooz::s_send(dealer, "hi");

    std::this_thread::sleep_for(500ms);

    loop.shutdown();
    t.join();

    ASSERT_EQ(2, count);
}