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
#include <errno.h>
#include <pthread.h>

#include "udp_search.h"
#include "../file_reader.h"
#include "udp_server.h"
#include "tcp_client.h"
#include "tcp_server.h"

#define PORT    8080
#define BUF_SIZE 1024

void search_udp_servers(char *triplet_str) {

    int sockfd;
    int8_t buffer[BUF_SIZE] = {0};
    struct sockaddr_in servaddr;

    // Creating socket file descriptor
    if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    int broadcast = 1;

    setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST,
               &broadcast, sizeof broadcast);

    memset(&servaddr, 0, sizeof(servaddr));

    // Filling server information
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);
    servaddr.sin_addr.s_addr = htonl(INADDR_BROADCAST);

    uint32_t n, len;

    int c = sendto(sockfd, triplet_str, strlen(triplet_str),
           0, (const struct sockaddr *) &servaddr,
           sizeof(servaddr));

    if (c < 0) {
        printf("error: %d\n", errno);
    }

    n = recvfrom(sockfd, (char *)buffer, BUF_SIZE,
                 MSG_WAITALL, (struct sockaddr *) &servaddr,
                 &len);

    buffer[n] = '\0';

    udp_server_answer_t *answer = (udp_server_answer_t *) buffer;

    if (answer->success) {
        printf("found, port: %d\n", answer->port);
        pthread_t *tcp_client = (pthread_t *) malloc(sizeof(pthread_t));
        tcp_server_data_t *server_data = malloc(sizeof(tcp_server_data_t));
        server_data->arg = *answer;
        server_data->ctx = NULL;
        pthread_create(tcp_client, NULL, start_tcp_client, server_data);
    } else {
        printf("not found :(\n");
    }

    close(sockfd);
}