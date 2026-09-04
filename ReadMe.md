# Ring Buffer (Fixed-Capacity, Overwrite-on-Full)

## Design Decisions
- **Capacity**: Capacity of the ring buffer needs to be enough that it can store upcoming requests while processing of the past once are done, this system does not finalizes or suggest the capacity the caller decides it before starting, array is used of N size (template<typename T, int N>)
- **Full behavior**: the ring buffer is used in places where the consistency and speed is prefered over processing each data so what is does is if it becomes full it overwrites the data from the head which is being coded in push() of the RingBuffer class , so it overwrites the oldest just like FIFO
- **Empty behavior**: when the ring_buffer is empty , we cant pop anything from a empty buffer so the datatype used here is optional<T> which returns nullopt if pop() is applied on empty ringbuffer 
- **Storage**: and array of typename T and int N is begin declared so it can handle any type of datatype and caller can decide the capacity of th ringbuffer 
- **Wraparound**: for the wrap around i had two option to bring the pointer back to the start of ring_buffer using modulo or bitmask , modulo have a calculation over head but can work with any number of N whereas bitmask is much faster than modulo but the N should be a power of 2 

## Move Semantics
const T& copies the whole object as it sees it as an lvalue and things that it will be used again so it considers the value variable as a non disposable and thus allocates memory and copies data fully creating an overhead
so we use std::move T&& to avoid this overload just as move removes the tag of lvalue from the variable and now compiler see it as temp rvalue and does not copy everything just takes the pointers, capacity and size and make the move before object in a empty but legal state

## Debugging: GDB Session
gdb helps to dry run specific parts of code and seing the change in values of variables at the same time in order to verify code line by line 


## Profiling: strace
strace helps us figure out the latency of our code , the number of system call s and outputs a full table of all time related info and summary of system calls for every aspect of code

## A Real Bug I Caught
what i did is to forget to put a _ in CMAKE_CXX STANDARD it didnt break as
g++ 15.2.0 defaults to C++17 (-std=gnu++17), ensuring full backward compatibility and native support for all stable modern C++ features without triggering the breaking changes introduced in C++20.


## Testing
I coded many use of RingBuffer push and pop and also made cases for FAIL and PASS for the unit testing of CTEST covering the push pop order the empty pop behavior the overwrite correctness and move vs copy overload 
