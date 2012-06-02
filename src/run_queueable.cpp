#define HTTP_HOST "queueable.herokuapp.com"

#include <fstream>
#include <map>
#include <stdio.h>
#include <sstream>
#include <string>
#include <sys/time.h>
#include <sys/utsname.h>

#include "tcp.h"
#include "udss.h"
#include "udds.h"

// no message queues on OS X
#ifndef __APPLE__
#include "mq.h"
#endif

#include "md5.h"

std::string unique_batch(struct utsname name)
{
    std::stringstream seed;
    struct timeval tv;

    gettimeofday(&tv, NULL);
    seed << tv.tv_sec;
    seed << tv.tv_usec;
    seed << getpid();
    seed << getppid();
    seed << name.sysname;
    seed << name.release;
    seed << name.version;
    seed << name.machine;

    return md5(seed.str());
}

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

    // Gather basic system info
    uname(&name);
    std::string uhash = unique_batch(name);
    std::string filename = uhash.substr(0, 8);
    filename.append(".xml");

    // Open output file for writing
    std::ofstream output;
    output.open(filename.c_str());

    std::stringstream results;
    results << "<?xml version=\"1.0\"?>\n";
    results << "<platform>\n";
    results << "<sysname>" << name.sysname << "</sysname><release>";
    results << name.release << "</release><version>" << name.version;
    results << "</version><machine>" << name.machine << "</machine>\n";

    results << "<batches_attributes type=\"array\">\n";
    results << "<batch>\n";
    results << "<uhash>" << uhash << "</uhash>\n";
    results << "<runs_attributes type=\"array\">\n";

    Tcp tcp;
    tcp.run_tests(&results, options);

    Udss udss;
    udss.run_tests(&results, options);

#ifdef __APPLE__
    // Message size of 4096 too large for OS X
    options["max_msg_size"] = "2048";
    // Failing on closing sendto() for 8 threads
    options["max_threads"] = "4";
#endif
    Udds udds;
    udds.run_tests(&results, options);

// no message queues on OS X
#ifndef __APPLE__
    Mq mq;
    mq.run_tests(&results, options);
#endif

    results << "</runs_attributes>\n";
    results << "</batch>\n";
    results << "</batches_attributes>\n";
    results << "</platform>\n";

    output << results.str();
    output.close();

    printf("*********************************************************************\n");
    printf("Tests complete. Submit results via curl:\n\n");
    printf("$ curl --header 'Content-type: application/xml' --header 'Accept: text/plain' --data @%s http://%s/platforms\n",
        filename.c_str(), HTTP_HOST);
    printf("*********************************************************************\n");

    return 0;
}
