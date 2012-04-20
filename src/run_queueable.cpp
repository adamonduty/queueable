#include <map>
#include <stdio.h>
#include <string>
#include <vector>

#include "tcp.h"
#include "udss.h"

std::map<std::string, std::string> options;

void run_tcp()
{
    Tcp tcp;
    tcp.run_tests(options);
}

void run_udss()
{
    Udss udss;
    udss.run_tests(options);
}

int main()
{
    options["port"] = "10000";
    options["items"] = "1000000";
    options["max_msg_size"] = "4096";
    options["max_threads"] = "32";
    options["path"] = "/tmp/queueable-socket";

    printf("<tests>\n");
    run_tcp();
    run_udss();
    printf("</tests>\n");

    return 0;
}
