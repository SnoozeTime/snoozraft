// Will test that the log is persisted correctly..
//
#include <gtest/gtest.h>
#include <raft/persistent_state.h>

TEST(persistent, writing) {
    std::string path = "/tmp/log_raft";

    {
        snooz::PersistentState state(path);
        state.append_log_entry(snooz::LogEntry(0, "hello"));
        state.set_term(45);
        state.set_voted_for("benoit");
    }

    // Now read the file
}
