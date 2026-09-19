#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <chrono>

int main() {
    // 1. create a UDP socket
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        std::cerr << "socket() failed\n";
        return 1;
    }

    // 2. bind it to a local address/port so it can receive packets
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(9999);

    if (bind(sockfd, (sockaddr*)&addr, sizeof(addr)) < 0) {
        std::cerr << "bind() failed\n";
        return 1;
    }

    std::cout << "Listening on UDP port 9999...\n";

    // 3. receive loop
    char buffer[1024];
    while (true) {
        ssize_t n = recvfrom(sockfd, buffer, sizeof(buffer) - 1, 0, nullptr, nullptr);
        
        if (n > 0) {
            auto received_time = std::chrono::high_resolution_clock::now();  // capture FIRST
            auto received_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(received_time.time_since_epoch()).count();

            buffer[n] = '\0';
            long sent_ns = std::stol(buffer);
            long latency_ns = received_ns - sent_ns;

            std::cout << "Received: " << buffer << "\n";       // print AFTER, doesn't pollute the measurement
            std::cout << "Latency: " << latency_ns << " ns\n";
        }
    }

    close(sockfd);
    return 0;
}