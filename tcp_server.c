//
// Created by georgii on 11.04.2021.
//

#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <stddef.h>
#include <unistd.h>
#include "tcp_server.h"

// Function designed for chat between client and server.
void func(int sockfd)
{
    tcp_server_request_t request;
    // infinite loop for chat
    while (0 != strncmp("ext", request.cmd, 3)) {
        // read the message from client and copy it in buffer
        read(sockfd, &request, sizeof(request));
        // print buffer which contains the client contents
        printf("From client: %s\t To client : ", request.cmd);
//        write(sockfd, buff, sizeof(buff));
    }
    printf("TCP Server Exit...\n");
}


void *start_tcp_server(void *thread_data) {
    tcp_server_data_t *server_data = thread_data;
    int sockfd, connfd, len;
    struct sockaddr_in servaddr, cli;

    // socket create and verification
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        printf("TCP socket creation failed...\n");
        exit(0);
    }
    else
        printf("TCP Socket successfully created..\n");
    bzero(&servaddr, sizeof(servaddr));

    // assign IP, PORT
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(server_data->arg.port);

    // Binding newly created socket to given IP and verification
    if ((bind(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr))) != 0) {
        printf("TCP socket bind failed...\n");
        exit(0);
    }
    else
        printf("TCP Socket successfully binded..\n");

    // Now server is ready to listen and verification
    if ((listen(sockfd, 5)) != 0) {
        printf("TCP Listen failed...\n");
        exit(0);
    }
    else
        printf("TCP Server listening..\n");
    len = sizeof(cli);

    // Accept the data packet from client and verification
    connfd = accept(sockfd, (struct sockaddr*)&cli, &len);
    if (connfd < 0) {
        printf("TCP server accept failed...\n");
        exit(0);
    }
    else
        printf("TCP server accept the client...\n");

    // Function for chatting between client and server
    func(connfd);

    // After chatting close the socket
    close(sockfd);
    return NULL;
}