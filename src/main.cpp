#include <iostream>
#include "node.h"


int main(int argc, char** argv) {

    // TODO Add boost program options to choose what kind of config (from json/yaml file or from environment)
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <CONFIG FILE>\n";
        return 1;
    }

    snooz::JsonConfig conf = snooz::JsonConfig::load_from_file(argv[1]);
    snooz::Node node{conf};node.start();

    return 0;
}