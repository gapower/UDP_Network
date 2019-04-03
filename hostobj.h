#pragma once
#include <string>
#include <map>
#include "link.h"

// THE HOSTOBJ CLASS
// This class contains all of the information about the host
// It's links, distance vectors, etc
// Contains function to calculate and access this information

class HostObj{
public:
    // Constructor, pass hostname to create a new host
    HostObj(std::string);

    // Add a neighbour/link
    void addLink(std::string, int, int);
    // List all of the current links
    void printLinks();
    // Retrieve all of the current links
    // Note returns pointer, allows maniulation of link's data by main
    std::map< std::string, struct link >* getLinks();

    // Print the table of distance vectors
    void printTable();
    // Update a distance value in this table, e.g. AB4 -> AB3
    void updateTable(std::string, std::string, int);
    // Calculate the host's distance vector
    void regenTable();
    // Mark a node as active
    void activateNeighbour(std::string);
    // Mark a node as inactive
    void deleteNeighbour(std::string);

    // Getter methods, simply retrieves attributes
    std::string getHostname();
    int getWeight(std::string);
    int getsendPort(std::string);
    int getreceivePort();
    std::string getDistanceVector(std::string);

    // Delete a distance vector stored in memory, to replace with new information
    void clearRow(std::string);
    // Find the link which will carry a packet to a destination
    struct link* findSendLink(std::string);
private:
    // The name of the host
    std::string hostname;
    // The listening port of the host
    int receiveport;
    // All of the host's neighbours
    std::map< std::string, struct link > links;
    // The host's current distance vectors
    std::map< std::string, std::map< std::string, int > > fwdtable;
};
