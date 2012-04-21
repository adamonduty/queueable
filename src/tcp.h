#ifndef _TCP_H
#define _TCP_H

#include <arpa/inet.h>
#include <map>
#include <string>
#include <vector>

#include "stream_socket.h"

class Tcp: public StreamSocket {
    protected:
        const char * backend_name();
        int socket_family();
        void setup_bind();
        void setup_client_bind();
        void setup_client_connect();

        struct sockaddr_in sock_in;
        struct sockaddr_in client_sock_in;
};

#endif
