#ifndef _TCP_H
#define _TCP_H

#include <map>
#include <string>
#include <vector>

#include "queueable.h"

class Tcp: public Queueable {
    public:
        bool start(std::map<std::string, std::string> options);
        void enqueue(std::vector<std::string> * items);
        void dequeue(std::vector<std::string> * items);
        void stop();

    protected:
        bool setup_server();
        bool setup_client();
        int get_port();

    private:
        int sockfd;
};

#endif
