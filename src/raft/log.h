//
// Created by benoit on 18/10/28.
//

#pragma once

#include <string>
namespace snooz {

class LogEntry {
public:
    LogEntry(int term, std::string content): term_{term}, content_{std::move(content)} {}

    /// Representation in the storage
    ///
    /// \return a string that represent the log entry
    std::string to_string() const;

    static LogEntry from_string(const std::string& repr);

    int term() const;
    std::string content() const;
private:
    int term_{0};
    std::string content_{};
};
}