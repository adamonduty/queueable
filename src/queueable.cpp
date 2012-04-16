#include "queueable.h"

#include <map>
#include <stdio.h>
#include <string>
#include <string.h>
#include <stdint.h>
#include <vector>

// Run a series of tests with this backend
void Queueable::run_tests()
{
    // Server distributes work
    if (options["mode"].compare("server") == 0)
    {
        for (int i = 1; i <= 4096; i*=2)
        {
            test_enqueue(1000000, i);
            printf("enqueued 10000 items of size %d\n", i);
        }
    }

    // Client consumes work
    else if (options["mode"].compare("client") == 0)
    {
        for (int i = 1; i <= 4096; i*=2)
        {
            test_dequeue(1000000);
            printf("dequeued 10000 items of size %d\n", i);
        }
    }
    else
      printf("Unable to run tests\n");
}

void Queueable::test_enqueue(int num_items, uint32_t msg_size)
{
    std::vector<std::string> items;
    char msg_size_buffer[4];

    items.reserve(1000);
    msg_size += 4; // room for pascal string
    memcpy(msg_size_buffer, &msg_size, 4);

    // Build 1000 item vector with desired message size
    for (int i = 0; i < 1000; ++i)
    {
        std::string str;
        str.reserve(msg_size);
        str.append(msg_size_buffer, 4);
        for (uint32_t j = 0; j < msg_size; ++j)
            str.append("a");

        items.push_back(str);
    }

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
