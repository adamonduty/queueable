#include <map>
#include <stdio.h>
#include <string>
#include <vector>

#include "tcp.h"

int main()
{
    Tcp tcp;
    std::map<std::string, std::string> options;

    options["port"] = "10000";
    options["items"] = "1000000";
    options["clients"] = "32";
    tcp.run_tests(options);

    return 0;
}
