//
// Created by georgii on 11.04.2021.
//

#include "tcp_client.h"
#include "tcp_server.h"

#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>


void func_client(int sockfd) {
    tcp_server_request_t request = {0};
    char get_cmd[3] = "get";
    char exit_cmd[3] = "ext";
    strncpy(request.cmd, get_cmd, 3);
    write(sockfd, &request, sizeof(request));
    strncpy(request.cmd, exit_cmd, 3);
    write(sockfd, &request, sizeof(request));
    printf("Client Exit...\n");
}

void *start_tcp_client(void *thread_data) {
    tcp_server_data_t *server_data = thread_data;

    int sockfd, connfd;
    struct sockaddr_in servaddr, cli;

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
    servaddr.sin_port = htons(server_data->arg.port);

    // connect the client socket to server socket
    if (connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) != 0) {
        printf("connection with the server failed...\n");
        exit(0);
    } else
        printf("connected to the server..\n");

    // function for chat
    func_client(sockfd);

    // close the socket
    close(sockfd);

    return NULL;
}