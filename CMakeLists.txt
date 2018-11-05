cmake_minimum_required(VERSION 3.7)
project(snoozraft2)

set(CMAKE_CXX_STANDARD 14)
#set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D_GLIBCXX_DEBUG")

# for YouCompleteMe
SET( CMAKE_EXPORT_COMPILE_COMMANDS ON )
IF( EXISTS "${CMAKE_CURRENT_BINARY_DIR}/compile_commands.json" )
          EXECUTE_PROCESS( COMMAND ${CMAKE_COMMAND} -E copy_if_different
                      ${CMAKE_CURRENT_BINARY_DIR}/compile_commands.json
                      ${CMAKE_CURRENT_SOURCE_DIR}/compile_commands.json
         )
ENDIF()



# FOR TESTING
add_subdirectory (googletest-release-1.7.0)
enable_testing()
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

find_package( Boost REQUIRED COMPONENTS program_options system log log_setup)
ADD_DEFINITIONS(-DBOOST_LOG_DYN_LINK)
include_directories( ${Boost_INCLUDE_DIRS} )


include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

include_directories(src)
add_subdirectory(src)
add_subdirectory(tests)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/resources/
        DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
