#include "tcp.h"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <map>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <sstream>
#include <string>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <vector>

void Tcp::before_fork()
{
    this->options = options;
    setup_server();
}

void Tcp::after_fork()
{
    int connfd = 0;
    int clients = get_clients();

    // accept all children
    if (parent == true)
    {
        for (int i = 0; i < clients; ++i)
        {
            /* attempt accept */
            if ((connfd = accept(sockfd, NULL, NULL)) < 0) {
                perror("accept fail");
                continue;
            }

            conn_fds.push_back(connfd);
        }
    }
    else if (child == true)
    {
        close(sockfd);
        setup_client();
    }
}

bool Tcp::setup_server()
{
    struct sockaddr_in server_addr;
    int port = get_port();
    int optval = 1;

    /* attempt to get socket */
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket fail");
        goto fail_socket;
    }

    /* set SO_REUSEADDR for quick restarts */
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

    /* setup bind parameters */
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(port);

    /* attempt bind */
    if (bind(sockfd, (struct sockaddr *) &server_addr, sizeof(server_addr))
        < 0) {
        perror("bind fail");
        goto fail_bind;
    }

    /* attempt listen */
    if (listen(sockfd, get_clients()) < 0) {
        perror("listen fail");
        goto fail_listen;
    }

    return true;

fail_listen:
fail_bind:
    close(sockfd);
fail_socket:
    sockfd = -1;
    return false;
}

bool Tcp::setup_client()
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

void Tcp::enqueue(std::vector<std::string> * items)
{
    std::vector<std::string>::iterator it;
    char msg_size_buffer[4];
    uint32_t msg_size = 0;
    int count = 0;
    int offset = 0;
    int connfd = 0;

    // Select a connection, balanced round-robin
    connfd = conn_fds.at(connection_count % conn_fds.size());
    ++connection_count;

    for (it = items->begin(); it != items->end(); ++it)
    {
        msg_size = (uint32_t) it->size();
        memcpy(msg_size_buffer, &msg_size, 4);

        offset = 0;
        while (sizeof(msg_size) - offset > 0)
        {
            count = write(connfd, msg_size_buffer+offset, sizeof(msg_size) - offset);
            offset += count;

            if (count < 0)
                perror("write fail");
        }

        offset = 0;
        while (it->size() - offset > 0)
        {
            count = write(connfd, it->c_str() + offset, it->size() - offset);
            offset += count;

            if (count < 0)
                perror("write fail");
        }
    }
}

void Tcp::dequeue(std::vector<std::string> * items)
{
    char * buffer = NULL;
    uint32_t buffer_size = 4096;
    uint32_t str_size = 0;
    int count = 0;
    int offset = 0;
    bool done = false;
    std::string item;
    int max_items = 1000;

    // Initial buffer size at 4096
    buffer = (char *) malloc(buffer_size);

    // Read 4 bytes for string size
    while (done == false && (int) items->size() < max_items)
    {
        offset = 0;
        while (done == false && sizeof(str_size) - offset > 0)
        {
            count = read(sockfd, buffer + offset, sizeof(str_size) - offset);
            offset += count;
            if (count == 0)
                done = true;
        }

        memcpy(&str_size, buffer, 4);
        if (str_size > buffer_size)
        {
            buffer_size = str_size;
            buffer = (char *) realloc(buffer, buffer_size);
        }

        // Read the rest of the message
        offset = 0;
        item.clear();
        item.reserve(str_size);

        while (done == false && str_size - offset > 0)
        {
            count = read(sockfd, buffer + offset, str_size - offset);
            offset += count;
            item.append(buffer, count);

            if (count == 0)
                done = true;

        }
        if (item.size() > 0)
            items->push_back(item);
    }

    free(buffer);
}

void Tcp::cleanup()
{
    std::vector<int>::iterator it;

    // Close all child connections
    if (parent == true)
    {
        for (it = conn_fds.begin(); it != conn_fds.end(); ++it)
        {
            close(*it);
        }
    }
    // Close connection with server
    else if (child == true)
    {
        close(sockfd);
    }
}
