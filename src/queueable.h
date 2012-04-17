#ifndef _QUEUEABLE_H
#define _QUEUEABLE_H

#include <map>
#include <string>
#include <sys/time.h>
#include <stdint.h>
#include <vector>

class Queueable {
    public:
        virtual bool start(std::map<std::string, std::string> options) = 0;
        virtual void enqueue(std::vector<std::string> * items) = 0;
        virtual void dequeue(std::vector<std::string> * items, int num_items) = 0;
        virtual void stop() = 0;

        void run_tests();
        void start_test(std::string msg);
        void stop_test();
        void test_enqueue(int num_items, uint32_t msg_size);
        void test_dequeue(int num_items);

    protected:
        std::map<std::string, std::string> options;
        struct timeval start_tv;
        struct timeval stop_tv;
        std::string test_message;
};

#endif
