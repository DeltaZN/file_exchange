//
// Created by georgii on 25.03.2021.
//

#include <malloc.h>
#include <stdint.h>
#include <string.h>
#include <bits/pthreadtypes.h>
#include <pthread.h>
#include "cmd_handler.h"
#include "util.h"
#include "file_reader.h"
#include "net/udp_search.h"

const static char* DISPLAY_CMD = "display";
const static char* DOWNLOAD_CMD = "download";
const static char* EXIT_CMD = "exit";
const static char* HELP_CMD = "help";

const static char* unknown_cmd = "unknown cmd";

const static char* help_display = "display [filename]";
const static char* help_download = "download [file triplet]";
const static char* help_exit = "display [filename]";
const static char* help_help = "exit [filename]";

void display_cmd(app_context_t *ctx, const char* path) {
    list_item_t *item = ctx->triplet_list;
    while (item->data != NULL) {
        file_triplet_t *triplet = ((file_triplet_t *) item->data);
        if (!strcmp(triplet->filename, path)) {
            char *triplet_str = calloc(1, 256);
            char *filesize_str = calloc(1, 16);

            strcat(triplet_str, triplet->filename);
            strcat(triplet_str, ":");
            sprintf(filesize_str, "%ld", triplet->filesize);
            strcat(triplet_str, filesize_str);
            strcat(triplet_str, ":");
            strncat(triplet_str, triplet->hash, 32);

            put_action(ctx->events_module, triplet_str);
            free(triplet_str);
            free(filesize_str);
            return;
        }
        item = item->next;
    }
    char *str = calloc(1, 256);
    sprintf(str, "Could find file with name %s", path);
    put_action(ctx->events_module, str);
    free(str);
}

void download_cmd(char* triplet, app_context_t *ctx) {
    char *triplet_str = calloc(1, 512);
    strcpy(triplet_str, triplet);
    udp_client_data_t *udp_cd = malloc(sizeof(udp_client_data_t));
    udp_cd->triplet_str = triplet_str;
    udp_cd->ctx = ctx;
    pthread_t *search_udp = (pthread_t *) malloc(sizeof(pthread_t));
    pthread_create(search_udp, NULL, search_udp_servers, udp_cd);
}

void help_cmd(app_context_t* ctx) {
    put_action(ctx->events_module, help_display);
    put_action(ctx->events_module, help_download);
    put_action(ctx->events_module, help_exit);
    put_action(ctx->events_module, help_help);
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
        display_cmd(ctx, args[1]);
    } else if (!strcmp(args[0], DOWNLOAD_CMD)) {
        download_cmd(args[1], ctx);
    } else if (!strcmp(args[0], HELP_CMD)) {
        help_cmd(ctx);
    } else if (!strcmp(args[0], EXIT_CMD)) {
        ctx->exit = 1;
        ret_code = 1;
    } else {
        put_action(ctx->events_module, unknown_cmd);
    }

    free(args[0]);
    free(args[1]);
    free(args[2]);

    return ret_code;
}
