#pragma once
#include <string>
#include <map>

class HostObj{
public:
    HostObj(std::string);

    void addLink(std::string, int, int);
    void printLinks();
    std::map< std::string, std::pair<int, int> > getLinks();

    void printTable();
    void updateTable(std::string, std::string, int);
    void reGenTable();

    std::string getHostname();
    int getWeight(std::string);
    int getsendPort(std::string);
    int getreceivePort();
private:
    std::map< std::string, std::pair<int, int> > links;
    std::string hostname;
    int receiveport;
    std::map< std::string, std::map< std::string, int > > fwdtable;
    //int fwdtable[6][6];
};
