#include "udds.h"

#include <netinet/in.h>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>

const char * Udds::backend_name()
{
    return "Unix Domain Datagram Socket";
}

void Udds::after_fork()
{
    struct sockaddr_un from;
    socklen_t from_len = sizeof(from);
    int count = 0;
    char buffer[512];

    // Wait for 'alive' message from each client
    if (parent == true)
    {
        for (int i = 0; i < threads; ++i)
        {
            bzero(&from, from_len);
            from_len = sizeof(from);
            count = recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr *) &from, &from_len);

            D fprintf(stderr, "Received alive packet from %s\n", from.sun_path);
            if (sendto(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr *) &from, sizeof(struct sockaddr_un)) < 0)
            {
                perror("sendto in after_fork");
                exit(1);
            }

            sockaddrs.push_back(from);
        }
    }
    else if (child == true)
    {
        close(sockfd);
        setup_client();
        send_alive();
    }

    connection_count = 0;
}

// Tell the server we're alive
void Udds::send_alive()
{
    int count = 0;

    count = sendto(sockfd, "1", 1, 0, server_addr, server_addr_size);
    if (count < 0)
        perror("sendto on alive packet");
}

void Udds::cleanup()
{
    std::vector<struct sockaddr_un>::iterator it;
    if (parent == true)
    {
        for (it = sockaddrs.begin(); it != sockaddrs.end(); ++it)
        {
            D fprintf(stderr, "cleanup signal to %s\n", it->sun_path);
            if (sendto(sockfd, "", 0, 0, (struct sockaddr *) &(*it), sizeof(struct sockaddr_un)) < 0)
                perror("sendto in cleanup");
        }

        sockaddrs.clear();
    }

    close(sockfd);
    if (parent == true)
        unlink(options["path"].c_str());

    if (child == true)
        unlink(client_path.c_str());
}

void Udds::enqueue(std::vector<std::string> * items)
{
    std::vector<std::string>::iterator it;
    struct sockaddr_un addr;
    socklen_t addr_len = sizeof(struct sockaddr_un);
    int count = 0;

    // Select a connection, balance round-robin
    addr = (sockaddrs.at(connection_count % sockaddrs.size()));

    for (it = items->begin(); it != items->end(); ++it)
    {
        count = sendto(sockfd, it->c_str(), it->size(), 0, (struct sockaddr *) &addr, addr_len);

        if (count < 0)
            perror("sendto");
    }
}

void Udds::dequeue(std::vector<std::string> * items)
{
    char * buffer = NULL;
    uint32_t buffer_size = 4096;
    bool done = false;
    std::string item;
    int max_items = 1000;
    int count = 0;
    struct sockaddr_un from;
    socklen_t from_len = sizeof(from);

    buffer = (char *) malloc(buffer_size);

    while (done == false && (int) items->size() < max_items)
    {
        count = recvfrom(sockfd, buffer, buffer_size, 0, (struct sockaddr *) &from, &from_len);
        if (count < 0)
        {
            perror("recvfrom");
        }
        else if (count == 0)
        {
            done = true;
            items->clear();
        }
        else
        {
            item.clear();
            item.reserve(buffer_size);
            item.append(buffer, count);
            if (item.size() > 0)
                items->push_back(item);
        }
    }

    free(buffer);
}

int Udds::socket_family()
{
    return AF_UNIX;
}

int Udds::socket_type()
{
    return SOCK_DGRAM;
}

void Udds::setup_bind()
{
    int length = 0;

    // Remove old path if neccessary
    unlink(options["path"].c_str());

    bzero(&sock_un, sizeof(sock_un));
    sock_un.sun_family = socket_family();

    // Copy string from options into sock_un, respecting maximum length
    length = options["path"].size();
    if (length > 104)
        length = 104;

    memcpy(sock_un.sun_path, options["path"].c_str(), length);

    server_addr = (struct sockaddr *) &sock_un;
    server_addr_size = sizeof(sock_un);
}

void Udds::setup_client_bind()
{
    int length = 0;
    client_path = options["client_path"];

    // Append thread id to path to make it unique among other clients
    std::stringstream ss("");
    ss << thread_id;
    client_path.append(ss.str());

    // Remove old path if neccessary
    unlink(client_path.c_str());

    bzero(&client_sock_un, sizeof(client_sock_un));
    client_sock_un.sun_family = socket_family();

    // Copy string from options into sock_un, respecting maximum length
    length = client_path.size();
    if (length > 104)
        length = 104;

    memcpy(client_sock_un.sun_path, client_path.c_str(), length);

    client_addr = (struct sockaddr *) &client_sock_un;
    client_addr_size = sizeof(client_sock_un);
}

void Udds::setup_client_connect()
{
    client_addr = (struct sockaddr *) &sock_un;
    client_addr_size = sizeof(sock_un);
    //int length = 0;

    //// Copy string from options into sock_un, respecting maximum length
    //length = options["path"].size();
    //if (length > 104)
    //    length = 104;

    //memcpy(client_sock_un.sun_path, options["path"].c_str(), length);
}

// Not supported on datagram sockets
void Udds::perform_listen()
{
}
