//
// Created by georgii on 24.03.2021.
//

#ifndef SPO_LAB_3_FILE_EXCHANGE_FILE_READER_H
#define SPO_LAB_3_FILE_EXCHANGE_FILE_READER_H

#include <stdint.h>
#include <openssl/md5.h>
#include "list.h"

typedef struct file_triplet {
    char* filename;
    char hash[MD5_DIGEST_LENGTH * 2];
    size_t filesize;
} file_triplet_t;

int8_t run_file_reader(char* dir_path, list_item_t** list_ret);
void destroy_file_triplet(file_triplet_t *triplet);
#endif //SPO_LAB_3_FILE_EXCHANGE_FILE_READER_H
