//
// Created by benoit on 18/10/23.
//
#include <gtest/gtest.h>
#include "protocol/message.h"
#include "protocol/messages/PeerListMessage.h"

using namespace snooz;

TEST(protocol_pack, peer_list) {
    std::vector<std::string> peers = {"peer1", "peer2", "peer3"};
    Message msg{std::make_unique<PeerListMessage>(peers)};

    auto zmq_msg = msg.pack("address");
    auto frames = zmq_msg.frames();
    ASSERT_EQ(3, frames.size());
    // Peer list is position 0 in the enum
    ASSERT_STREQ("address", frames[0].c_str());
    ASSERT_STREQ("0", frames[1].c_str());
}

