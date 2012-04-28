#include "queueable.h"

#include <iomanip>
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

void Queueable::run_tests(std::stringstream *results, std::map<std::string, std::string> options)
{
    this->options = options;
    this->results = results;
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

    timeval_subtract(&duration, &stop_tv, &start_tv);

    print_results();
}

// timersub() is not portable, so here is a replacement
// http://www.delorie.com/gnu/docs/glibc/libc_428.html
int Queueable::timeval_subtract(struct timeval *result, struct timeval *x, struct timeval *y)
{
    /* Perform the carry for the later subtraction by updating y. */
    if (x->tv_usec < y->tv_usec) {
        int nsec = (y->tv_usec - x->tv_usec) / 1000000 + 1;
        y->tv_usec -= 1000000 * nsec;
        y->tv_sec += nsec;
    }
    if (x->tv_usec - y->tv_usec > 1000000) {
        int nsec = (x->tv_usec - y->tv_usec) / 1000000;
        y->tv_usec += 1000000 * nsec;
        y->tv_sec -= nsec;
    }

    /* Compute the time remaining to wait.
       tv_usec is certainly positive. */
    result->tv_sec = x->tv_sec - y->tv_sec;
    result->tv_usec = x->tv_usec - y->tv_usec;

    /* Return 1 if result is negative. */
    return x->tv_sec < y->tv_sec;
}

void Queueable::print_results()
{
    *results << "<run>";
    *results << "<backend>" << backend_name() << "</backend>";
    *results << "<threads>" << threads << "</threads>";
    *results << "<seconds>" << (long) duration.tv_sec << ".";
    *results << std::setfill('0') << std::setw(6) << (long) duration.tv_usec << "</seconds>";
    *results << "<msg_size>" << msg_size << "</msg_size>";
    *results << "</run>\n";

    printf("%s: threads=%d, msg_size=%d, seconds=%ld.%06ld\n", backend_name(),
        threads, msg_size, (long) duration.tv_sec, (long) duration.tv_usec);
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
