//
// Created by benoit on 18/10/17.
//

#include <gtest/gtest.h>
#include "config.h"

std::string get_path(std::string rel) {
    return "../../resources/" + rel;
}

TEST(json_config, non_bootstrap) {
    auto conf = snooz::Config(snooz::JsonConfigImpl::load_from_file(get_path("conf.json")));
    auto nodes = conf.bootstrap_nodes();

    ASSERT_EQ(2, nodes.size());
    ASSERT_STREQ("tcp://localhost:5555", nodes[0].c_str());
    ASSERT_STREQ("tcp://localhost:5556", nodes[1].c_str());
    ASSERT_FALSE(conf.is_bootstrap());
    ASSERT_STREQ("5557", conf.port().c_str());
}


TEST(json_config, bootstrap) {
    auto conf = snooz::Config(snooz::JsonConfigImpl::load_from_file(get_path("conf_bootstrap.json")));
    auto nodes = conf.bootstrap_nodes();

    ASSERT_EQ(2, nodes.size());

    ASSERT_STREQ("tcp://localhost:5555", nodes[0].c_str());
    ASSERT_STREQ("tcp://localhost:5556", nodes[1].c_str());
    ASSERT_TRUE(conf.is_bootstrap());
    ASSERT_STREQ("5555", conf.port().c_str());

}

TEST(json_config, non_bootstrap_error) {

}

TEST(json_config, bootstrap_error) {

}