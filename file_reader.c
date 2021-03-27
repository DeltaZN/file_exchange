//
// Created by georgii on 24.03.2021.
//

#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <stdint.h>
#include <openssl/md5.h>
#include "list.h"
#include "file_reader.h"
#include "util.h"

static inline uint8_t is_special_dir(char *dir_name) {
    return 0 == strcmp(".", dir_name) || 0 == strcmp("..", dir_name);
}

static inline uint8_t is_file(struct dirent *entry) {
    return 8 == entry->d_type;
}

static inline uint8_t is_subdir(struct dirent *entry) {
    return 4 == entry->d_type && !is_special_dir(entry->d_name);
}

void destroy_file_triplet(file_triplet_t *triplet) {
    free(triplet->filename);
    free(triplet);
}

void calc_hash(FILE *inFile, uint8_t *hash) {
    MD5_CTX mdContext;
    unsigned char data[1024];
    MD5_Init (&mdContext);
    size_t bytes;
    while ((bytes = fread (data, 1, 1024, inFile)) != 0)
        MD5_Update(&mdContext, data, bytes);
    MD5_Final(hash ,&mdContext);
}

list_item_t* populate_list(list_item_t *list, const char* dir_path, const char* file_name) {
    char filename[512] = {0};
    strcpy(filename, dir_path);
    strcat(filename, "/");
    strcat(filename, file_name);

    file_triplet_t *new_el = calloc(1, sizeof(file_triplet_t));

    FILE *inFile = fopen(filename, "rb");

    new_el->filename = calloc(1, 256);
    strcpy(new_el->filename, file_name);

    fseek(inFile, 0, SEEK_END);
    new_el->filesize = ftell(inFile);
    fseek(inFile, 0, SEEK_SET);

    uint8_t hash[MD5_DIGEST_LENGTH] = {0};
    calc_hash(inFile, hash);

    for (int i = 0; i < MD5_DIGEST_LENGTH; ++i) {
        char hex_chars[2];
        get_hex_chars(hash[i], hex_chars);
        new_el->hash[2*i] = hex_chars[0];
        new_el->hash[2*i + 1] = hex_chars[1];
    }

    fclose (inFile);
    return push(list, new_el);
}

void visit_dir(list_item_t **list, char *dir_path, char *dir_name) {
    char full_path[512] = {0};
    strcpy(full_path, dir_path);
    strcat(full_path, "/");
    strcat(full_path, dir_name);
    DIR *dir = opendir(full_path);
    if (dir == 0x0) {
        printf("That shouldn't happen\n");
        return;
    }
    struct dirent *entry;
    while ((entry = readdir(dir))) {
        if (is_file(entry)) {
            *list = populate_list(*list, full_path, entry->d_name);
        }
        if (is_subdir(entry)) {
            visit_dir(list, full_path, entry->d_name);
        }
    }
    closedir(dir);
}

int8_t run_file_reader(char* dir_path, list_item_t** list_ret) {
    list_item_t *list = calloc(1, sizeof(list_item_t));
    DIR *dir = opendir(dir_path);
    if (dir == 0x0) {
        return -1;
    }
    struct dirent *entry;
    while ((entry = readdir(dir))) {
        if (is_file(entry)) {
            list = populate_list(list, dir_path, entry->d_name);
        }
        if (is_subdir(entry)) {
            visit_dir(&list, dir_path, entry->d_name);
        }
    }
    closedir(dir);
    *list_ret = list;
    return 0;
}
