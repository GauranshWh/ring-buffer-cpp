#include <iostream>
#include <thread>
#include <chrono>
#include <optional>

// Cleanly isolate the class definitions using preprocessor renaming
#define RingBuffer MutexRingBuffer
#include "../src/spsc_mutex_queue.hpp"
#undef RingBuffer

#define RingBuffer LockFreeRingBuffer
#include "../src/spsc_lockfree_queue.hpp"
#undef RingBuffer

const int NUM_ITEMS = 100000; 
const size_t CAPACITY = 131072;  // was 1024

void print_results(const std::string& label, std::chrono::nanoseconds duration, int items) {
    double seconds = duration.count() / 1'000'000'000.0;
    double ops_per_sec = items / seconds;
    double ns_per_op = static_cast<double>(duration.count()) / items;

    std::cout << "=== " << label << " ===\n"
              << "Duration:      " << seconds << " seconds\n"
              << "Throughput:    " << ops_per_sec / 1'000'000.0 << " million ops/sec\n"
              << "Latency/op:    " << ns_per_op << " ns/op\n\n";
}

int main() {
    std::cout << "Starting SPSC Queue Benchmark (" << NUM_ITEMS << " items)...\n\n";

    // =========================================================================
    // 1. Benchmark Mutex-Based Queue
    // =========================================================================
    {
        MutexRingBuffer<int, CAPACITY> queue;

        auto start = std::chrono::high_resolution_clock::now();

        std::thread producer([&]() {
            for (int i = 0; i < NUM_ITEMS; ++i) {
                while (!queue.push(i)) {
                    std::this_thread::yield(); 
                }
                // if (i % 10000 == 0) std::cout << "  producer at " << i << "\n";
            }
        });

        std::thread consumer([&]() {
            for (int i = 0; i < NUM_ITEMS; ++i) {
                std::optional<int> val;
                while (!(val = queue.pop())) {
                    std::this_thread::yield(); 
                }
                // if (i % 10000 == 0) std::cout << "  consumer at " << i << "\n";
            }
        });

        producer.join();
        consumer.join();

        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
        print_results("Mutex-Based SPSC Queue", duration, NUM_ITEMS);
    }

    // =========================================================================
    // 2. Benchmark Lock-Free Queue
    // =========================================================================
    {
        LockFreeRingBuffer<int, CAPACITY> queue;

        auto start = std::chrono::high_resolution_clock::now();

        std::thread producer([&]() {
            for (int i = 0; i < NUM_ITEMS; ++i) {
                while (!queue.push(i)) {
                    std::this_thread::yield(); 
                }
                // if (i % 10000 == 0) std::cout << "  producer at " << i << "\n";
            }
        });

        std::thread consumer([&]() {
            for (int i = 0; i < NUM_ITEMS; ++i) {
                std::optional<int> val;
                while (!(val = queue.pop())) {
                    std::this_thread::yield(); 
                }
                // if (i % 10000 == 0) std::cout << "  consumer at " << i << "\n";
            }
        });

        producer.join();
        consumer.join();

        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
        print_results("Lock-Free SPSC Queue", duration, NUM_ITEMS);
    }

    return 0;
}
