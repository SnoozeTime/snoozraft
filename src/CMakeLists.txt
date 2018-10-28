
set(CMAKE_CXX_FLAGS "-Wall -Werror")
add_library(snoozraft string_utils.cc
        zmq/zmq_helpers.cc
        node.cc
        peer.cc
        config.cc
        zmq/zmq_loop.cc
        zmq/message.cc
        os.cc
        protocol/message.cc
        raft/raft_fsm.cc
        raft/log.cc
        frontend/frontend.cc)
target_link_libraries(snoozraft zmq pthread  ${CONAN_LIBS} ${Boost_LIBRARIES})

add_executable(main main.cpp)
target_link_libraries(main snoozraft)
