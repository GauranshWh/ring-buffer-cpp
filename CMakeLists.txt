cmake_minimum_required(VERSION 3.10)
project(RingBuffer CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(ring_buffer_test tests/main.cpp)

add_executable(race_demo src/race_demo.cpp)
target_link_libraries(race_demo pthread)

add_executable(spsc_concurrent_test tests/spsc_concurrent_test.cpp)
target_link_libraries(spsc_concurrent_test pthread)

add_executable(spsc_lockfree_test tests/spsc_lockfree_test.cpp)
target_link_libraries(spsc_lockfree_test pthread)

add_executable(benchmark tests/benchmark.cpp)
target_link_libraries(benchmark pthread)

add_executable(benchmark_pad tests/false_sharing_benchmark.cpp)
target_link_libraries(benchmark_pad pthread)

add_executable(reciever src/udp_reciever.cpp)
target_link_libraries(reciever pthread)

add_executable(producer src/udp_sender.cpp)
target_link_libraries(producer pthread)

enable_testing()
add_test(NAME ring_buffer_tests COMMAND ring_buffer_test)