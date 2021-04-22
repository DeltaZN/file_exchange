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
#include "udp_server.h"
#include "tcp_client.h"

#define PORT    8080
#define BUF_SIZE 1024

void *search_udp_servers(void *thread_data) {
    udp_client_data_t *udp_cd = thread_data;
    char *triplet_str = udp_cd->triplet_str;

    int sockfd;
    int8_t buffer[BUF_SIZE] = {0};
    struct sockaddr_in servaddr, cl_addr;

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        log_error(udp_cd->ctx->events_module, "[ERROR, UDP-search] couldn't create socket (%d)");
        return NULL;
    }

    int broadcast = 1;

    struct timeval tv;
    tv.tv_sec = 1;
    tv.tv_usec = 0;

    if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0) {
        log_error(udp_cd->ctx->events_module, "[ERROR, UDP-search] couldn't set receive timeout (%d)");
        return NULL;
    }

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
        log_error(udp_cd->ctx->events_module, "[ERROR, UDP-search] (%d)");
        return NULL;
    }

    int8_t received_smth = 0;

    while (1) {
        memset(buffer, 0, BUF_SIZE);
        n = recvfrom(sockfd, (char *) buffer, BUF_SIZE,
                     MSG_WAITALL, (struct sockaddr *) &cl_addr,
                     &len);

        if (-1 == n) {
            if (received_smth) {
                return NULL;
            }
            put_action(udp_cd->ctx->events_module, "[UDP-search] file not found");
            break;
        }
        received_smth = 1;

        buffer[n] = '\0';

        udp_server_answer_t *answer = (udp_server_answer_t *) buffer;

        if (answer->success) {
            pthread_t *tcp_client = (pthread_t *) malloc(sizeof(pthread_t));
            tcp_client_data_t *tcp_cd = malloc(sizeof(tcp_client_data_t));
            tcp_cd->port = answer->port;
            tcp_cd->triplet = answer->triplet;
            tcp_cd->server_addr = cl_addr.sin_addr.s_addr;
            tcp_cd->ctx = udp_cd->ctx;
            pthread_create(tcp_client, NULL, start_tcp_client, tcp_cd);
        }
    }

    close(sockfd);
    free(triplet_str);
    free(udp_cd);

    return NULL;
}