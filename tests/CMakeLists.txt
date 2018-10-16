include_directories(${PROJECT_SOURCE_DIR}/src)

function (add_test test_class)
    add_executable(${test_class} ${test_class}.cc)
    target_link_libraries(${test_class} snoozraft gtest gtest_main)
endfunction()

add_test(string_utils_test)
add_test(peer_test)