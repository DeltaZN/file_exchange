//
// Created by georgii on 25.03.2021.
//

#ifndef SPO_LAB_3_FILE_EXCHANGE_APP_CONTEXT_H
#define SPO_LAB_3_FILE_EXCHANGE_APP_CONTEXT_H

#include <stdint.h>
#include "list.h"

typedef struct app_context {
    list_item_t* triplet_list;
    int8_t exit;
} app_context_t;

#endif //SPO_LAB_3_FILE_EXCHANGE_APP_CONTEXT_H
