//
// Created by benoit on 18/10/28.
//

#include "persistent_state.h"
#include "error.h"
using nlohmann::json;

namespace snooz {

PersistentState::PersistentState(std::string path):
    file_name_(path),
    input_(path) {
    if (!input_.is_open()) {
        throw ConfigException("Error opening persistent state file: " + file_name_);
    }

    data_["term"] = 0;
    data_["voted_for"] = "";
    data_["log"] = json::array();
}

void PersistentState::write() {
    // always use the last version of log.
    data_["log"].clear();
    for (auto& entry : log_.entries()) {
        data_["log"].push_back(entry.to_string());
    }
    std::ofstream ofs(file_name_, std::ios::out | std::ios::trunc); // remove all :D
    ofs << data_.dump();
}

// Will persist to file and then add to log entries
void PersistentState::append_log_entry(LogEntry entry) {
    log_.push(entry);
}

Log& PersistentState:: get_log() {
    return log_;
}

const Log& PersistentState:: get_log() const {
    return log_;
}

// Will add special entry to log.
void PersistentState::set_term(int term) {
    data_["term"] = term;
}

int PersistentState::term() const {
    return data_["term"];
}

void PersistentState::set_voted_for(std::string candidate) {
    data_["voted_for"] = candidate;
}

std::string PersistentState::voted_for() const {
    return data_["voted_for"];
}

}
