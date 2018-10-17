add_library(snoozraft string_utils.cc zmq_helpers.cc node.cc peer.cc config.cc)
target_link_libraries(snoozraft zmq pthread  ${CONAN_LIBS})

add_executable(main main.cpp)
target_link_libraries(main snoozraft)