//
// Created by benoit on 18/10/21.
//

#include <gtest/gtest.h>
#include "handle.h"
#include <string>

struct Obj {
    Obj(int i, std::string j): value_1{i}, value_str{std:: move(j)} {};
    int value_1;
    std::string value_str;
};

using ObjManager = snooz::HandleManager<Obj>;

TEST(handle, basic_test) {
    ObjManager m;

    auto h = m.add(std::make_unique<Obj>(2, "hi"));

    ASSERT_EQ(1, h.version);

    auto* obj_ptr = m.get(h);
    ASSERT_TRUE(obj_ptr != nullptr);
    ASSERT_EQ(2, obj_ptr->value_1);
    ASSERT_STREQ("hi", obj_ptr->value_str.c_str());

    // Now, add a second object.
    auto h2 = m.add(std::make_unique<Obj>(23, "another"));
    ASSERT_EQ(1, h2.version);
    auto* obj2_ptr = m.get(h2);
    ASSERT_TRUE(obj2_ptr != nullptr);
    ASSERT_EQ(23, obj2_ptr->value_1);
    ASSERT_STREQ("another", obj2_ptr->value_str.c_str());

    // Remove the first. Handle should not be valid anymore.
    ASSERT_TRUE(m.remove(h));
    ASSERT_TRUE(m.get(h) == nullptr);

    auto h3 = m.add(std::make_unique<Obj>(3, "hello"));
    ASSERT_EQ(2, h3.version);
    ASSERT_EQ(0, h3.idx);
    auto *obj3_ptr = m.get(h3);
    ASSERT_TRUE(obj3_ptr != nullptr);
    ASSERT_EQ(3, obj3_ptr->value_1);

    // Cannot get with old handle
    ASSERT_TRUE(m.get(h) == nullptr);

}

TEST(handle, cannot_remove_with_old_handle) {
    ObjManager m;

    auto h = m.add(std::make_unique<Obj>(2, "hi"));
    ASSERT_EQ(1, h.version);
    ASSERT_TRUE(m.remove(h));

    auto h2 = m.add(std::make_unique<Obj>(3, "hello"));
    ASSERT_FALSE(m.remove(h));
    ASSERT_TRUE(m.get(h) == nullptr);

}