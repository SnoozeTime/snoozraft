#include <gtest/gtest.h>
#include "string_utils.h"

TEST(string_util, join) {
    std::vector<std::string> vc = {"bonjour", "ca", "va"};
    ASSERT_STREQ("bonjour,ca,va", snooz::join(vc, ",").c_str());
    ASSERT_STREQ("bonjourX caX va", snooz::join(vc, "X ").c_str());

    std::vector<std::string> vc2 = {"hi"};
    ASSERT_STREQ("hi", snooz::join(vc2, "d").c_str());
    ASSERT_STREQ("", snooz::join(std::vector<std::string>(), "d").c_str());
}

TEST(string_util, split) {
    std::string tosplit = "Hello,you";
    auto vec = snooz::split(tosplit, ",");
    ASSERT_EQ(2, vec.size());
    ASSERT_STREQ("Hello", vec[0].c_str());
    ASSERT_STREQ("you", vec[1].c_str());

    auto vec2 = snooz::split(tosplit, "u");
    ASSERT_EQ(1, vec2.size());
    ASSERT_STREQ("Hello,yo", vec2[0].c_str());

    auto vec3 = snooz::split(tosplit, ",y");
    ASSERT_EQ(2, vec3.size());
    ASSERT_STREQ("Hello", vec3[0].c_str());
    ASSERT_STREQ("ou", vec3[1].c_str());

    auto vec4 = snooz::split(tosplit, "!");
    ASSERT_EQ(1, vec4.size());
    ASSERT_STREQ("Hello,you", vec4[0].c_str());

    auto vec5 = snooz::split("", ",");
    ASSERT_EQ(0, vec5.size()); //TODO huuum
}

TEST(string_util, lower) {
    auto res = snooz::lower("BonjOur");
    ASSERT_STREQ("bonjour", res.c_str());

    auto res2 = snooz::lower("");
    ASSERT_STREQ("", res2.c_str());
}