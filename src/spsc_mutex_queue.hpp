#pragma once
#include <array>
#include <optional>
#include <cstddef>
#include <mutex>


template<typename T, std::size_t N>

class RingBuffer{
public:

    bool push(T&& value){
        std::lock_guard<std::mutex> lock(mut_lock);
        if (full()){
            head_ = (head_+1) & (N-1);
        }
        else{
            count_++;
    }

    buffer_[tail_] = std::move(value);
    tail_ = (tail_ + 1) & (N-1);
    return true;
    }

    bool push(const T& value){

        std::lock_guard<std::mutex> lock(mut_lock);

        if (full()){
            head_ = (head_+1) & (N-1);
        }
        else{
            count_++;
        }

        buffer_[tail_] = value;
        tail_ = (tail_ + 1 ) & (N-1); // N but be a power of 2
        return true;
    }

    std::optional<T> pop(){

        std::lock_guard<std::mutex> lock(mut_lock);
        if (empty()){
            return std::nullopt;
        }
        std::optional<T> num = buffer_[head_];
        head_ = (head_ + 1) & (N-1);
        count_--;
        return num;
    }

    bool empty() const{
        if(count_ == 0) return true;
        return false;
    }

    bool full() const{
        if (count_ == N) return true;
        return false;
    }

private:
    std::array<T,N> buffer_{};
    std::size_t head_ = 0;
    std::size_t tail_ = 0;
    std::size_t count_ = 0; ///how many elements are currently stored
    std::mutex mut_lock;

};

