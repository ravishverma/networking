#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/un.h>
#include <stdio.h>
#include <string.h>
#include <netinet/in.h>

#include <thread>
#include <chrono>
#include <iostream>

void error(const char *msg) {
    perror(msg);
    exit(1);
}

// UDP server
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

    sockaddr_in client_addr;
    client_addr.sin_family = AF_INET;
    client_addr.sin_port = htons(port);
    client_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    socklen_t addr_len = sizeof(client_addr);

    int n_sent;
    int count = 0;

    char buf[packetSize];
    memset(buf, 0, packetSize);

    using namespace std::chrono_literals;
    while (count<packetCount) {
        n_sent = sendto(sockfd, buf, packetSize,
                    0, (struct sockaddr*) &client_addr, addr_len);

        if (n_sent<0) {
            error("ERROR writing");
        }

        count++;

        std::this_thread::sleep_for(1ms);
    }

    std::cout << "Total sent count : " << count << std::endl;
    close(sockfd);
    return 0;
}
