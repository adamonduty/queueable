#include <map>
#include <stdio.h>
#include <string>
#include <vector>

#include "tcp.h"

int main()
{
    Tcp tcp;
    std::map<std::string, std::string> options;
    std::vector<std::string> items;

    options["mode"] = "client";
    options["port"] = "10000";

    if (tcp.start(options) == true)
    {
        printf("connected\n");
        tcp.run_tests();
        tcp.stop();
    }

    return 0;
}
