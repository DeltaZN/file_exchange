//
// Created by georgii on 27.03.2021.
//

#ifndef SPO_LAB_3_FILE_EXCHANGE_UDP_SERVER_H
#define SPO_LAB_3_FILE_EXCHANGE_UDP_SERVER_H
#include "file_reader.h"

typedef struct udp_server_answer {
    int8_t success;
    uint16_t port;
    file_triplet_t triplet;
} udp_server_answer_t;

void *start_udp_server(void *thread_data);
#endif //SPO_LAB_3_FILE_EXCHANGE_UDP_SERVER_H
