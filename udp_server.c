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
#define BUF_SIZE 1024

void *start_udp_server(void *thread_data) {
    int sock_fd;
    char buffer[BUF_SIZE] = {0};
    struct sockaddr_in serv_addr, cl_addr;
    if ((sock_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    int broadcast = 1;

    uint16_t port = PORT;

    setsockopt(sock_fd, SOL_SOCKET, SO_BROADCAST,
               &broadcast, sizeof broadcast);

    memset(&serv_addr, 0, sizeof(serv_addr));
    memset(&cl_addr, 0, sizeof(cl_addr));

    // Filling server information
    serv_addr.sin_family = AF_INET; // IPv4
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(port);

    // Bind the socket with the server address
    while (bind(sock_fd, (const struct sockaddr *) &serv_addr,
                sizeof(serv_addr)) < 0) {
        printf("bind on port %d failed\n", port++);
        serv_addr.sin_port = htons(port);
    }

    printf("Successfully started server on port %d!\n", port);

    uint32_t len, n;

    len = sizeof(cl_addr);  //len is value/resuslt

    while (1) {
        n = recvfrom(sock_fd, (char *) buffer, BUF_SIZE,
                     MSG_WAITALL, (struct sockaddr *) &cl_addr,
                     &len);

        if (n > BUF_SIZE) {
            printf("too long message\n");
        }

        const char *confirmed = "C";

        sendto(sock_fd, confirmed, strlen(confirmed),
               MSG_CONFIRM, (const struct sockaddr *) &cl_addr,
               len);

        printf("req: %s\n", buffer);
    }

    return NULL;
}
