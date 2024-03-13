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

void error(const char *msg) {
    perror(msg);
    exit(1);
}

int main() {
    int port = 6000;
    int packetSize = 256;

    int sockfd;
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd<0) {
        error("ERROR opening socket");
    }

    sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    socklen_t addr_len = sizeof(serv_addr);

    if (bind(sockfd, (struct sockaddr*) &serv_addr, addr_len) < 0) {
        error("ERROR binding socket to address");
    }

    int n_recv;
    int count = 0;

    char buf[packetSize];

    using namespace std::chrono_literals;
    
    bool started = false;
    while (true) {
        n_recv = recvfrom(sockfd, buf, packetSize,
                    MSG_DONTWAIT, (struct sockaddr*) &serv_addr, &addr_len);

        if (n_recv<0) {
            if (started) {
                break;
            }
        } else {
            if (!started) {
                started = true;
            }
            count++;
        }

        // Fake process that takes time to complete
        std::this_thread::sleep_for(50ms);
    }

    std::cout << "Single threaded client receive count : " << count << std::endl;
    close(sockfd);
    return 0;
}
