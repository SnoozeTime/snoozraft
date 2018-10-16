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