#include "peer.h"
#include <gtest/gtest.h>
#include <chrono>
#include <thread>

using namespace std::literals;
using namespace snooz;

TEST(peer_timeout, timeout) {
    zmq::context_t context{1};
    snooz::Peer peer{context, "hi", "ici"};
    peer.reset_deadline(2);
    std::this_thread::sleep_for(1s);
    ASSERT_TRUE(peer.is_alive());
    std::this_thread::sleep_for(1s);
    ASSERT_FALSE(peer.is_alive());
}