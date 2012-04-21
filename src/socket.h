#ifndef _SOCKET_H
#define _SOCKET_H

#include <map>
#include <string>
#include <vector>

#include "queueable.h"

class Socket: public Queueable {
    public:
        virtual void before_fork();
        virtual void after_fork() = 0;
        virtual void cleanup() = 0;
        virtual void enqueue(std::vector<std::string> * items) = 0;
        virtual void dequeue(std::vector<std::string> * items) = 0;

    protected:
        virtual bool setup_server();
        virtual bool setup_client();

        virtual void perform_socket();
        virtual void perform_bind();
        virtual void perform_listen();

        virtual void perform_client_socket();
        virtual void perform_client_bind();
        virtual void perform_client_connect();

        virtual int socket_family() = 0;
        virtual int socket_type() = 0;
        virtual void setup_bind() = 0;
        virtual void setup_client_bind() = 0;
        virtual void setup_client_connect() = 0;

        // parent
        struct sockaddr * server_addr;
        int server_addr_size;

        // parent and child
        int sockfd;

        // child
        struct sockaddr * client_addr;
        int client_addr_size;
};

#endif
