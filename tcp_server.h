//
// Created by georgii on 11.04.2021.
//

#ifndef SPO_LAB_3_FILE_EXCHANGE_TCP_SERVER_H
#define SPO_LAB_3_FILE_EXCHANGE_TCP_SERVER_H

#include "app_context.h"
#include "udp_server.h"

typedef struct tcp_server_request {
    char cmd[3];
    size_t arg;
} tcp_server_request_t;

typedef struct tcp_server_data {
    app_context_t *ctx;
    udp_server_answer_t arg;
} tcp_server_data_t;

void *start_tcp_server(void *thread_data);
#endif //SPO_LAB_3_FILE_EXCHANGE_TCP_SERVER_H
