#include <iostream>
#include "zmq_helpers.h"
#include "string_utils.h"
#include <thread>
#include <vector>
#include <chrono>

using namespace std::literals;

void client_thread(int i) {
    zmq::context_t context{1};

    zmq::socket_t dealer{context, ZMQ_DEALER};
    dealer.connect("tcp://localhost:5555");
    snooz::s_sendmore(dealer, "HI");
    snooz::s_send(dealer, std::to_string(i));

    auto addrs = snooz::s_recv(dealer);
    std::cout << "Received " << addrs << std::endl;
    std::this_thread::sleep_for(5s);
}


int main() {

    zmq::context_t context{1};
    zmq::socket_t router{context, ZMQ_ROUTER};
    router.bind("tcp://*:5555");

    std::vector<std::thread> client_threads;
    client_threads.reserve(10);
    for (int i = 0; i < 10; i++) {
        client_threads.emplace_back(client_thread, i);
    }

    std::vector<std::string> client_ids;

    while (true) {
        // router always append the address of the client in front of the actual message.
        auto address = snooz::s_recv(router);
        auto content = snooz::s_recv(router);
        auto id = snooz::s_recv(router);

        client_ids.push_back(id);
        std::cout << "Received " << content << " from " << address << std::endl;

        snooz::s_sendmore(router, address);
        snooz::s_send(router, snooz::join(client_ids, ","));

        if (client_ids.size() == 10) {
            break;
        }
    }

    for (auto& t : client_threads) {
        t.join();
    }

    return 0;
}