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
#include <pthread.h>

#include "udp_server.h"
#include "../app_context.h"
#include "tcp_server.h"

#define PORT 8080
#define BUF_SIZE 1024

file_triplet_t* find_triplet(list_item_t* triplet_list, char *str) {
    list_item_t *item = triplet_list;
    while (item->data != NULL) {
        char triplet_str[1024] = {0};
        file_triplet_t *triplet = ((file_triplet_t *) item->data);
        strcat(triplet_str, triplet->filename);
        strcat(triplet_str, ":");
        char snum[1024];
        sprintf(snum, "%ld", triplet->filesize);
        strcat(triplet_str, snum);
        strcat(triplet_str, ":");
        strncat(triplet_str, triplet->hash, MD5_DIGEST_LENGTH * 2);
        if (!strcmp(str, triplet_str)) {
            return triplet;
        }
        item = item->next;
    }
    return NULL;
}

void *start_udp_server(void *thread_data) {
    app_context_t *ctx = thread_data;
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
        char bind_failed[64] = {0};
        sprintf(bind_failed, "[UDP-server] bind on port %d failed", port++);
        put_action(ctx->events_module, bind_failed);
        serv_addr.sin_port = htons(port);
    }

    char bind_successful[64] = {0};
    sprintf(bind_successful, "[UDP-server] Successfully started server on port %d!", port);
    put_action(ctx->events_module, bind_successful);

    uint32_t len, n;

    len = sizeof(cl_addr);

    while (!ctx->exit) {
        memset(buffer, 0, BUF_SIZE);
        n = recvfrom(sock_fd, (char *) buffer, BUF_SIZE,
                     MSG_WAITALL, (struct sockaddr *) &cl_addr,
                     &len);

        if (n > BUF_SIZE) {
            put_action(ctx->events_module, "[ERROR, UDP-server] too long message");
        }

        file_triplet_t *pTriplet = find_triplet(ctx->triplet_list, buffer);
        char *search_result = calloc(1, 256);
        strcat(search_result, "[UDP-server] searching ");
        strcat(search_result, buffer);
        if (pTriplet) {
            strcat(search_result, " found");
        } else {
            strcat(search_result, " not found");
        }
        put_action(ctx->events_module, search_result);
        free(search_result);

        udp_server_answer_t answer = {0};

        if (pTriplet) {
            pthread_t *tcp_server = (pthread_t *) malloc(sizeof(pthread_t));
            tcp_server_data_t *server_data = malloc(sizeof(tcp_server_data_t));
            server_data->triplet = pTriplet;
            server_data->ctx = ctx;
            init_tcp_server(server_data);

            answer.success = 1;
            answer.port = server_data->port;
            answer.triplet.filesize = pTriplet->filesize;
            strncpy(answer.triplet.hash, pTriplet->hash, 32);
            strcpy(answer.triplet.filename, pTriplet->filename);

            pthread_create(tcp_server, NULL, start_tcp_server, server_data);
        }

        sendto(sock_fd, &answer, sizeof(udp_server_answer_t),
               MSG_CONFIRM, (const struct sockaddr *) &cl_addr,
               len);
    }

    return NULL;
}
