#pragma once
#include <string>
#include <map>
#include "link.h"

class HostObj{
public:
    HostObj(std::string);

    void addLink(std::string, int, int);
    void printLinks();
    std::map< std::string, struct link > getLinks();

    void printTable();
    void updateTable(std::string, std::string, int);
    void regenTable();
    void activateNeighbour(std::string);
    void deleteNeighbour(std::string);

    std::string getHostname();
    int getWeight(std::string);
    int getsendPort(std::string);
    int getreceivePort();

    std::string getDistanceVector(std::string);

    struct link* findSendLink(std::string);
private:
    std::map< std::string, struct link > links;
    std::string hostname;
    int receiveport;
    std::map< std::string, std::map< std::string, int > > fwdtable;
    //int fwdtable[6][6];
};
