//
// Created by georgii on 24.03.2021.
//

#include <malloc.h>
#include "list.h"

list_item_t *push(list_item_t *pp, void *data) {
    list_item_t *p = malloc(sizeof(list_item_t));
    p->next = pp;
    p->data = data;
    return p;
}

list_item_t *remove_el(list_item_t *pp, list_item_t *p) {
    list_item_t *item = pp;
    if (item == p) {
        return item->next;
    }
    while (item != NULL) {
        if (item->next == p) {
            item->next = p->next;
            p->next = NULL;
            return pp;
        } 
        item = item->next;
    }
    return NULL;
}

void destroy_list(list_item_t* list, int (*destroy_data)(void*)) {
    list_item_t *item = list;
    while (item != NULL) {
        if (item->data != NULL) {
            destroy_data(item->data);
        }
        list_item_t *prev = item;
        item = item->next;
        free(prev);
    }
}
