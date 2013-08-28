#include "udss.h"

#include <netinet/in.h>
#include <strings.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>

const char * Udss::backend_name()
{
    return "Unix Domain Stream Socket";
}

int Udss::socket_family()
{
    return AF_UNIX;
}

void Udss::setup_bind()
{
    int length = 0;

    // Remove old path if neccessary
    unlink(options["path"].c_str());

    bzero(&sock_un, sizeof(sock_un));
    sock_un.sun_family = socket_family();

    // Copy string from options into sock_un, respecting maximum length
    length = options["path"].size();
    if (length > 104)
        length = 104;

    memcpy(sock_un.sun_path, options["path"].c_str(), length);

    server_addr = (struct sockaddr *) &sock_un;
    server_addr_size = sizeof(sock_un);
}

void Udss::setup_client_connect()
{
    int length = 0;

    bzero(&client_sock_un, sizeof(client_sock_un));
    client_sock_un.sun_family = socket_family();

    // Copy string from options into sock_un, respecting maximum length
    length = options["path"].size();
    if (length > 104)
        length = 104;

    memcpy(client_sock_un.sun_path, options["path"].c_str(), length);

    client_addr = (struct sockaddr *) &client_sock_un;
    client_addr_size = sizeof(client_sock_un);
}
