//
// Created by benoit on 18/10/28.
//

#pragma once

#include <string>
#include <vector>

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

///
/// Persisted log on each server.
/// Index begins at 1!
class Log {

public:

    void push(LogEntry entry);
    void push(std::string content, int term);

    // Overwrite everything from index
    void overwrite(size_t from, std::vector<std::tuple<int, std::string>> entries);

    // Index begins at 1!
    const LogEntry& operator[](size_t index) const; //optional

    size_t size() const;
private:

    std::vector<LogEntry> entries_;
};

}
