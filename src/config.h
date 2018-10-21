//
// Created by benoit on 18/10/17.
//

#ifndef SNOOZRAFT2_CONFIG_H
#define SNOOZRAFT2_CONFIG_H

#include <vector>
#include <string>
#include <nlohmann/json.hpp>
#include <boost/filesystem.hpp>
#include <fstream>
#include <sstream>
#include <iostream>

namespace snooz {

template <class ConfigImpl>
class Config {
public:

    explicit Config(ConfigImpl impl): impl_(std::move(impl)) {
        validate();
    }

    ///
    /// \return vector of boostrap nodes for this network
    std::vector<std::string> bootstrap_nodes() const {
        return impl_.bootstrap_nodes();
    }

    ///
    /// \return true if the node for this configuration is a boostrap node.
    bool is_bootstrap() const {
        return impl_.is_bootstrap();
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
    static Config<ConfigImpl> load_from_file(const std::string& filename);

private:


    void validate() {
        impl_.validate();
    }

    ConfigImpl impl_;
};

template<class ConfigImpl>
Config<ConfigImpl> Config<ConfigImpl>::load_from_file(const std::string &filename) {
    return Config{ConfigImpl::load_from_file(filename)};
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
//              "tcp://localhost:5555"
//            ],
//            "is_bootstrap": "N"
//        }
//    }
//
class JsonConfigImpl {


public:
    explicit JsonConfigImpl(nlohmann::json json);

    static JsonConfigImpl load_from_file(const std::string& filename);

    void validate();

    ///
    /// \return vector of boostrap nodes for this network
    std::vector<std::string> bootstrap_nodes() const;

    ///
    /// \return true if the node for this configuration is a boostrap node.
    bool is_bootstrap() const;

    ///
    /// \return port where this node is listening
    std::string port() const;

    ///
    /// \return host where this node is listening
    std::string host() const;


private:
    nlohmann::json json_;
};


/// Read the configuration from the environment
class EnvConfigImpl {
public:

    constexpr static char HOST_ENV[] = "SNOOZ_HOST";
    constexpr static char PORT_ENV[] = "SNOOZ_PORT";
    constexpr static char IS_BOOTSTRAP_ENV[] = "SNOOZ_BOOTSTRAP";
    constexpr static char BOOTSTRAP_NODES_ENV[] = "SNOOZ_BOOTSTRAP_NODES";

    EnvConfigImpl();


    void validate();

    ///
    /// \return vector of boostrap nodes for this network
    std::vector<std::string> bootstrap_nodes() const;

    ///
    /// \return true if the node for this configuration is a boostrap node.
    bool is_bootstrap() const;

    ///
    /// \return port where this node is listening
    std::string port() const;

    ///
    /// \return host where this node is listening
    std::string host() const;

private:
    // Comma separated.
    std::vector<std::string> bootstrap_nodes_;
    bool is_bootstrap_;
    std::string port_;
    std::string host_;
};

using JsonConfig = Config<JsonConfigImpl>;
using EnvConfig = Config<EnvConfigImpl>;

}

#endif //SNOOZRAFT2_CONFIG_H
