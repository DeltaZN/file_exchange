//
// Created by georgii on 27.03.2021.
//

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <pthread.h>

#include "udp_search.h"
#include "../file_reader.h"
#include "udp_server.h"
#include "tcp_client.h"

#define PORT    8080
#define BUF_SIZE 1024

void *search_udp_servers(void *thread_data) {
    char *triplet_str = thread_data;

    int sockfd;
    int8_t buffer[BUF_SIZE] = {0};
    struct sockaddr_in servaddr, cl_addr;

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
        perror("[ERROR, UDP-search] socket creation failed");
        exit(EXIT_FAILURE);
    }

    int broadcast = 1;

    setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST,
               &broadcast, sizeof broadcast);

    memset(&servaddr, 0, sizeof(servaddr));
    memset(&cl_addr, 0, sizeof(cl_addr));

    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);
    servaddr.sin_addr.s_addr = htonl(INADDR_BROADCAST);

    size_t n, len;

    len = sizeof(cl_addr);

    size_t c = sendto(sockfd, triplet_str, strlen(triplet_str),
           0, (const struct sockaddr *) &servaddr,
           sizeof(servaddr));

    if (c < 0) {
        printf("[ERROR, UDP-search]: %d\n", errno);
    }

    n = recvfrom(sockfd, (char *)buffer, BUF_SIZE,
                 MSG_WAITALL, (struct sockaddr *) &cl_addr,
                 &len);

    if (-1 == n) {
        printf("[ERROR, UDP-search] couldn't receive data\n");
    }

    buffer[n] = '\0';

    udp_server_answer_t *answer = (udp_server_answer_t *) buffer;

    if (answer->success) {
        printf("[UDP-search] found, port: %d\n", answer->port);
        pthread_t *tcp_client = (pthread_t *) malloc(sizeof(pthread_t));
        tcp_client_data_t *cd = malloc(sizeof(tcp_client_data_t));
        cd->port = answer->port;
        cd->triplet = answer->triplet;
        cd->server_addr = cl_addr.sin_addr.s_addr;
        pthread_create(tcp_client, NULL, start_tcp_client, cd);
    } else {
        printf("[UDP-search] not found :(\n");
    }

    close(sockfd);
    free(triplet_str);

    return NULL;
}