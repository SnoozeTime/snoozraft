//
// Created by benoit on 18/10/16.
//

#ifndef SNOOZRAFT2_PEER_H
#define SNOOZRAFT2_PEER_H

#include <chrono>
#include "zmq/zmq_helpers.h"
#include "zmq/message.h"

namespace snooz {

constexpr static int DISCONNECTED_TIMEOUT_SEC{30};

/// Represent other peers of the network.
class Peer {
public:

    ///
    /// \param ctx
    /// \param address to connect to
    /// \param my_address where my node is
    Peer(zmq::context_t& ctx, std::string address, std::string my_address);

    /// Need to be called so that the socket will connect to the external peer
    void connect();

    void send(std::string msg);
    void send(const ZmqMessage& msg);

    /// Will extend the deadline_. Should be called whenever we receive a message from this peer.
    void reset_deadline(int seconds=DISCONNECTED_TIMEOUT_SEC);

    /// Check that deadline is not expired. If it is, the node will remove this peer from the connected
    /// nodes.
    ///
    /// \return true if deadline is not expired
    bool is_alive() const;


    std::string address() const;
private:
    /// When considered dead.
    std::chrono::system_clock::time_point deadline_;


    std::string address_;

    /// To send messages !
    zmq::socket_t dealer_;
};

}


#endif //SNOOZRAFT2_PEER_H
