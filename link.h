#pragma once
#include <netinet/in.h>
#include <string>

struct link{
public:
    int port;
    int weight;
    double lifetime; // For timing out
    bool active;
    struct sockaddr_in address;
};
