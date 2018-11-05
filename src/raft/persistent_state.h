//
// Created by benoit on 18/10/28.
//

#pragma once

#include <string>
#include <fstream>
#include "raft/log.h"

namespace snooz {

/// Persistent state is persisted to disk before answering to RPC. If a peer crash,
/// it should be able to get back this state at startup.
///
/// For example, before telling a peer we have voted for him in an election,
/// we write its identity to disk.

class PersistentState {
public:

    explicit PersistentState(std::string path);

    // Will persist to file and then add to log entries
    void append_log_entry(LogEntry entry);
    const Log& get_log() const;

    // Will add special entry to log.
    void set_term(int term);
    int get_term() const;

    void set_voted_for(std::string candidate);
    const std::string& get_voted_for() const;
private:

    void write_to_file(const std::string& repr);

    /// Where the data is persisted.
    std::string file_name_;
    std::ofstream input_;

    Log log_;
    int term_{0};
    std::string voted_for_;
};
}



