#pragma once
#include <array>
#include <optional>
#include <cstddef>
#include <atomic>

template<typename T, std::size_t N>
class RingBuffer {
public:
    bool push(const T& value) {
        std::size_t current_tail = tail_.load(std::memory_order_relaxed);
        std::size_t next_tail = (current_tail + 1) & (N - 1);

        if(next_tail == head_.load(std::memory_order_acquire)){
            return false;
        }

        buffer_[current_tail] = value;
        tail_.store(next_tail , std::memory_order_release); 
        return true;
        
    }

    std::optional<T> pop() {
        
        std::size_t current_head = head_.load(std::memory_order_relaxed);

        if(current_head == tail_.load(std::memory_order_acquire)){
            return std::nullopt;
        }

        T value = std::move(buffer_[current_head]);
        std::size_t next_head = (current_head + 1) & (N-1);

        head_.store(next_head,std::memory_order_release);
        return value;
    }

private:
    std::array<T, N> buffer_{};
    std::atomic<std::size_t> head_{0};
    std::atomic<std::size_t> tail_{0};
};