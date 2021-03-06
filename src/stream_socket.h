#ifndef _STREAM_SOCKET_H
#define _STREAM_SOCKET_H

#include <map>
#include <string>
#include <vector>

#include "socket.h"

class StreamSocket: public Socket {
    public:
        void after_fork();
        void cleanup();
        void enqueue(std::vector<std::string> * items);
        void dequeue(std::vector<std::string> * items);

    protected:
        virtual int socket_family() = 0;
        int socket_type();
        virtual void setup_bind() = 0;
        virtual void setup_client_bind();
        virtual void setup_client_connect() = 0;

        virtual void perform_client_bind();

        // parent
        std::vector<int> conn_fds;

        // child
        int connection_count;
};

#endif

