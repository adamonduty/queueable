#include "tcp.h"

#include <arpa/inet.h>
#include <strings.h>

const char * Tcp::backend_name()
{
    return "TCP Socket";
}

int Tcp::socket_family()
{
    return AF_INET;
}

void Tcp::setup_bind()
{
    /* setup bind parameters */
    bzero(&sock_in, sizeof(sock_in));
    sock_in.sin_family = socket_family();
    sock_in.sin_addr.s_addr = htonl(INADDR_ANY);
    sock_in.sin_port = htons(get_port());

    server_addr = (struct sockaddr *) &sock_in;
    server_addr_size = sizeof(sock_in);
}

void Tcp::setup_client_connect()
{
    struct in_addr addr;
    inet_pton(socket_family(), "127.0.0.1", &addr);

    bzero(&client_sock_in, sizeof(client_sock_in));
    client_sock_in.sin_family = socket_family();
    client_sock_in.sin_addr = addr;
    client_sock_in.sin_port = htons(get_port());

    client_addr = (struct sockaddr *) &client_sock_in;
    client_addr_size = sizeof(client_sock_in);
}
