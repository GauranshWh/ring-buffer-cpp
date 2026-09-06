#include <iostream>
#include <thread>
#include <mutex>

int counter = 0;
std::mutex counter_mutex;

void increment_many() {
    for (int i = 0; i < 100000; i++) {
        std::lock_guard<std::mutex> lock(counter_mutex);
        counter++;
    }
}

int main() {
    std::thread t1(increment_many);
    std::thread t2(increment_many);
    t1.join();
    t2.join();
    std::cout << "Final counter: " << counter << std::endl;
    return 0;
}