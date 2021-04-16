//
// Created by georgii on 27.03.2021.
//

#ifndef SPO_LAB_3_FILE_EXCHANGE_UDP_SEARCH_H
#define SPO_LAB_3_FILE_EXCHANGE_UDP_SEARCH_H
#include "../app_context.h"

typedef struct udp_client_data {
    char *triplet_str;
    app_context_t *ctx;
} udp_client_data_t;

void *search_udp_servers(void *thread_data);
#endif //SPO_LAB_3_FILE_EXCHANGE_UDP_SEARCH_H
