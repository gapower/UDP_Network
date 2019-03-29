#pragma once
#include <netinet/in.h>

struct link{
public:
    int port;
    int weight;
    bool active;
    struct sockaddr_in address;
};