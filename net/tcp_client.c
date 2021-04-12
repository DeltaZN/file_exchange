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
const static char* EXIT_CMD = "ext";

void perform_download(int sockfd, file_triplet_dto_t triplet) {
    tcp_server_request_t request = {0};
    tcp_server_answer_t answer = {0};
    int current_file = open(triplet.filename, O_WRONLY | O_CREAT, 00777);
    int32_t c = 0;
    for (size_t i = 0; i < triplet.filesize; i += 4096) {
        strncpy(request.cmd, GET_CMD, 3);
        request.arg = c;
        write(sockfd, &request, sizeof(request));
        read(sockfd, &answer, sizeof(answer));
        printf("[TCP_CLIENT] received data, len: %d\n", answer.len);
        pwrite(current_file, &answer.payload, answer.len, i);
        c++;
    }
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
    perform_download(sockfd, cd->triplet);

    close(sockfd);
    free(cd);
    return NULL;
}