#include "../src/spsc_lockfree_queue.hpp"
#include <iostream>
#include <thread>
#include <vector>

int main() {
    RingBuffer<int, 131072> q;
    const int NUM_ITEMS = 100000;

    std::thread producer([&]() {
        for (int i = 0; i < NUM_ITEMS; i++) {
            while (!q.push(i)) {
                // buffer full, retry (busy-wait)
            }
        }
    });

    std::vector<int> received;
    std::thread consumer([&]() {
        int popped_count = 0;
        while (popped_count < NUM_ITEMS) {
            auto result = q.pop();
            if (result.has_value()) {
                received.push_back(*result);
                popped_count++;
            }
        }
    });

    producer.join();
    consumer.join();

    bool all_pass = true;

    for (int i = 0; i < NUM_ITEMS; i++) { 
        if (received[i] != i) {           
            all_pass = false;
            std::cout << "FAIL at index " << i << ": expected " << i << ", got " << received[i] << "\n";
            break;             
        }
    }

    if (all_pass) {
        std::cout << "PASS\n";           
    }
    return 0;
}