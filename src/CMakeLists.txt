add_library(snoozraft string_utils.cc zmq_helpers.cc)
target_link_libraries(snoozraft zmq pthread)

add_executable(main main.cpp)
target_link_libraries(main snoozraft)