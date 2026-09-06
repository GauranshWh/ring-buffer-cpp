#include "../src/spsc_mutex_queue.hpp"
#include <iostream>
#include <thread>
#include <vector>

int main() {
    RingBuffer<int, 131072> q;  // capacity big enough to avoid overwrite for this test
    const int NUM_ITEMS = 100000;

    // Producer thread: push 0, 1, 2, ..., NUM_ITEMS-1
    std::thread producer([&]() {
        for (int i = 0; i < NUM_ITEMS; i++) {
            q.push(i);
        }
    });

    // Consumer thread: pop NUM_ITEMS times, store what it got
    std::vector<int> received;
    std::thread consumer([&]() {
        int popped_count = 0;
        while (popped_count < NUM_ITEMS) {
            auto result = q.pop();
            if (result.has_value()) {
                received.push_back(*result);
                popped_count++;
            }
            // if empty, just loop and try again (busy-wait — not efficient, but fine for this test)
        }
    });

    producer.join();
    consumer.join();

    bool all_passed = true;

    for(int i = 0 ; i < NUM_ITEMS ; i++){
        if(received[i] != i){
            all_passed = false;
            std::cout<< "FAIL\n";
            return 0;
        }
    }

    if(all_passed){
        std::cout<<"PASS\n";
    }




    // your job: verify `received` actually contains 0, 1, 2, ..., NUM_ITEMS-1 IN ORDER
    // hint: loop through `received`, check received[i] == i, track a bool all_passed

    return 0;
}