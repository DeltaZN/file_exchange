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

void serve_client(int sockfd, file_triplet_t *triplet) {
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
}

void init_tcp_server(tcp_server_data_t *sd) {
    sd->sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sd->sockfd == -1) {
        printf("[ERROR, TCP-server] TCP socket creation failed...\n");
        return;
    }

    bzero(&sd->servaddr, sizeof(struct sockaddr_in));

    uint16_t port_test = 2421;

    sd->servaddr.sin_family = AF_INET;
    sd->servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    sd->servaddr.sin_port = htons(port_test);

    while (bind(sd->sockfd, (const struct sockaddr *) &sd->servaddr,
                sizeof(struct sockaddr_in)) < 0) {
        port_test++;
        sd->servaddr.sin_port = htons(port_test);
    }
    sd->port = port_test;
}


void *start_tcp_server(void *thread_data) {
    tcp_server_data_t *sd = thread_data;
    int32_t connfd, len;

    if ((listen(sd->sockfd, 5)) != 0) {
        printf("[ERROR, TCP-server] Listen failed...\n");
        exit(0);
    }
    len = sizeof(sd->client);

    connfd = accept(sd->sockfd, (struct sockaddr *) &sd->client, &len);
    if (connfd < 0) {
        printf("[ERROR, TCP-server] server accept failed...\n");
        exit(0);
    }

    serve_client(connfd, sd->triplet);

    close(sd->sockfd);
    free(sd);
    return NULL;
}