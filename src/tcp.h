#ifndef _TCP_H
#define _TCP_H

#include <map>
#include <string>
#include <vector>

#include "queueable.h"

class Tcp: public Queueable {
    public:
        void before_fork();
        void after_fork();
        void cleanup();
        void enqueue(std::vector<std::string> * items);
        void dequeue(std::vector<std::string> * items);

    protected:
        bool setup_server();
        bool setup_client();

    private:
        // parent
        std::vector<int> conn_fds;

        // parent and child
        int sockfd;

        // child
        int connection_count;
};

#endif
