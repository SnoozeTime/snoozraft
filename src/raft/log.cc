//
// Created by benoit on 18/10/28.
//

#include "log.h"
#include <sstream>
#include <tuple>
#include <string_utils.h>
#include <cassert>

namespace snooz {
std::string LogEntry::to_string() const {
    std::stringstream ss;
    ss << "term: " << term_ << ", content: " << content_;
    return ss.str();
}

LogEntry LogEntry::from_string(const std::string &repr) {
    auto fields = snooz::split(repr, ",");
    assert(fields.size() == 2);

    auto term_tokens = split(fields[0], ":");
    assert(term_tokens.size() == 2);
    std::string term_str = snooz::trim(term_tokens[1]);

    auto content_tokens =split(fields[1], ":");
    assert(content_tokens.size() == 2);
    std::string content = snooz::trim(content_tokens[1]);

    return LogEntry(std::stoi(term_str), content);
}

int LogEntry::term() const {
    return term_;
}

std::string LogEntry::content() const {
    return content_;
}


const LogEntry &Log::operator[](size_t index) const {
    assert(index > 0); // from 1.
    return entries_[index-1]; // boooom
}

void Log::overwrite(size_t from, const std::vector<std::tuple<int, std::string>>& entries) {
    // what happen if from is > size?
    assert(from-1 <= entries_.size());
    assert(from >= 1);

    // From = 1 means we need to overwrite from index 0 in the vector.

    // First remove all the entries from index from, then add the new ones
    entries_.erase(entries_.begin()+from-1,
                   entries_.end());

    for (const auto& entry: entries) {
        push(std::get<1>(entry), std::get<0>(entry));
    }
}

void Log::push(LogEntry entry) {
    entries_.push_back(std::move(entry));
}

void Log::push(std::string content, int term) {
    entries_.emplace_back(term, std::move(content));
}

size_t Log::size() const {
    return entries_.size();
}



}
