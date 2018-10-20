//
// Created by benoit on 18/10/17.
//

#include "config.h"

namespace snooz {
JsonConfigImpl::JsonConfigImpl(nlohmann::json json): json_(std::move(json)) {}

JsonConfigImpl JsonConfigImpl::loadFromFile(const std::string& filename) {
    std::ifstream fs{filename};
    std::stringstream ss;
    ss << fs.rdbuf();

    auto json = nlohmann::json::parse(ss.str());
    return JsonConfigImpl(json);
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
}