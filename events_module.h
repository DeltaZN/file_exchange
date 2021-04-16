//
// Created by georgii on 12.04.2021.
//

#ifndef SPO_LAB_3_FILE_EXCHANGE_EVENTS_MODULE_H
#define SPO_LAB_3_FILE_EXCHANGE_EVENTS_MODULE_H

#include <bits/pthreadtypes.h>
#include <pthread.h>
#include "list.h"
#include "file_reader.h"

typedef struct transfer_progress {
    size_t transferred;
    file_triplet_dto_t triplet;
} transfer_progress_t;

typedef struct events_module_data {
    pthread_mutex_t upload_mutex;
    pthread_mutex_t download_mutex;
    pthread_mutex_t actions_mutex;
    list_item_t *upload_list;
    list_item_t *download_list;
    list_item_t *actions_list;
} events_module_data_t;

void init_events_module(events_module_data_t* em);
void destroy_events_module(events_module_data_t* em);

void put_download(events_module_data_t* em, transfer_progress_t *progress);
void del_download(events_module_data_t* em, transfer_progress_t *progress);
void put_upload(events_module_data_t* em, transfer_progress_t *progress);
void del_upload(events_module_data_t* em, transfer_progress_t *progress);
void put_action(events_module_data_t* em, char *str);
#endif //SPO_LAB_3_FILE_EXCHANGE_EVENTS_MODULE_H
