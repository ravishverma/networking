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
    int MAXSIZE = 65535;
    int FLOODSIZE = 256;
    int FLOODCOUNT = 2*MAXSIZE/FLOODSIZE;

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

    int n;
    int count = 0;

    char buf[FLOODSIZE];

    using namespace std::chrono_literals;
    while (count<FLOODCOUNT) {
        n = recvfrom(sockfd, buf, FLOODSIZE,
                    0, (struct sockaddr*) &serv_addr, &addr_len);
        count++;

        std::cout << "Count : " << count << std::endl;
        std::this_thread::sleep_for(5ms);
    }

    close(sockfd);
    return 0;
}
