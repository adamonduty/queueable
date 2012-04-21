#ifndef _QUEUEABLE_H
#define _QUEUEABLE_H

#include <list>
#include <map>
#include <string>
#include <sys/time.h>
#include <stdint.h>
#include <vector>

#ifdef DEBUG
  #define D if (1)
#else
  #define D if (0)
#endif

class Queueable {
    public:
        virtual const char * backend_name() = 0;
        virtual void before_fork() = 0;
        virtual void after_fork() = 0;
        virtual void cleanup() = 0;
        virtual void enqueue(std::vector<std::string> * items) = 0;
        virtual void dequeue(std::vector<std::string> * items) = 0;

        void run_tests(std::map<std::string, std::string> options);
        void run_test();
        void perform_fork();
        void start_test(std::string msg);
        void stop_test();
        void print_results();
        void test_enqueue(int num_items, uint32_t msg_size);
        void test_dequeue();
        void perform_wait();

        // Convenience methods for attributes
        int get_max_threads();
        int get_items();
        int get_port();
        int get_max_msg_size();

    protected:
        std::map<std::string, std::string> options;
        struct timeval start_tv;
        struct timeval stop_tv;
        struct timeval duration;
        std::string test_message;

        bool child;
        bool parent;
        std::list<int> children;

        int msg_size;
        int threads;
        int thread_id;
};

#endif
