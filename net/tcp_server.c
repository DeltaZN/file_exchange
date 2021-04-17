//
// Created by georgii on 11.04.2021.
//

#include <stdio.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <stddef.h>
#include <unistd.h>
#include <fcntl.h>
#include "tcp_server.h"
#include "tcp_client.h"

void serve_client(int sockfd, file_triplet_t *triplet, app_context_t *ctx) {
    int current_file = open(triplet->filename, O_RDONLY, 00666);
    tcp_server_request_t request;
    tcp_server_answer_t answer;
    transfer_progress_t progress = {0};
    transfer_progress_t *cur_progress = &progress;
    list_item_t *existing_upload = find_upload(ctx->events_module, cur_progress);
    if (existing_upload) {
        cur_progress = existing_upload->data;
    } else {
        progress.triplet.filesize = triplet->filesize;
        strncpy(progress.triplet.hash, triplet->hash, 32);
        strcpy(progress.triplet.filename, triplet->filename);
        put_upload(ctx->events_module, &progress);
    }
    while (0 != strncmp("ext", request.cmd, 3)) {
        read(sockfd, &request, sizeof(request));
        if (0 == strncmp("get", request.cmd, 3)) {
            uint16_t size = 4096;
            if (triplet->filesize < size * request.arg + 4096) {
                size = triplet->filesize - size * request.arg;
            }
            pread(current_file, answer.payload, size, 4096 * request.arg);
            answer.len = size;
            write(sockfd, &answer, sizeof(answer));
        } else if (0 == strncmp("prg", request.cmd, 3)) {
            cur_progress->transferred = request.arg;
            put_upload(ctx->events_module, cur_progress);
        }
    }
    del_upload(ctx->events_module, cur_progress);
    close(current_file);
}

void init_tcp_server(tcp_server_data_t *sd) {
    sd->sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sd->sockfd == -1) {
        put_action(sd->ctx->events_module, "[ERROR, TCP-server] TCP socket creation failed...");
        return;
    }

    bzero(&sd->servaddr, sizeof(struct sockaddr_in));

    uint16_t port_test = 2421;

    sd->servaddr.sin_family = AF_INET;
    sd->servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    sd->servaddr.sin_port = htons(port_test);

    while (bind(sd->sockfd, (const struct sockaddr *) &sd->servaddr,
                sizeof(struct sockaddr_in)) < 0) {
        port_test++;
        sd->servaddr.sin_port = htons(port_test);
    }
    sd->port = port_test;
}


void *start_tcp_server(void *thread_data) {
    tcp_server_data_t *sd = thread_data;
    int32_t connfd, len;

    if ((listen(sd->sockfd, 5)) != 0) {
        printf("[ERROR, TCP-server] Listen failed...\n");
        exit(0);
    }
    len = sizeof(sd->client);

    connfd = accept(sd->sockfd, (struct sockaddr *) &sd->client, &len);
    if (connfd < 0) {
        printf("[ERROR, TCP-server] server accept failed...\n");
        exit(0);
    }

    char *start_upload = calloc(1, 256);
    strcat(start_upload, "Started uploading file ");
    strcat(start_upload, sd->triplet->filename);
    put_action(sd->ctx->events_module, start_upload);
    serve_client(connfd, sd->triplet, sd->ctx);

    char *finished_upload = calloc(1, 256);
    strcat(finished_upload, "Upload finished ");
    strcat(finished_upload, sd->triplet->filename);
    put_action(sd->ctx->events_module, finished_upload);

    close(sd->sockfd);
    free(sd);
    return NULL;
}