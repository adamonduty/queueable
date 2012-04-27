#include <map>
#include <stdio.h>
#include <string>
#include <sys/utsname.h>

#include "tcp.h"
#include "udss.h"
#include "udds.h"
#include "mq.h"

int main()
{
    struct utsname name;
    std::map<std::string, std::string> options;

    options["port"] = "10000";
    options["items"] = "1000000";
    options["max_msg_size"] = "4096";
    options["max_threads"] = "32";
    options["path"] = "/tmp/queueable-socket";
    options["client_path"] = "/tmp/queueable-socket-client";
    options["message_queue"] = "/queueable";

    uname(&name);

    printf("<?xml version=\"1.0\"?>\n");
    printf("<platform>\n");
    printf("<sysname>%s</sysname><release>%s</release><version>%s</version><machine>%s</machine>\n",
      name.sysname, name.release, name.version, name.machine);
    printf("<batches_attributes type=\"array\">\n");
    printf("<batch>\n");
    printf("<runs_attributes type=\"array\">\n");

    Tcp tcp;
    tcp.run_tests(options);

    Udss udss;
    udss.run_tests(options);

    Udds udds;
    udds.run_tests(options);

    Mq mq;
    mq.run_tests(options);

    printf("</runs_attributes>\n");
    printf("</batch>\n");
    printf("</batches_attributes>\n");
    printf("</platform>\n");

    return 0;
}
