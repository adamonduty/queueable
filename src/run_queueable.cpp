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
    options["clients"] = "16";
    options["path"] = "/tmp/queueable-socket";

printf("start tcp tests ******************\n");
    run_tcp();
printf("start udss tests ******************\n");
    run_udss();

    return 0;
}
