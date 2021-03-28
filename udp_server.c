//
// Created by georgii on 27.03.2021.
//

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include "udp_server.h"

#define PORT 8080
#define BUF_SIZE 2

void *start_udp_server(void *thread_data) {
    int sock_fd;
    char buffer[BUF_SIZE];
    struct sockaddr_in serv_addr, cl_addr;
    if ((sock_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&serv_addr, 0, sizeof(serv_addr));
    memset(&cl_addr, 0, sizeof(cl_addr));

    // Filling server information
    serv_addr.sin_family = AF_INET; // IPv4
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(PORT);

    // Bind the socket with the server address
    if (bind(sock_fd, (const struct sockaddr *) &serv_addr,
             sizeof(serv_addr)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    uint32_t len, n;

    len = sizeof(cl_addr);  //len is value/resuslt

    n = recvfrom(sock_fd, (char *)buffer, BUF_SIZE,
                 MSG_WAITALL, ( struct sockaddr *) &cl_addr,
                 &len);

    const char *confirmed = "C";

    sendto(sock_fd, confirmed, strlen(confirmed),
           MSG_CONFIRM, (const struct sockaddr*) &cl_addr,
           len);

    return NULL;
}
