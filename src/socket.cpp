#include "socket.h"

#include <arpa/inet.h>
#include <stdio.h>
#include <sstream>
#include <string.h>
#include <sys/types.h>

void Socket::before_fork()
{
    setup_server();
}

void Socket::perform_socket()
{
    int optval = 1;

    /* attempt to get socket */
    if ((sockfd = socket(socket_family(), socket_type(), 0)) < 0)
        perror("socket fail");

    /* set SO_REUSEADDR for quick restarts */
    if (socket_family() == AF_INET || socket_family() == AF_INET6)
        setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
}

void Socket::perform_bind()
{
    /* attempt bind */
    if (bind(sockfd, server_addr, server_addr_size) < 0)
        perror("bind fail");
}

void Socket::perform_listen()
{
    /* attempt listen */
    if (listen(sockfd, get_clients()) < 0)
        perror("listen fail");
}

bool Socket::setup_server()
{
    perform_socket();
    setup_bind();
    perform_bind();
    perform_listen();
    return true;
}

bool Socket::setup_client()
{
    struct sockaddr_in server_addr;
    int port = get_port();
    struct in_addr addr;

    inet_pton(AF_INET, "127.0.0.1", &addr);

    /* Setup sockaddr_in */
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr = addr;
    server_addr.sin_port = htons(port);

    /* attempt to get socket */
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket fail");
        return false;
    }

    /* attempt connect */
    if (connect(sockfd, (struct sockaddr *) &server_addr, sizeof(server_addr)) != 0) {
        perror("connect fail");
        return false;
    }

    return true;
}
