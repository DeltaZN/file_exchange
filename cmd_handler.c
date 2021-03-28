//
// Created by georgii on 25.03.2021.
//

#include <malloc.h>
#include <stdint.h>
#include <string.h>
#include "cmd_handler.h"
#include "util.h"
#include "file_reader.h"
#include "udp_search.h"

const static char* DISPLAY_CMD = "display";
const static char* DOWNLOAD_CMD = "download";
const static char* EXIT_CMD = "exit";
const static char* HELP_CMD = "help";

void display_cmd(list_item_t* triplet_list, const char* path) {
    list_item_t *item = triplet_list;
    while (item->data != NULL) {
        file_triplet_t *triplet = ((file_triplet_t *) item->data);
        if (!strcmp(triplet->filename, path)) {
            printf("%s:", triplet->filename);
            printf("%ld:", triplet->filesize);
            for (int i = 0; i < MD5_DIGEST_LENGTH * 2; ++i) {
                printf("%c", triplet->hash[i]);
            }
            printf("\n");
        }
        item = item->next;
    }
}

void download_cmd(char* triplet) {
    search_udp_servers(triplet);
}

void help_cmd() {
    printf("display [filename]\n");
    printf("download [file triplet]\n");
    printf("help\n");
    printf("exit\n");
}

/** non-zero value == terminate program */
int8_t handle_command(app_context_t* ctx, const char* cmd) {
    int8_t ret_code = 0;
    char *args[3];
    args[0] = calloc(1, 256);
    args[1] = calloc(1, 256);
    args[2] = calloc(1, 256);
    parse(cmd, args);

    if (!strcmp(args[0], DISPLAY_CMD)) {
        display_cmd(ctx->triplet_list, args[1]);
    } else if (!strcmp(args[0], DOWNLOAD_CMD)) {
        download_cmd(args[1]);
    } else if (!strcmp(args[0], HELP_CMD)) {
        help_cmd();
    } else if (!strcmp(args[0], EXIT_CMD)) {
        ret_code = 1;
    } else {
        printf("unknown cmd\n");
    }

    free(args[0]);
    free(args[1]);
    free(args[2]);

    return ret_code;
}
