//
// Created by benoit on 18/10/28.
//

#include "persistent_state.h"
#include "error.h"

namespace snooz {
    
PersistentState::PersistentState(std::string path):
    file_name_(path),
    input_(path) {
    if (!input_.is_open()) {
        throw ConfigException("Error opening persistent state file: " + file_name_);
    }
}

// Will persist to file and then add to log entries
void PersistentState::append_log_entry(LogEntry entry) {
    // What happened if failed to write to file?
    write_to_file("LOG:" + entry.to_string());
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
    write_to_file("TERM:" + std::to_string(term));
    term_ = term;
}
    
int PersistentState::get_term() const {
    return term_;
}

void PersistentState::set_voted_for(std::string candidate) {
    write_to_file("VOTE:" + candidate + " for term " + std::to_string(term_));
    voted_for_ = candidate;
}

const std::string& PersistentState::get_voted_for() const {
    return voted_for_;
}


void PersistentState::write_to_file(const std::string& repr) {
    input_ << repr << '\n';
}

}
