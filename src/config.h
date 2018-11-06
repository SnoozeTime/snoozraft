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
#include <memory>

namespace snooz {


class ConfigImpl {


public:

    virtual ~ConfigImpl() = default;

    virtual void validate() = 0;

    ///
    /// \return vector of boostrap nodes for this network
    virtual std::vector<std::string> bootstrap_nodes() const = 0;

    ///
    /// \return true if the node for this configuration is a boostrap node.
    virtual bool is_bootstrap() const = 0;

    ///
    /// \return port where this node is listening
    virtual std::string port() const = 0;

    ///
    /// \return host where this node is listening
    virtual std::string host() const = 0;

    ///
    /// \return the port of the client frontend
    virtual std::string client_port() const = 0;

    ///
    /// \return the path to the storage file
    virtual std::string store_filepath() const = 0;
};

class Config {
public:

    explicit Config(ConfigImpl *impl): impl_(impl) {
        validate();
    }

    ///
    /// \return vector of boostrap nodes for this network
    std::vector<std::string> bootstrap_nodes() const {
        return impl_->bootstrap_nodes();
    }

    ///
    /// \return true if the node for this configuration is a boostrap node.
    bool is_bootstrap() const {
        return impl_->is_bootstrap();
    }

    ///
    /// \return host where this node is listening
    std::string host() const {
        return impl_->host();
    }

    ///
    /// \return port where this node is listening
    std::string port() const {
        return impl_->port();
    }

    std::string client_port() const {
        return impl_->client_port();
    }


    ///
    /// \return the path to the storage file
    std::string store_filepath() const {
        return impl_->store_filepath();
    }
private:


    void validate() {
        impl_->validate();
    }

    std::unique_ptr<ConfigImpl> impl_;
};


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
class JsonConfigImpl: public ConfigImpl {


public:
    explicit JsonConfigImpl(nlohmann::json json);

    static JsonConfigImpl* load_from_file(const std::string& filename);

    void validate() override;

    ///
    /// \return vector of boostrap nodes for this network
    std::vector<std::string> bootstrap_nodes() const override;

    ///
    /// \return true if the node for this configuration is a boostrap node.
    bool is_bootstrap() const override;

    ///
    /// \return port where this node is listening
    std::string port() const override;

    ///
    /// \return host where this node is listening
    std::string host() const override;

    std::string client_port() const override;

    ///
    /// \return the path to the storage file
    std::string store_filepath() const override;

private:
    nlohmann::json json_;
};


/// Read the configuration from the environment
class EnvConfigImpl: public ConfigImpl {
public:

    constexpr static char HOST_ENV[] = "SNOOZ_HOST";
    constexpr static char PORT_ENV[] = "SNOOZ_PORT";
    constexpr static char CLIENT_PORT_ENV[] = "SNOOZ_CLIENT_PORT";
    constexpr static char IS_BOOTSTRAP_ENV[] = "SNOOZ_BOOTSTRAP";
    constexpr static char BOOTSTRAP_NODES_ENV[] = "SNOOZ_BOOTSTRAP_NODES";
    constexpr static char STORE_PATH_ENV[] = "SNOOZ_STORE_PATH";

    EnvConfigImpl();


    void validate() override;

    ///
    /// \return vector of boostrap nodes for this network
    std::vector<std::string> bootstrap_nodes() const override;

    ///
    /// \return true if the node for this configuration is a boostrap node.
    bool is_bootstrap() const override;

    ///
    /// \return port where this node is listening
    std::string port() const override;

    ///
    /// \return host where this node is listening
    std::string host() const override;

    std::string client_port() const override;

    ///
    /// \return the path to the storage file
    std::string store_filepath() const override;


private:
    // Comma separated.
    std::vector<std::string> bootstrap_nodes_;
    bool is_bootstrap_;
    std::string port_;
    std::string host_;
    std::string client_port_;
    std::string store_path_;
};


}

#endif //SNOOZRAFT2_CONFIG_H
