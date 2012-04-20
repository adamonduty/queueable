#include "stream_socket.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <vector>

void StreamSocket::after_fork()
{
    int connfd = 0;
    int threads = get_threads();

    // accept all children
    if (parent == true)
    {
        for (int i = 0; i < threads; ++i)
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

void StreamSocket::cleanup()
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

    // For child, close connection with parent
    // For parent, close accept fd
    close(sockfd);
}

void StreamSocket::enqueue(std::vector<std::string> * items)
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

void StreamSocket::dequeue(std::vector<std::string> * items)
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

int StreamSocket::socket_type()
{
    return SOCK_STREAM;
}
