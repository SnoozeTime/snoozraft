//
// Created by benoit on 18/10/17.
//

#ifndef SNOOZRAFT2_CONFIG_H
#define SNOOZRAFT2_CONFIG_H

#include <vector>
#include <string>
#include <nlohmann/json.hpp>
#include <experimental/optional>
#include <fstream>
#include <sstream>
#include <iostream>

namespace snooz {

struct BootstrapConfig {
    std::string name;
    std::string address;
};

template <class ConfigImpl>
class Config {
public:

    explicit Config(ConfigImpl impl): impl_(std::move(impl)) {
        validate();
    }

    ///
    /// \return vector of boostrap nodes for this network
    std::vector<BootstrapConfig> bootstrap_nodes() const {
        return impl_.bootstrap_nodes();
    }

    ///
    /// \return true if the node for this configuration is a boostrap node.
    bool is_bootstrap() const {
        return impl_.is_bootstrap();
    }

    ///
    /// \return name of the bootstrap node or nothing if node is not a bootstrap.
    std::experimental::optional<std::string> bootstrap_name() const {
        return impl_.bootstrap_name();
    }

    ///
    /// \return host where this node is listening
    std::string host() const {
        return impl_.host();
    }

    ///
    /// \return port where this node is listening
    std::string port() const {
        return impl_.port();
    }

    /// Load the configuration from a file
    static Config<ConfigImpl> load(const std::string& filename);

private:


    void validate() {
        impl_.validate();
    }

    ConfigImpl impl_;
};

template<class ConfigImpl>
Config<ConfigImpl> Config<ConfigImpl>::load(const std::string &filename) {
    return Config{ConfigImpl::loadFromFile(filename)};
}

//
//    {
//
//        "node": {
//            "host": "localhost
//            "port": "5556"
//        },
//
//        "membership": {
//            "bootstrap_nodes": [
//            {
//                "name": "BOOTSTRAP1",
//                        "address": "tcp://localhost:5555"
//            }],
//            "is_bootstrap": "N"
//        }
//    }
//
class JsonConfigImpl {


public:
    JsonConfigImpl(nlohmann::json json): json_(std::move(json)) {}

    static JsonConfigImpl loadFromFile(const std::string& filename) {
        std::ifstream fs{filename};
        std::stringstream ss;
        ss << fs.rdbuf();

        auto json = nlohmann::json::parse(ss.str());
        return JsonConfigImpl(json);
    }

    void validate() {}

    ///
    /// \return vector of boostrap nodes for this network
    std::vector<BootstrapConfig> bootstrap_nodes() const {

        std::vector<BootstrapConfig> nodes;

        for (auto& bt: json_["membership"]["bootstrap_nodes"]) {
            BootstrapConfig conf = {bt["name"], bt["address"]};
            nodes.push_back(conf);
        }

        return nodes;
    }

    ///
    /// \return true if the node for this configuration is a boostrap node.
    bool is_bootstrap() const {
        return json_["membership"]["is_bootstrap"] == "Y";
    }

    ///
    /// \return name of the bootstrap node or nothing if node is not a bootstrap.
    std::experimental::optional<std::string> bootstrap_name() const {

        auto maybe_name = json_["membership"].find("name");
        if (maybe_name != json_["membership"].end()) {
            return std::experimental::optional<std::string>((*maybe_name).get<std::string>());
        }
        return std::experimental::nullopt;
    }

    ///
    /// \return port where this node is listening
    std::string port() const {
        return json_["node"]["port"];
    }

    ///
    /// \return host where this node is listening
    std::string host() const {
        return json_["node"]["host"];
    }

private:
    nlohmann::json json_;
};

using JsonConfig = Config<JsonConfigImpl>;

}

#endif //SNOOZRAFT2_CONFIG_H
