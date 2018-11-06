//
// Created by benoit on 18/10/17.
//

#include "config.h"
#include "os.h"
#include "string_utils.h"
#include "error.h"

namespace snooz {
JsonConfigImpl::JsonConfigImpl(nlohmann::json json): json_(std::move(json)) {}

JsonConfigImpl* JsonConfigImpl::load_from_file(const std::string& filename) {
    std::ifstream fs{filename};
    std::stringstream ss;
    ss << fs.rdbuf();

    auto json = nlohmann::json::parse(ss.str());
    return new JsonConfigImpl(json);
}

void JsonConfigImpl::validate() {}

///
/// \return vector of boostrap nodes for this network
std::vector<std::string> JsonConfigImpl::bootstrap_nodes() const {

    std::vector<std::string> nodes;

    for (auto& bt: json_["membership"]["bootstrap_nodes"]) {
        nodes.push_back(bt);
    }

    return nodes;
}

///
/// \return true if the node for this configuration is a boostrap node.
bool JsonConfigImpl::is_bootstrap() const {
    return json_["membership"]["is_bootstrap"] == "Y";
}

///
/// \return port where this node is listening
std::string JsonConfigImpl::port() const {
    return json_["node"]["port"];
}

///
/// \return host where this node is listening
std::string JsonConfigImpl::host() const {
    return json_["node"]["host"];
}

std::string JsonConfigImpl::client_port() const {
    return json_["client"]["port"];
}

std::string JsonConfigImpl::store_filepath() const {
    return json_["node"]["store_path"];
}

// ====================================================
// Configuration from environment variables. Easier to
// use with Docker
// ====================================================
constexpr char EnvConfigImpl::HOST_ENV[];
constexpr char EnvConfigImpl::PORT_ENV[];
constexpr char EnvConfigImpl::CLIENT_PORT_ENV[];
constexpr char EnvConfigImpl::IS_BOOTSTRAP_ENV[];
constexpr char EnvConfigImpl::BOOTSTRAP_NODES_ENV[];
constexpr char EnvConfigImpl::STORE_PATH_ENV[];

EnvConfigImpl::EnvConfigImpl():
    port_(snooz::getenv(PORT_ENV)),
    host_(snooz::getenv(HOST_ENV)),
    client_port_(snooz::getenv(CLIENT_PORT_ENV)),
    store_path_(snooz::getenv(STORE_PATH_ENV)) {

    auto is_bootstrap_str = snooz::getenv(std::string(IS_BOOTSTRAP_ENV));
    is_bootstrap_ = is_bootstrap_str == "Y" || is_bootstrap_str == "True";
    std::cout << is_bootstrap_ << " from " << is_bootstrap_str << " - when lower " << lower(is_bootstrap_str) << std::endl;
    bootstrap_nodes_ = split(snooz::getenv(BOOTSTRAP_NODES_ENV), ",");
}

// need port, host and at least a bootstrap node.
void EnvConfigImpl::validate() {
    if (port_.empty()) {
        throw snooz::ConfigException("port is not specified. Please set " + std::string(PORT_ENV) +  " environment variable.");
    }

    if (host_.empty()) {
        throw snooz::ConfigException("host is not specified. Please set " + std::string(HOST_ENV) +" environment variable.");
    }

    if (bootstrap_nodes_.empty()) {
        throw snooz::ConfigException("There are no bootstrap nodes. Please add at least one node with " + std::string(BOOTSTRAP_NODES_ENV));
    }

    if (client_port_.empty()) {
        throw new snooz::ConfigException("Client port is not specified. Please set " + std::string(CLIENT_PORT_ENV));
    }

    if (store_path_.empty()) {
        throw snooz::ConfigException("No store path set. Please set the path with " + std::string(STORE_PATH_ENV));
    }
}

std::vector<std::string> EnvConfigImpl::bootstrap_nodes() const {
    return bootstrap_nodes_;
}

bool EnvConfigImpl::is_bootstrap() const {
    return is_bootstrap_;
}

std::string EnvConfigImpl::port() const {
    return port_;
}

std::string EnvConfigImpl::host() const {
    return host_;
}

std::string EnvConfigImpl::client_port() const {
    return client_port_;
}

std::string EnvConfigImpl::store_filepath() const {
    return store_path_;
}

}

