#ifndef _MQ_H
#define _MQ_H

#include <mqueue.h>
#include <string>
#include <vector>

#include "queueable.h"

class Mq: public Queueable {
    public:
        const char * backend_name() { return "POSIX Message Queues"; }
        void before_fork();
        void after_fork();
        void cleanup();
        void enqueue(std::vector<std::string> * items);
        void dequeue(std::vector<std::string> * items);

    private:
        mqd_t queuefd;
        long max_msg_size;
        std::string mq_name;
};

#endif
