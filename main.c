#include <stdio.h>
#include <malloc.h>
#include "file_reader.h"
#include "list.h"
#include "app_context.h"

void print_list(list_item_t *list) {
    list_item_t *item = list;
    while (item->data != NULL) {
        file_triplet_t *triplet = ((file_triplet_t *) item->data);
        printf("file: %s\n", triplet->filename);
        printf("  size: %ld\n", triplet->filesize);
        printf("  hash: ");
        for (size_t i = 0; i < MD5_DIGEST_LENGTH; i++) {
            printf("%02x", triplet->hash[i]);
        }
        printf("\n");
        item = item->next;
    }
}

int main(int argc, char **argv) {
    if (argc < 2) {
        printf("Please, specify working directory\n");
        return 0;
    } else {
        list_item_t *list = calloc(1, sizeof(list_item_t));
        int8_t ret_code = run_file_reader(argv[1], &list);
        if (ret_code == -1) {
            printf("Unable to access directory\n");
            return 0;
        }

        app_context_t *ctx = calloc(1, sizeof(app_context_t));
        ctx->triplet_list = list;
        print_list(list);
        destroy_list(list, (int (*)(void *)) destroy_file_triplet);
    }
}
