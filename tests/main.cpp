#include "../src/ring_buffer.hpp"
#include <iostream>
#include<string>

int main(){

    bool all_passed = true;

    RingBuffer<std::string, 4> ri;
    ri.push("hello");
    ri.push("world");
    ri.push(std::string("hello"));

    auto pop_result = ri.pop();
    std::string a = pop_result.value_or(""); 
    std::cout << a << "\n";

    pop_result = ri.pop();
    a = pop_result.value_or(""); 
    std::cout << a << "\n";

    pop_result = ri.pop();
    a = pop_result.value_or(""); 
    std::cout << a << "\n";
    RingBuffer<int,4> r;

    // Test 1: push 4, pop 4, check order and values
    r.push(1);
    r.push(2);
    r.push(3);
    r.push(4);

    for (int expected = 1; expected <= 4; expected++) {
        auto result = r.pop();
        if (!result.has_value()) {
            all_passed = false;
            std::cout << "FAIL: expected " << expected << " but got nullopt" << std::endl;
        } else if (*result != expected) {
            all_passed = false;
            std::cout << "FAIL: expected " << expected << " but got " << *result << std::endl;
        } else {
            std::cout << "PASS: got " << *result << std::endl;
        }
    }

    // Test 2: pop on empty buffer should give nullopt
    auto empty_result = r.pop();
    if (empty_result.has_value()) {
            all_passed = false;
        std::cout << "FAIL: expected nullopt on empty pop, got " << *empty_result << std::endl;
    } else {
        std::cout << "PASS: empty pop correctly returned nullopt" << std::endl;
    }

    // Test 3: push 5 into capacity-4 buffer, oldest (1) should be overwritten
    r.push(1);
    r.push(2);
    r.push(3);
    r.push(4);
    r.push(5); // this should overwrite 1

    // your job: pop everything here and check you get 2, 3, 4, 5 — NOT 1, 2, 3, 4
    // write this loop yourself, same pattern as Test 1

    for(int expected = 2; expected <= 5 ; expected++){
        auto result = r.pop();
        if (!result.has_value()) {
            std::cout << "FAIL: expected " << expected << " but got nullopt" << std::endl;
            all_passed = false;
        } else if (*result != expected) {
            all_passed = false;
            std::cout << "FAIL: expected " << expected << " but got " << *result << std::endl;
        } else {
            std::cout << "PASS: got " << *result << std::endl;
        }
    }

    return all_passed ? 0 : 1;
}