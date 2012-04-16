#include "tcp.h"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <map>
#include <sstream>
#include <stdio.h>
#include <sstream>
#include <string>
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

    /* attempt to get socket */
    if ((acceptfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket fail");
        goto fail_socket;
    }

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
}

void Tcp::dequeue(std::vector<std::string> * items)
{
}

void Tcp::stop()
{
}
