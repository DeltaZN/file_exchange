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


void func_client(int sockfd, file_triplet_dto_t triplet) {
    tcp_server_request_t request = {0};
    char get_cmd[3] = "get";
    char exit_cmd[3] = "ext";
    tcp_server_answer_t answer = {0};
    int current_file = open(triplet.filename, O_WRONLY | O_CREAT, 00777);
    int32_t c = 0;
    for (size_t i = 0; i < triplet.filesize; i += 4096) {
        strncpy(request.cmd, get_cmd, 3);
        request.arg = c;
        write(sockfd, &request, sizeof(request));
        read(sockfd, &answer, sizeof(answer));
        printf("received data, len: %d\n", answer.len);
        pwrite(current_file, &answer.payload, answer.len, i);
        c++;
    }
    close(current_file);
    strncpy(request.cmd, exit_cmd, 3);
    write(sockfd, &request, sizeof(request));
    printf("Client Exit...\n");
}

void *start_tcp_client(void *thread_data) {
    tcp_client_data_t *cd = thread_data;
    int sockfd;
    struct sockaddr_in servaddr;

    // socket create and varification
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        printf("socket creation failed...\n");
        exit(0);
    } else
        printf("Socket successfully created..\n");
    bzero(&servaddr, sizeof(servaddr));

    // assign IP, PORT
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    servaddr.sin_port = htons(cd->port);

    // connect the client socket to server socket
    if (connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) != 0) {
        printf("connection with the server failed...\n");
        exit(0);
    } else
        printf("connected to the server..\n");

    // function for chat
    func_client(sockfd, cd->triplet);

    // close the socket
    close(sockfd);
    free(cd);
    return NULL;
}