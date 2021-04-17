//
// Created by georgii on 11.04.2021.
//

#include "tcp_client.h"
#include "tcp_server.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <fcntl.h>

const static char* GET_CMD = "get";
const static char* PROGRESS_CMD = "prg";
const static char* EXIT_CMD = "ext";

void perform_download(int sockfd, file_triplet_dto_t triplet, app_context_t *ctx) {
    transfer_progress_t progress = {0};
    transfer_progress_t *cur_progress = &progress;
    progress.triplet = triplet;
    list_item_t *existing_download = find_download(ctx->events_module, &progress);
    if (existing_download) {
        cur_progress = existing_download->data;
    } else {
        put_download(ctx->events_module, &progress);
    }
    tcp_server_request_t request = {0};
    tcp_server_answer_t answer = {0};
    int current_file = open(triplet.filename, O_WRONLY | O_CREAT, 00777);
    while (cur_progress->global * 4096 < triplet.filesize) {
        strncpy(request.cmd, GET_CMD, 3);
        request.arg = cur_progress->global++;
        if (request.arg * 4096 > triplet.filesize) {
            break;
        }
        write(sockfd, &request, sizeof(request));
        read(sockfd, &answer, sizeof(answer));
        cur_progress->transferred += answer.len;
        put_download(ctx->events_module, cur_progress);
        pwrite(current_file, &answer.payload, answer.len, request.arg * 4096);
        // send progress status to the server
        strncpy(request.cmd, PROGRESS_CMD, 3);
        request.arg = cur_progress->transferred;
        write(sockfd, &request, sizeof(request));
    }
    del_download(ctx->events_module, cur_progress);
    close(current_file);
    strncpy(request.cmd, EXIT_CMD, 3);
    write(sockfd, &request, sizeof(request));
}

void *start_tcp_client(void *thread_data) {
    tcp_client_data_t *cd = thread_data;
    int sockfd;
    struct sockaddr_in servaddr;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        printf("[ERROR, TCP_CLIENT] socket creation failed!\n");
        exit(0);
    }
    bzero(&servaddr, sizeof(servaddr));

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = cd->server_addr;
    servaddr.sin_port = htons(cd->port);

    if (connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) != 0) {
        printf("[ERROR, TCP_CLIENT] connection with the server failed!\n");
        exit(0);
    }

    char *start_download = calloc(1, 256);
    strcat(start_download, "Started downloading file ");
    strcat(start_download, cd->triplet.filename);
    put_action(cd->ctx->events_module, start_download);
    perform_download(sockfd, cd->triplet, cd->ctx);
    char *finish_download = calloc(1, 256);
    strcat(finish_download, "Finished downloading file ");
    strcat(finish_download, cd->triplet.filename);
    put_action(cd->ctx->events_module, finish_download);

    close(sockfd);
    free(cd);
    return NULL;
}