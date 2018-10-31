#include <iostream>
#include "node.h"
#include <boost/program_options.hpp>
#include <boost/log/trivial.hpp>

#include "log.h"

namespace po = boost::program_options;

int main(int argc, char** argv) {

    init("snoozraft");
    BOOST_LOG(app_logger::get()) << "hi";
    po::options_description desc("Command line options");
    desc.add_options()
            ("help", "print help")
            ("config_type", po::value<std::string>(), "where to load config - Can be ENV or JSON")
            ("config_file", po::value<std::string>(), "File where the configuration is store. Only in case of JSON")
            ("log_name", po::value<std::string>(), "Path of log");

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (vm.count("help")) {
        BOOST_LOG_TRIVIAL(info) << desc << "\n";
        return 1;
    }

    if (vm.count("config_type") == 0) {
        BOOST_LOG_TRIVIAL(error) << "config_type is not set";
        return 1;
    }

    auto config_type = vm["config_type"].as<std::string>();
    if (config_type != "JSON" && config_type != "ENV") {
        BOOST_LOG_TRIVIAL(error) << "config_type should be either JSON or ENV, got " << config_type << " instead.";
        return 1;
    }

    if (config_type == "JSON" && vm.count("config_file") == 0) {
        BOOST_LOG_TRIVIAL(error) << "JSON config should specify configuration file";
        return 1;
    }

    snooz::ConfigImpl* impl;
    if (config_type == "ENV") {
        // New'd but Config object will destruct it!
        impl = new snooz::EnvConfigImpl();
    } else if (config_type == "JSON" ){
        impl = snooz::JsonConfigImpl::load_from_file(vm["config_file"].as<std::string>());
    } else {
        assert(false);
    }

    snooz::Config conf{impl};

    snooz::Node node{std::move(conf)};node.start();

    return 0;
}