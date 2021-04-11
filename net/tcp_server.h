//
// Created by georgii on 11.04.2021.
//

#ifndef SPO_LAB_3_FILE_EXCHANGE_TCP_SERVER_H
#define SPO_LAB_3_FILE_EXCHANGE_TCP_SERVER_H
#include "../app_context.h"
#include "udp_server.h"
#include <netinet/in.h>
#include <sys/socket.h>;

typedef struct tcp_server_request {
    char cmd[3];
    size_t arg;
} tcp_server_request_t;

typedef struct tcp_server_data {
    int32_t sockfd;
    uint16_t port;
    struct sockaddr_in servaddr;
    struct sockaddr_in client;
    file_triplet_t *triplet;
} tcp_server_data_t;

void init_tcp_server(tcp_server_data_t *sd);
void *start_tcp_server(void *thread_data);
#endif //SPO_LAB_3_FILE_EXCHANGE_TCP_SERVER_H
