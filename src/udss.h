#ifndef _UDSS_H
#define _UDSS_H

#include <sys/un.h>

#include "stream_socket.h"

class Udss: public StreamSocket {
    protected:
        const char * backend_name();
        int socket_family();
        void setup_bind();
        void setup_client_connect();

        struct sockaddr_un sock_un;
        struct sockaddr_un client_sock_un;
};

#endif
