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

bool Tcp::start(std::map<std::string, std::string> options)
{
    this->options = options;

    if (options["mode"].compare("server") == 0)
    {
        return setup_server();
    }
    else if (options["mode"].compare("client") == 0)
    {
        return setup_client();
    }

    return false;
}

bool Tcp::setup_server()
{
    struct sockaddr_in server_addr;
    int port = get_port();
    int acceptfd;
    int optval = 1;

    /* attempt to get socket */
    if ((acceptfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket fail");
        goto fail_socket;
    }

    /* set SO_REUSEADDR for quick restarts */
    setsockopt(acceptfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

    /* setup bind parameters */
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(port);

    /* attempt bind */
    if (bind(acceptfd, (struct sockaddr *) &server_addr, sizeof(server_addr))
        < 0) {
        perror("bind fail");
        goto fail_bind;
    }

    /* attempt listen */
    if (listen(acceptfd, 5) < 0) {
        perror("listen fail");
        goto fail_listen;
    }

    /* attempt accept */
    if ((sockfd = accept(acceptfd, NULL, NULL)) < 0) {
        perror("accept fail");
        goto fail_accept;
    }

    close(acceptfd);
    return true;

fail_accept:
fail_listen:
fail_bind:
    close(acceptfd);
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

int Tcp::get_port()
{
    std::stringstream ss(options["port"]);
    int port;
    ss >> port;

    return port;
}

void Tcp::enqueue(std::vector<std::string> * items)
{
    std::vector<std::string>::iterator it;

    for (it = items->begin(); it != items->end(); ++it)
    {
        write(sockfd, it->c_str(), it->size());
    }
}

void Tcp::dequeue(std::vector<std::string> * items, int num_items)
{
    char * buffer = NULL;
    uint32_t buffer_size = 4096;
    uint32_t str_size = 0;
    int count = 0;
    std::string item;

    // Initial buffer size at 4096
    buffer = (char *) malloc(buffer_size);

    // Read 4 bytes for string size
    while ((count = read(sockfd, buffer, 4)) > 0)
    {
        memcpy(&str_size, buffer, 4);
        if (str_size > buffer_size)
        {
            buffer_size = str_size;
            buffer = (char *) realloc(buffer, buffer_size);
        }

        // Read the rest of the message
        item.clear();
        item.reserve(str_size);

        while (str_size > 0 && (count = read(sockfd, buffer, str_size)) > 0)
        {
            str_size -= count;
            item.append(buffer, count);
        }
        items->push_back(item);

        if ((int) items->size() >= num_items)
            break;
    }

    free(buffer);
}

void Tcp::stop()
{
}
