include_directories(${PROJECT_SOURCE_DIR}/src)

enable_testing()

function (add_snooz_test test_class)
    add_executable(${test_class} ${test_class}.cc)
    target_link_libraries(${test_class} snoozraft gtest gtest_main)
    add_test(${test_class} ${test_class})
endfunction()

add_snooz_test(string_utils_test)
add_snooz_test(peer_test)
add_snooz_test(config_test)
add_snooz_test(zmq_loop_test)
add_snooz_test(handle_test)
add_snooz_test(protocol_test)
add_snooz_test(log_test)
add_snooz_test(persistent_test)
