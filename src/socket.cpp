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
    perform_client_socket();
    setup_client_connect();
    perform_client_connect();
}

void Socket::perform_client_socket()
{
    /* attempt to get socket */
    if ((sockfd = socket(socket_family(), socket_type(), 0)) < 0) {
        perror("socket fail");
    }
}

void Socket::perform_client_connect()
{
    /* attempt connect */
    if (connect(sockfd, client_addr, client_addr_size) != 0) {
        perror("connect fail");
    }
}
