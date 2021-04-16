//
// Created by georgii on 24.03.2021.
//

#ifndef SPO_LAB_3_FILE_EXCHANGE_LIST_H
#define SPO_LAB_3_FILE_EXCHANGE_LIST_H

typedef struct list_item {
    struct list_item *next;
    void *data;
} list_item_t;

list_item_t *push(list_item_t *pp, void *data);
list_item_t *remove_el(list_item_t *pp, list_item_t *p);
void destroy_list(list_item_t* list, int (*destroy_data)(void*));
#endif //SPO_LAB_3_FILE_EXCHANGE_LIST_H
