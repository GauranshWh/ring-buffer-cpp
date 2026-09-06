cmake_minimum_required(VERSION 3.10)
project(RingBuffer CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(ring_buffer_test tests/main.cpp)

add_executable(race_demo src/race_demo.cpp)
target_link_libraries(race_demo pthread)

add_executable(spsc_concurrent_test tests/spsc_concurrent_test.cpp)
target_link_libraries(spsc_concurrent_test pthread)

enable_testing()
add_test(NAME ring_buffer_tests COMMAND ring_buffer_test)