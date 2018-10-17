cmake_minimum_required(VERSION 3.10)
project(snoozraft2)

set(CMAKE_CXX_STANDARD 17)

# FOR TESTING
add_subdirectory (googletest-release-1.7.0)
enable_testing()
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

set(CMAKE_CXX_STANDARD 14)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

add_subdirectory(src)
add_subdirectory(tests)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/resources/
        DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
