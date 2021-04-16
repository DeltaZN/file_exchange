//
// Created by georgii on 11.04.2021.
//

#ifndef SPO_LAB_3_FILE_EXCHANGE_TCP_CLIENT_H
#define SPO_LAB_3_FILE_EXCHANGE_TCP_CLIENT_H
#include <stdint.h>
#include <netinet/in.h>
#include "../file_reader.h"
#include "../app_context.h"

typedef struct tcp_server_answer {
    uint16_t len;
    int8_t payload[4096];
} tcp_server_answer_t;

typedef struct tcp_client_data {
    uint16_t port;
    file_triplet_dto_t triplet;
    in_addr_t server_addr;
    app_context_t *ctx;
} tcp_client_data_t;

void *start_tcp_client(void *thread_data);
#endif //SPO_LAB_3_FILE_EXCHANGE_TCP_CLIENT_H
