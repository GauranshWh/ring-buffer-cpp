#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <chrono>

int main() {
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(9999);
    inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr);

    auto now = std::chrono::high_resolution_clock::now();
    auto ns = std::chrono::duration_cast<std::chrono::nanoseconds>(now.time_since_epoch()).count();
    std::string msg = std::to_string(ns);
    sendto(sockfd, msg.c_str(), msg.size(), 0, (sockaddr*)&addr, sizeof(addr));

    std::cout << "Sent: " << msg << "\n";

    close(sockfd);
    return 0;
}