//
// Created by benoit on 18/10/23.
//
#include <gtest/gtest.h>
#include "protocol/message.h"
#include "protocol/messages/PeerListMessage.h"
#include "protocol/messages/JoinMessage.h"
#include "msg_handler.h"

using namespace snooz;

class TestHandler: public MessageHandler {
public:

    void on_message(const PeerListMessage& peer_list) override {
        ok = true;
    }

    void on_message(const MessageData &msg) override {
        std::cout << "not handled\n";
    }

    bool ok{false};
};

TEST(protocol_pack, peer_list) {
    std::vector<std::string> peers = {"peer1", "peer2", "peer3"};
    Message msg{std::make_unique<PeerListMessage>(peers)};

    auto zmq_msg = msg.pack("address");
    auto frames = zmq_msg.frames();
    ASSERT_EQ(3, frames.size());
    // Peer list is position 0 in the enum
    ASSERT_STREQ("address", frames[0].c_str());
    ASSERT_STREQ("0", frames[1].c_str());

    // Then unpack.
    Message unpacked;
    unpacked.unpack(zmq_msg);
    ASSERT_EQ(MessageType::PEER_LIST, unpacked.message_type());
    const auto peer_list = dynamic_cast<const PeerListMessage*>(unpacked.data());
    ASSERT_TRUE(peer_list != nullptr);
    auto unpacked_peers = peer_list->peers();
    ASSERT_EQ(3, unpacked_peers.size());
    ASSERT_STREQ("peer1", unpacked_peers[0].c_str());
    ASSERT_STREQ("peer2", unpacked_peers[1].c_str());
    ASSERT_STREQ("peer3", unpacked_peers[2].c_str());
}


TEST(double_dispatch, msg_handled) {
    std::vector<std::string> peers = {"peer1", "peer2", "peer3"};
    Message msg{std::make_unique<PeerListMessage>(peers)};

    TestHandler handler;
    handler.dispatch(msg);
    ASSERT_TRUE(handler.ok);
}

TEST(double_dispatch, msg_not_handled) {
    Message msg{std::make_unique<JoinMessage>("hi")};

    TestHandler handler;
    handler.dispatch(msg);
    ASSERT_FALSE(handler.ok);
}