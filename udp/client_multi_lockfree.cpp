#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 

#include <thread>
#include <chrono>
#include <iostream>
#include <atomic>
#include <vector>

void error(const char *msg) {
    perror(msg);
    exit(1);
}

void start_reading(std::atomic<int> &count, int &sockfd,
                    int packetSize, int packetCount) {
    std::cout << "Thread : " << std::this_thread::get_id() << " waiting..." << std::endl;

    int n_recv;
    char buf[packetSize];

    using namespace std::chrono_literals;
    while (count<packetCount) {
        sockaddr_in client_addr;
        socklen_t client_addrlen = sizeof(client_addr);

        // Thread safe operation
        n_recv = recvfrom(sockfd, buf, packetSize,
                    MSG_DONTWAIT, (struct sockaddr*) &client_addr, &client_addrlen);

        if (n_recv>=0) {
            count++;
            std::cout << "Thread : " << std::this_thread::get_id() << " Count : " << count << std::endl;
        }

        std::this_thread::sleep_for(50ms);
    }
}

int main() {
    int port = 6000;
    int MAXSIZE = 65535;
    int packetSize = 256;
    int packetCount = 2*MAXSIZE/packetSize;

    int sockfd;
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd<0) {
        error("ERROR opening socket");
    }

    sockaddr_in serv_addr;

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    socklen_t addr_len = sizeof(sockaddr_in);

    if (bind(sockfd, (struct sockaddr*) &serv_addr, addr_len) < 0) {
        error("ERROR binding socket to address");
    } else {
        std::cout << "Thread : " << std::this_thread::get_id() << " port : " << ntohs(serv_addr.sin_port) << std::endl;
    }

    std::atomic<int> count = 0;

    std::vector<std::thread> threads;

    for (int i=0; i<30; i++) {
        threads.push_back(std::thread(start_reading, std::ref(count), std::ref(sockfd),
                                        packetSize, packetCount));
    }

    for (auto &t: threads) {
        t.join();
    }

    close(sockfd);
    return 0;
}
