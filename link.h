#pragma once
#include <netinet/in.h>
#include <string>

// THE LINK STRUCT CONTAINS ALL INFORMATION RELEVANT TO A SPECIFIC LINK

struct link{
public:
    int port;                       // The link's outgoing port
    int weight;                     // The weight associated with the link
    bool active;                    // Is the link in use currently?
    double lifetime;                // Timeout, countdown until deactivation
    struct sockaddr_in address;     // The socket address associated with the link
};
