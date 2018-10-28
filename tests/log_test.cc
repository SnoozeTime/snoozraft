//
// Created by benoit on 18/10/28.
//
#include <raft/log.h>
#include <gtest/gtest.h>

using namespace snooz;

TEST(log_to_string, normal_case) {
    LogEntry entry(0, "hello");
    ASSERT_STREQ("term: 0, content: hello", entry.to_string().c_str());
}

TEST(log_from_string, normal_case) {
    auto entry = LogEntry::from_string("term: 0, content: hello");
    ASSERT_EQ(0, entry.term());
    ASSERT_STREQ("hello", entry.content().c_str());
}
