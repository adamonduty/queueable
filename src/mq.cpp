#ifndef __APPLE__

#include <fcntl.h>
#include <mqueue.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <sys/stat.h>
#include <vector>

#include "mq.h"

void Mq::before_fork()
{
    struct mq_attr attr;
    mq_name = options["message_queue"];

    mq_unlink(mq_name.c_str());
    queuefd = mq_open(mq_name.c_str(), O_RDWR | O_CREAT, S_IRUSR | S_IWUSR, NULL);
    if (queuefd < 0)
        perror("mq_open");

    if (mq_getattr(queuefd, &attr) < 0)
        perror("mq_getattr");

    max_msg_size = attr.mq_msgsize;
}

void Mq::after_fork()
{
}

void Mq::cleanup()
{
    // Send finished message to client
    if (parent == true)
    {
        for (int i = 0; i < threads; ++i)
        {
            if (mq_send(queuefd, "", 0, 0) < 0)
                perror("finished message");
        }

        if (mq_unlink(mq_name.c_str()) < 0)
            perror("mq_unlink");
    }

    if (mq_close(queuefd) < 0)
        perror("mq_close");
}

void Mq::enqueue(std::vector<std::string> * items)
{
    std::vector<std::string>::iterator it;

    for (it = items->begin(); it != items->end(); ++it)
    {
        if (mq_send(queuefd, it->c_str(), it->size(), 0) < 0)
            perror("mq_send");
    }
}

void Mq::dequeue(std::vector<std::string> * items)
{
    bool done = false;
    char * buffer = NULL;
    int count = 0;
    int max_items = 1000;
    std::string item;

    buffer = (char *) malloc(max_msg_size);
    item.reserve(max_msg_size);

    while (done == false && (int) items->size() < max_items)
    {
        count = mq_receive(queuefd, buffer, max_msg_size, 0);
        if (count < 0)
            perror("mq_receive");
        else if (count == 0)
        {
            done = true;
            items->clear();
        }
        else
        {
            item.assign(buffer, count);
            items->push_back(item);
        }
    }

    free(buffer);
}
#endif
