//
// Created by benoit on 18/10/28.
//

#pragma once

#include <string>

namespace snooz {

/// Persistent state is persisted to disk before answering to RPC. If a peer crash,
/// it should be able to get back this state at startup.
///
/// For example, before telling a peer we have voted for him in an election,
/// we write its identity to disk.

class PersistentState {
public:


private:

    /// Where the data is persisted.
    std::string file_name_;
};
}



