//
// Created by georgii on 11.04.2021.
//

#include <stdio.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <stddef.h>
#include <unistd.h>
#include <fcntl.h>
#include "tcp_server.h"
#include "tcp_client.h"

// Function designed for chat between client and server.
void func(int sockfd, file_triplet_t *triplet) {
    int current_file = open(triplet->filename, O_RDONLY, 00666);
    tcp_server_request_t request;
    tcp_server_answer_t answer;
    while (0 != strncmp("ext", request.cmd, 3)) {
        read(sockfd, &request, sizeof(request));
        printf("From client: %s\n", request.cmd);
        if (0 == strncmp("get", request.cmd, 3)) {
            uint16_t size = 4096;
            if (triplet->filesize < size * request.arg + 4096) {
                size = triplet->filesize - size * request.arg;
            }
            pread(current_file, answer.payload, size, 4096 * request.arg);
            answer.len = size;
            write(sockfd, &answer, sizeof(answer));
        }
    }
    close(current_file);
    printf("TCP Server Exit...\n");
}

void init_tcp_server(tcp_server_data_t *sd) {
    sd->sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sd->sockfd == -1) {
        printf("TCP socket creation failed...\n");
        return;
    } else {
        printf("TCP Socket successfully created..\n");
    }

    bzero(&sd->servaddr, sizeof(struct sockaddr_in));

    uint16_t port_test = 2421;

    // assign IP, PORT
    sd->servaddr.sin_family = AF_INET;
    sd->servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    sd->servaddr.sin_port = htons(port_test);

    while (bind(sd->sockfd, (const struct sockaddr *) &sd->servaddr,
                sizeof(struct sockaddr_in)) < 0) {
        printf("TCP bind on port %d failed\n", port_test++);
        sd->servaddr.sin_port = htons(port_test);
    }
    sd->port = port_test;
    printf("TCP Socket successfully binded..\n");
}


void *start_tcp_server(void *thread_data) {
    tcp_server_data_t *sd = thread_data;
    int32_t connfd, len;

    if ((listen(sd->sockfd, 5)) != 0) {
        printf("TCP Listen failed...\n");
        exit(0);
    } else
        printf("TCP Server listening..\n");
    len = sizeof(sd->client);

    // Accept the data packet from client and verification
    connfd = accept(sd->sockfd, (struct sockaddr *) &sd->client, &len);
    if (connfd < 0) {
        printf("TCP server accept failed...\n");
        exit(0);
    } else
        printf("TCP server accept the client...\n");

    // Function for chatting between client and server
    func(connfd, sd->triplet);

    // After chatting close the socket
    close(sd->sockfd);
    free(sd);
    return NULL;
}