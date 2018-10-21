//
// Created by benoit on 18/10/20.
//

#pragma once

#include <stdexcept>

namespace snooz {

/// This is what happens when you don't configure correctly!
class ConfigException : public std::exception {

public:
    ConfigException() = default;
    explicit ConfigException(std::string message): message_(std::move(message)) {};
    const char* what() const noexcept override {
        return message_.c_str();
    }

private:
    std::string message_;
};
}