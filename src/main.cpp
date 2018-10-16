#include <iostream>
#include "zmq_helpers.h"
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

    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " <PORT> <BOOTSTRAP_PORT>\n";
        return 1;
    }

    std::string bootstrap_port{argv[2]};
    std::string port{argv[1]};

    std::vector<std::string> bootstraps;
    if (bootstrap_port != "0") {
        bootstraps.push_back("tcp://localhost:" + bootstrap_port);
    }
    snooz::Node node{bootstraps, port};

    node.start();

    return 0;
}