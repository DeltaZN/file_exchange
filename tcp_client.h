//
// Created by georgii on 11.04.2021.
//

#ifndef SPO_LAB_3_FILE_EXCHANGE_TCP_CLIENT_H
#define SPO_LAB_3_FILE_EXCHANGE_TCP_CLIENT_H

#include <stdint.h>

typedef struct tcp_server_answer {
    int16_t len;
    int8_t payload[4096];
} tcp_server_answer_t;
void *start_tcp_client(void *thread_data);
#endif //SPO_LAB_3_FILE_EXCHANGE_TCP_CLIENT_H
