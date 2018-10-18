#include <iostream>
#include "zmq/zmq_helpers.h"
#include "string_utils.h"
#include <thread>
#include <vector>
#include <chrono>

#include "node.h"

#include <iostream>

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


int main(int argc, char** argv) {

    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <CONFIG FILE>\n";
        return 1;
    }

    snooz::JsonConfig conf = snooz::JsonConfig::load(argv[1]);
    snooz::Node node{conf};node.start();

    return 0;
}