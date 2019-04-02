#pragma once
#include <netinet/in.h>

struct link{
public:
    int port;
    int weight;
    double lifetime; // For timing out
    bool active;
    struct sockaddr_in address;
};