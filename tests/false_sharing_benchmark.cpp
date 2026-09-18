
#include <iostream>
#include <thread>
#include <chrono>
#include <optional>
#include <string>

#define RingBuffer UnpaddedRingBuffer
#include "../src/spsc_lockfree_queue.hpp"
#undef RingBuffer

#define RingBuffer PaddedRingBuffer
#include "../src/spsc_lockfree_padded.hpp"
#undef RingBuffer

const int NUM_ITEMS = 1000000;   // bigger N — false sharing effects can be subtle, need enough iterations
const size_t CAPACITY = 1048576;  // large enough to avoid backpressure/yield overhead, per your earlier fix

void print_results(const std::string& label, std::chrono::nanoseconds duration, int items) {
    double seconds = duration.count() / 1'000'000'000.0;
    double ns_per_op = static_cast<double>(duration.count()) / items;
    std::cout << "=== " << label << " ===\n"
              << "Duration:   " << seconds << " s\n"
              << "Latency/op: " << ns_per_op << " ns/op\n\n";
}

int main() {
    // your job: same producer/consumer pattern as your earlier benchmark,
    // once for UnpaddedRingBuffer<int, CAPACITY>, once for PaddedRingBuffer<int, CAPACITY>
    // print_results for both, so you can directly compare
    {
    UnpaddedRingBuffer<int,CAPACITY> ub;

    auto start = std::chrono::high_resolution_clock::now();

    std::thread producer([&]() {
            for (int i = 0; i < NUM_ITEMS; ++i) {
                while (!ub.push(i)) {
                    std::this_thread::yield(); 
                }
            }
        });

    std::thread consumer([&]() {
        for(int i = 0 ; i < NUM_ITEMS; i++){
            std::optional<int> val;
            while(!(val = ub.pop())){
                std::this_thread::yield();
            }
        }
    });

    producer.join();
    consumer.join();

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end-start);
    print_results("Unpadded SPSC Queue", duration, NUM_ITEMS);
    }

    {
    PaddedRingBuffer<int, CAPACITY> pb;

    auto start = std::chrono::high_resolution_clock::now();

    std::thread producer([&]() {
            for (int i = 0; i < NUM_ITEMS; ++i) {
                while (!pb.push(i)) {
                    std::this_thread::yield(); 
                }
            }
        });

    std::thread consumer([&]() {
        for(int i = 0 ; i < NUM_ITEMS; i++){
            std::optional<int> val;
            while(!(val = pb.pop())){
                std::this_thread::yield();
            }
        }
    });

    producer.join();
    consumer.join();

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end-start);
    print_results("Padded SPSC Queue", duration, NUM_ITEMS);

    }




}