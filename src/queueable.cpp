#include "queueable.h"

#include <list>
#include <map>
#include <stdio.h>
#include <stdlib.h>
#include <sstream>
#include <string>
#include <string.h>
#include <stdint.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <vector>

void Queueable::run_tests(std::map<std::string, std::string> options)
{
    this->options = options;
    int max_msg_size = get_max_msg_size();
    int max_threads = get_max_threads();

    for (int i = 1; i <= max_threads; i *= 2)
    {
        for (int j = 1; j <= max_msg_size; j *= 2)
        {
            threads = i;
            msg_size = j;
            run_test();
        }
    }
}

// Run a single test with this backend
void Queueable::run_test()
{

    // Set default options
    child = false;
    parent = false;
    children.clear();

    D fprintf(stderr, "before_fork()\n");
    before_fork();

    D fprintf(stderr, "perform_fork()\n");
    perform_fork();

    D fprintf(stderr, "after_fork(), parent = %d, child = %d\n", parent, child);
    after_fork();

    D fprintf(stderr, "finished after_fork(), parent = %d, child = %d\n", parent, child);

    // Start timer, run test, stop timer
    std::string msg = "enqueue ";
    msg.append(options["items"]);
    msg.append(" items of size ");
    std::stringstream ss("");
    ss << msg_size;
    msg.append(ss.str());

    start_test(msg);
    if (parent == true)
    {
        test_enqueue(get_items(), msg_size);
    }
    else if (child == true)
    {
        test_dequeue();
    }

    D fprintf(stderr, "cleanup, parent = %d, child = %d\n", parent, child);
    cleanup();
    perform_wait();
    stop_test();
}

void Queueable::perform_fork()
{
    for (int i = 0; i < threads; ++i)
    {
        child = false;
        parent = false;
        pid_t pid = fork();

        if (pid == 0) // child
        {
            child = true;
            thread_id = i;
            break;
        }
        else if (pid > 0) // parent
        {
            parent = true;
            children.push_back((int) pid);
        }
        else // error
        {
            parent = false;
            child = false;
            perror("fork failed");
        }
    }
}

void Queueable::start_test(std::string msg)
{
    test_message = msg;
    gettimeofday(&start_tv, NULL);
}

void Queueable::stop_test()
{
    gettimeofday(&stop_tv, NULL);

    timersub(&stop_tv, &start_tv, &duration);

    print_results();
}

void Queueable::print_results()
{
    printf("<test>");
    printf("<type>%s</type>", backend_name());
    printf("<threads>%d</threads>", threads);
    printf("<seconds>%ld.%06ld</seconds>", duration.tv_sec, duration.tv_usec);
    printf("<msg_size>%d</msg_size>", msg_size);
    printf("</test>\n");
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

void Queueable::test_dequeue()
{
    std::vector<std::string> items;

    do
    {
        items.clear();
        dequeue(&items);
    }
    while (items.size() > 0);
}

void Queueable::perform_wait()
{
    int status = 0;
    std::list<int>::iterator it;

    if (parent == true)
    {
        for (it = children.begin(); it != children.end(); ++it)
        {
            D fprintf(stderr, "waiting for pid %d\n", *it);
            if (waitpid(*it, &status, 0) < 0)
                perror("waitpid fail");
        }
    }
    else if (child == true)
    {
        exit(0);
    }
}

int Queueable::get_max_threads()
{
    std::stringstream ss(options["max_threads"]);
    int tmp;
    ss >> tmp;

    return tmp;
}

int Queueable::get_items()
{
    std::stringstream ss(options["items"]);
    int items;
    ss >> items;

    return items;
}

int Queueable::get_port()
{
    std::stringstream ss(options["port"]);
    int port;
    ss >> port;

    return port;
}

int Queueable::get_max_msg_size()
{
    std::stringstream ss(options["max_msg_size"]);
    int size;
    ss >> size;

    return size;
}
