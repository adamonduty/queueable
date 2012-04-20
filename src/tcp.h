#ifndef _TCP_H
#define _TCP_H

#include <arpa/inet.h>
#include <map>
#include <string>
#include <vector>

#include "stream_socket.h"

class Tcp: public StreamSocket {
    protected:
        int socket_family();
        void setup_bind();

        struct sockaddr_in sock_in;
};

#endif
