//
// Created by georgii on 12.04.2021.
//

#include <malloc.h>
#include <string.h>
#include "events_module.h"

list_item_t* find_transfer_progress(list_item_t* transfer_list, transfer_progress_t *tp) {
    list_item_t *item = transfer_list;
    while (item != NULL) {
        transfer_progress_t *item_data = item->data;
        if (item_data &&
            0 == strcmp(item_data->triplet.filename, tp->triplet.filename) &&
            0 == strncmp(item_data->triplet.hash, tp->triplet.hash, 32)
            ) {
            return item;
        }
        item = item->next;
    }
    return NULL;
}

void destroy_transfer_progress(transfer_progress_t *tp) {
    free(tp);
}

void destroy_action_message(char *str) {
    free(str);
}

void init_events_module(events_module_data_t* em) {
    pthread_mutex_init(&em->actions_mutex, NULL);
    pthread_mutex_init(&em->download_mutex, NULL);
    pthread_mutex_init(&em->upload_mutex, NULL);
    em->actions_list = calloc(1, sizeof(list_item_t));
    em->download_list = calloc(1, sizeof(list_item_t));
    em->upload_list = calloc(1, sizeof(list_item_t));
}

void destroy_events_module(events_module_data_t* em) {
    pthread_mutex_destroy(&em->actions_mutex);
    pthread_mutex_destroy(&em->download_mutex);
    pthread_mutex_destroy(&em->upload_mutex);
    destroy_list(em->download_list, (int (*)(void *)) destroy_transfer_progress);
    destroy_list(em->upload_list, (int (*)(void *)) destroy_transfer_progress);
    destroy_list(em->actions_list, (int (*)(void *)) destroy_action_message);
}

void put_download(events_module_data_t* em, transfer_progress_t *progress) {
    pthread_mutex_lock(&em->download_mutex);
    list_item_t *found = find_transfer_progress(em->download_list, progress);
    if (found) {
        transfer_progress_t *data = found->data;
        data->transferred = progress->transferred;
    } else {
        em->download_list = push(em->download_list, progress);
    }
    pthread_mutex_unlock(&em->download_mutex);
}

void del_download(events_module_data_t* em, transfer_progress_t *progress) {
    pthread_mutex_lock(&em->download_mutex);
    list_item_t *found = find_transfer_progress(em->download_list, progress);
    if (found) {
        em->download_list = remove_el(em->download_list, found);
        if (!em->download_list) {
            printf("[EVENTS-MODULE] ERROR on del download");
        }
    } else {
        printf("[EVENTS-MODULE] ERROR on del download");
    }
    pthread_mutex_unlock(&em->download_mutex);
}
void put_upload(events_module_data_t* em, transfer_progress_t *progress) {
    pthread_mutex_lock(&em->upload_mutex);
    list_item_t *found = find_transfer_progress(em->upload_list, progress);
    if (found) {
        transfer_progress_t *data = found->data;
        data->transferred = progress->transferred;
    } else {
        em->upload_list = push(em->upload_list, progress);
    }
    pthread_mutex_unlock(&em->upload_mutex);
}
void del_upload(events_module_data_t* em, transfer_progress_t *progress) {
    pthread_mutex_lock(&em->upload_mutex);
    list_item_t *found = find_transfer_progress(em->upload_list, progress);
    if (found) {
        em->upload_list = remove_el(em->upload_list, found);
        if (!em->upload_list) {
            printf("[EVENTS-MODULE] ERROR on del upload");
        }
    } else {
        printf("[EVENTS-MODULE] ERROR on del upload");
    }
    pthread_mutex_unlock(&em->upload_mutex);
}
void put_action(events_module_data_t* em, char *str) {
    pthread_mutex_lock(&em->actions_mutex);
    em->actions_list = push(em->actions_list, str);
    pthread_mutex_unlock(&em->actions_mutex);
}