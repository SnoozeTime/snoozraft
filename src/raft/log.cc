//
// Created by benoit on 18/10/28.
//

#include "log.h"
#include <sstream>
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


}