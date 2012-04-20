#ifndef _QUEUEABLE_H
#define _QUEUEABLE_H

#include <list>
#include <map>
#include <string>
#include <sys/time.h>
#include <stdint.h>
#include <vector>

class Queueable {
    public:
        virtual void before_fork() = 0;
        virtual void after_fork() = 0;
        virtual void cleanup() = 0;
        virtual void enqueue(std::vector<std::string> * items) = 0;
        virtual void dequeue(std::vector<std::string> * items) = 0;

        //virtual bool start(std::map<std::string, std::string> options) = 0;
        //virtual void stop() = 0;

        void run_tests(std::map<std::string, std::string> options);
        void perform_fork();
        void start_test(std::string msg);
        void stop_test();
        void test_enqueue(int num_items, uint32_t msg_size);
        void test_dequeue();
        void perform_wait();

        // Convenience methods for attributes
        int get_clients();
        int get_items();
        int get_port();


    protected:
        std::map<std::string, std::string> options;
        struct timeval start_tv;
        struct timeval stop_tv;
        std::string test_message;

        bool child;
        bool parent;
        std::list<int> children;
};

#endif
