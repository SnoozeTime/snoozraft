//
// Created by benoit on 18/10/17.
//

#include <gtest/gtest.h>
#include "config.h"

std::string get_path(std::string rel) {
    return "../../resources/" + rel;
}

TEST(json_config, non_bootstrap) {
    auto conf = snooz::JsonConfig::load(get_path("conf.json"));
    auto nodes = conf.bootstrap_nodes();

    ASSERT_EQ(2, nodes.size());

    ASSERT_STREQ("BOOTSTRAP1", nodes[0].name.c_str());
    ASSERT_STREQ("tcp://localhost:5555", nodes[0].address.c_str());
    ASSERT_STREQ("BOOTSTRAP2", nodes[1].name.c_str());
    ASSERT_STREQ("tcp://localhost:5556", nodes[1].address.c_str());

    ASSERT_FALSE(conf.is_bootstrap());

    if (conf.bootstrap_name()) {
        FAIL();
    }

    ASSERT_STREQ("5557", conf.port().c_str());
}


TEST(json_config, bootstrap) {
    auto conf = snooz::JsonConfig::load(get_path("conf_bootstrap.json"));
    auto nodes = conf.bootstrap_nodes();

    ASSERT_EQ(2, nodes.size());

    ASSERT_STREQ("BOOTSTRAP1", nodes[0].name.c_str());
    ASSERT_STREQ("tcp://localhost:5555", nodes[0].address.c_str());
    ASSERT_STREQ("BOOTSTRAP2", nodes[1].name.c_str());
    ASSERT_STREQ("tcp://localhost:5556", nodes[1].address.c_str());

    ASSERT_TRUE(conf.is_bootstrap());

    if (conf.bootstrap_name()) {
        auto name = conf.bootstrap_name().value();
        ASSERT_STREQ("BOOTSTRAP1", name.c_str());
    } else {
        FAIL();
    }

    ASSERT_STREQ("5555", conf.port().c_str());

}

TEST(json_config, non_bootstrap_error) {

}

TEST(json_config, bootstrap_error) {

}