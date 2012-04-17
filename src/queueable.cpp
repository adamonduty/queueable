#include "queueable.h"

#include <map>
#include <stdio.h>
#include <stdlib.h>
#include <sstream>
#include <string>
#include <string.h>
#include <stdint.h>
#include <sys/time.h>
#include <vector>

// Run a series of tests with this backend
void Queueable::run_tests()
{
    // Server distributes work
    if (options["mode"].compare("server") == 0)
    {
        for (int i = 1; i <= 4096; i*=2)
        {
            std::string msg = "enqueue a million items of size ";
            std::stringstream ss;
            ss << i;
            msg.append(ss.str());

            start_test(msg);
            test_enqueue(1000000, i);
            stop_test();
        }
    }

    // Client consumes work
    else if (options["mode"].compare("client") == 0)
    {
        for (int i = 1; i <= 4096; i*=2)
        {
            std::string msg = "dequeue a million items of size ";
            std::stringstream ss;
            ss << i;
            msg.append(ss.str());

            start_test(msg);
            test_dequeue(1000000);
            stop_test();
        }
    }
    else
      printf("Unable to run tests\n");
}

void Queueable::start_test(std::string msg)
{
    test_message = msg;
    gettimeofday(&start_tv, NULL);
}

void Queueable::stop_test()
{
    struct timeval diff;
    gettimeofday(&stop_tv, NULL);

    timersub(&stop_tv, &start_tv, &diff);

    printf("%s: %ld.%06ld\n", test_message.c_str(), diff.tv_sec, diff.tv_usec);
}

void Queueable::test_enqueue(int num_items, uint32_t msg_size)
{
    std::vector<std::string> items;
    char * buffer = NULL;
    items.reserve(1000);

    buffer = (char *) malloc(msg_size);

    // Build 1000 item vector with desired message size
    for (int i = 0; i < 1000; ++i)
    {
        std::string str;
        str.reserve(msg_size);
        str.append(buffer, msg_size);
        items.push_back(str);
    }
    free(buffer);

    for (int i = 0; i < num_items/1000; ++i)
    {
        enqueue(&items);
    }
}

void Queueable::test_dequeue(int num_items)
{
    std::vector<std::string> items;

    for (int i = 0; i < num_items/1000; ++i)
    {
        dequeue(&items, 1000);
        items.clear();
    }
}
