#include "tcp.h"

#include <strings.h>

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
