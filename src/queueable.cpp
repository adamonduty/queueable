#include "queueable.h"

#include <map>
#include <stdio.h>
#include <string>
#include <vector>

// Run a series of tests with this backend
void Queueable::run_tests()
{
    // Server distributes work
    if (options["mode"].compare("server") == 0)
    {
        for (int i = 1; i <= 4096; i*=2)
        {
            test_enqueue(10000, i);
            printf("enqueued 10000 items of size %d\n", i);
        }
    }

    // Client consumes work
    else if (options["mode"].compare("client") == 0)
    {
        for (int i = 1; i <= 4096; i*=2)
        {
            test_dequeue(10000);
            printf("dequeued 10000 items of size %d\n", i);
        }
    }
    else
      printf("Unable to run tests\n");
}

void Queueable::test_enqueue(int num_items, int msg_size)
{
    std::vector<std::string> items;
    items.reserve(1000);

    // Build 1000 item vector with desired message size
    for (int i = 0; i < 1000; ++i)
    {
        std::string str;
        str.reserve(msg_size);
        for (int j = 0; j < msg_size; ++j)
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
        dequeue(&items);
        items.clear();
    }
}
