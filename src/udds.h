#ifndef _UDDS_H
#define _UDDS_H

#include <string>
#include <sys/un.h>
#include <vector>

#include "socket.h"

class Udds : public Socket {
    public:
        const char * backend_name();
        void after_fork();
        void send_alive();
        void cleanup();
        void enqueue(std::vector<std::string> * items);
        void dequeue(std::vector<std::string> * items);

        int socket_family();
        int socket_type();
        void setup_bind();
        void setup_client_bind();
        void setup_client_connect();

        void perform_listen();

        struct sockaddr_un sock_un;
        struct sockaddr_un client_sock_un;

        // parent
        std::vector<struct sockaddr_un> sockaddrs;
        int connection_count;

        // child
        std::string client_path;
};

#endif
