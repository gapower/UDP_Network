#include "hostobj.h"
#include "link.h"
#include <string>
#include <map>
#include <iostream>

using namespace std;

// IMPLEMENTATION OF HOSTOBJ FUNCTIONS

// The constructor, create a new host given a name
HostObj::HostObj(std::string name){
    // Assign the host's new name
    hostname = name;

    // Select the port used based on the name (maybe bad idea)
    receiveport = name[0] + 9935;

    // Create empty distance vector table
    // Only value in this table is that a host is connected to itself
    map <string, pair<string, int> > newMap;
    newMap.insert(pair<string, pair<string, int> >(hostname, pair<string, int>(hostname, 0)));
    fwdtable.insert(pair<string, map<string, pair<string, int> > >(hostname, newMap));
}

// Add a new neighbour to the host, given the neighbour's name, port, and weight
void HostObj::addLink(string dest, int port, int weight){
    // Make new link struct and populate attributes
    link newLink;
    newLink.port = port;
    newLink.weight = weight;
    newLink.active = false;     // Neighbours are by default unactive
    newLink.lifetime = 0;       // Time to deactivation is 0, they are already deactivated

    // Add this new struct to the list of links
    // Key value is the host's name
    links.insert(pair<string, link >(dest, newLink));

    return;
}

// Mark a neighbour as active
void HostObj::activateNeighbour(string neighbour){
    // Store the neighbour's weight, so that we can create a distance vector
    int weight;

    // If the link doesn't exist, do nothing
    if(links.find(neighbour) == links.end()){
        //cout << "Can't find neighbour" << endl;
        return;
    }

    links.find(neighbour)->second.active = true;        // Set the link as active
    links.find(neighbour)->second.lifetime = 10;        // Set the timeout to 10 seconds

    weight = links.find(neighbour)->second.weight;      // Find the weight of this connection

    // Store in host's distance vector, the now active link
    fwdtable.find(hostname)->second.insert(pair<string, pair<string, int> >(neighbour, pair<string, int>(neighbour, weight)));
}

// Mark a neighbour as inactive
void HostObj::deleteNeighbour(string neighbour){
    // Define an iterator, which allows navigation of distance vector table
    map< std::string, std::map< std::string, pair<string, int > > >::iterator itr;

    // If the link doesn't exist, do nothing
    if(links.find(neighbour) == links.end()){
      //cout << "Cant find neighbour" << endl;
      return;
    }

    links.find(neighbour)->second.active = false;       // Set the link as inactive
    links.find(neighbour)->second.lifetime = 0;         // Set the timeout to zero
    fwdtable.erase(neighbour);                          // Remove the neighbour's distance vector from memory

    // For all other distance vectors, remove the distance value to the now deactivated link
    for(itr = fwdtable.begin(); itr != fwdtable.end(); ++itr){
        itr->second.erase(neighbour);
    }

    // Debug messages to examine how deleting a link affects the entire table
    // cout << "DELETING PROCESS: " << endl;
    // printTable();
    // regenTable();
    // printTable();
}

// Print all of a host's neighbours active or inactive
// Used for debugging
void HostObj::printLinks(){
    // Iterate through all of the neighbours
    map<string, link >::iterator itr;

    cout << "Here are the links associated with " << hostname << " at port " << receiveport << ":\n";

    // For each neighbour, print its link characteristics
    for(itr = links.begin(); itr != links.end(); ++itr){
        cout << "Dest = " << itr->first << " Port = " << itr->second.port << " Weight = " << itr->second.weight << "\n";
    }

    return;
}

// Print all of the distance vectors stored in memory
void HostObj::printTable(){
    // This function can only print the distance vectors of the routers provided in the handout

    cout << "Forwarding table for host " << hostname << ":\n";
    string src, dest;

    // For each router
    for(int i = 0; i < 6; i++){
        // Calculate the source character
        // ASCII value = 65 + offset, 65 corresponds to A, 66 to B, etc.
        src = char(i + 65);

        // If this source exists
        if(fwdtable.find(src) != fwdtable.end()){
            // For each router the source can connect to
            for(int j = 0; j < 6; j++){
                // Calculate the destination character
                dest = char(j + 65);

                // If the destination exists
                if(fwdtable.find(src)->second.find(dest) != fwdtable.find(src)->second.end())
                    // Print the weight
                    cout << fwdtable.find(src)->second.find(dest)->second.second << " ";
                // If the destination does not exist, print an empty coordinate
                else
                    cout << "- ";
            }
            cout << endl;
        }
        // If this source does not exist, print an empty row
        else
            cout << "- - - - - -\n";
    }

    return;
}

// Update a distance value, given the source, destination, and the new weight of this path
void HostObj::updateTable(string src, string dest, string path, int newWeight){

    // If the source doesn't exist already in the table
    // This means its a new source and must be added
    if(fwdtable.find(src) == fwdtable.end()){
        // Create a blank distance vector for this new source, with two connections
        // It's connected to itself with 0, and the host with the weight that is passed
        // Add the path node to the distance vector, paths to itself when destination is itself
        map<string, pair<string, int> > newMap;
        newMap.insert(pair<string, pair<string, int> >(src, pair<string, int>(src, 0)));
        newMap.insert(pair<string, pair<string, int> >(dest, pair<string, int>(path, newWeight)));

        // Add this distance vector to our table
        fwdtable.insert(pair<string, map<string, pair<string, int> > >(src, newMap));
    }
    // The source already exists
    else {
        // If the source does not have a path to the destination
        if(fwdtable.find(src)->second.find(dest) == fwdtable.find(src)->second.end()){
            // Change the source's distance vector to have this new path
            fwdtable.find(src)->second.insert(pair<string, pair<string, int> > (dest, pair<string, int>(path, newWeight)));
        // This source has a path to this destination already, must change an existing entry
        } else {
            fwdtable.find(src)->second.find(dest)->second.first = path;
            fwdtable.find(src)->second.find(dest)->second.second = newWeight;
        }
    }

    return;
}

// Calculate the host's distance vector using it's information from its neighbours
void HostObj::regenTable(){
    // Stores the cost to travel to a neighbour
    int cost;
    // If the table updates, links involved in other calculations may have been changed and need to be updated
    bool change = true;

    // For navigating through the list of neighbours
    map<string, struct link>::iterator itr1;
    // For navigating through values in a distance vector
    map<string, pair<string, int> >::iterator itr2;
    // Create a blank distance vector that will be the host's new distance vector
    map<string, pair<string, int> > hostTable;
    // Store distance vectors of neighbours in currTable
    map<string, pair<string, int> >* currTable;

    // Delete the old distance vector of the host
    fwdtable.erase(hostname);
    // First connection in the new table is the host's connection to itself
    hostTable.insert(pair<string, pair<string, int> > (hostname, pair<string, int>(hostname, 0)));

    // Add direct connections to the host's distance vector
    // Only add active neighbours to the DV
    for(itr1 = links.begin(); itr1 != links.end(); ++itr1)
            // If this link is active
            if(itr1->second.active)
                // If not already defined
                if(hostTable.find(itr1->first) == hostTable.end())
                    // Define this link in fwdtable
                    hostTable.insert(pair<string, pair<string, int> > (itr1->first, pair<string, int>(itr1->first, itr1->second.weight)));

    // Run this code as long as the distance vectors are changing
    while(change){
        // No changes have happened by default
        change = false;

        // For each neighbour
        for(itr1 = links.begin(); itr1 != links.end(); ++itr1){
            if(itr1->second.active){
                // Grab lowest cost to this neighbour
                // This will either be the direct connection or some other path
                cost = hostTable.find(itr1->first)->second.second;

                // If this link already exists in the distance vector table
                if(fwdtable.find(itr1->first) != fwdtable.end()){
                    //cout << "Checking " << itr1->first << "'s links" << endl;
                    // Analyse distance vector of the current node
                    currTable = &fwdtable.find(itr1->first)->second;

                    // Check all nodes the current neighbour has connections to
                    for(itr2 = currTable->begin(); itr2 != currTable->end(); ++itr2){
                        //cout << itr1->first << " is connected to " << itr2->first << " with a weight of " << itr2->second << endl;

                        // If the host has no path to this destination
                        // Add a new path travelling through the current neighbour
                        // Note we don't update if the path suggests pathing back through this host
                        //cout << "Comparing " << hostname << " and " << itr2->second.first << endl;
                        if(hostTable.find(itr2->first) == hostTable.end() && hostname != itr2->second.first){
                            // A change has taken place
                            change = true;

                            //cout << "Adding " << itr2->first << " with weight " << cost + itr2->second << endl;

                            // Update the distance vector with this new path
                            hostTable.insert(pair<string, pair<string, int> >(itr2->first, pair<string, int>(itr1->first, cost + itr2->second.second)));
                        }
                        // If the path through currNode is cheaper than the current path the host has
                        // Note we don't update if the path suggests pathing back through this host
                        else if(cost + itr2->second.second < hostTable.find(itr2->first)->second.second && hostname != itr2->second.first){
                            // A change has taken place
                            change = true;

                            //cout << hostTable.find(itr2->first)->first << " with weight " << hostTable.find(itr2->first)->second;
                            //cout << " is now ";
                            //cout << hostTable.find(itr2->first)->first << " with weight " << cost + itr2->second << endl;

                            // Update the distance vector with this new path
                            hostTable.find(itr2->first)->second.second = cost + itr2->second.second;
                        }
                    }
                }
            }
        }
    }

    // Delete poisoned routes
    itr2 = hostTable.begin();
    string delnodeName;

    while(itr2 != hostTable.end()){
        delnodeName = itr2->first;
        if(itr2->second.second > 18){
            ++itr2;
            hostTable.erase(delnodeName);
        } else
            ++itr2;
    }

    // Commit the host's new distance vector to the table of distance vectors
    fwdtable.insert(pair<string, map <string, pair<string, int> > > (hostname, hostTable));

    return;
}

// Find the best route to a destination (will likely be changed)
struct link* HostObj::findSendLink(string dest){
    string path;

    if(fwdtable.find(hostname)->second.find(dest) != fwdtable.find(hostname)->second.end()){
        path = fwdtable.find(hostname)->second.find(dest)->second.first;
    }
    else return NULL;

    if(links.find(path) != links.end())
        return &links.find(path)->second;
    else return NULL;

    /*
    int cost;
    int sendCost = 0;
    link* SendingLink = NULL;
    map<string, struct link>::iterator itr;
    map<string, int>* currNode;

    //cout << endl;
    //cout << "IN FindSendLink()" << endl;

    if(links.find(dest) != links.end()){
      if(links.find(dest)->second.active){
        //cout << "TAKING DIRECT CONNECTION" << endl;
        SendingLink = &links.find(dest)->second;
      }
      //else cout << "NO ACTIVE DIRECT CONNECTION" << endl;
    } //else cout << "NO ACTIVE OR INACTIVE DIRECT CONNECTION" << endl;

    for(itr = links.begin(); itr != links.end(); ++itr){
      //cout << "CHECKING LINK " << itr->first << endl;
      if(itr->second.active){
        //cout << "LINK IS ACTIVE" << endl;
        cost = itr->second.weight;
        //cout << "COST IS " << cost << endl;

        if(fwdtable.find(itr->first) != fwdtable.end()){
          currNode = &fwdtable.find(itr->first)->second;
          if(currNode->find(dest) != currNode->end()){
            //cout << "FOUND CONNECTION THROUGH " << itr->first << endl;
            if(SendingLink == NULL){
              //cout << "CONNECTION WEIGHT = " << currNode->find(dest)->second + cost << endl;
              //cout << "TAKING THIS CONNECTION" << endl;
              SendingLink = &itr->second;
              sendCost = currNode->find(dest)->second;
            }
            else {
              //cout << "CMP " << currNode->find(dest)->second + cost << " " << SendingLink->weight + sendCost << endl;
              //cout << "CONNECTION WEIGHT = " << currNode->find(dest)->second + cost << endl;
              if(currNode->find(dest)->second + cost < SendingLink->weight + sendCost){
                //cout << "TAKING THIS CONNECTION" << endl;
                SendingLink = &itr->second;
                sendCost = currNode->find(dest)->second;
              } //else cout << "IGNORING THIS CONNECTION" << endl;
            }
          } //else cout << "NO CONNECTION THROUGH " << itr->first << endl;
        }
      }
    }
    //cout << endl;
    return SendingLink;
    */
}

// Delete a distance vector
void HostObj::clearRow(string row){
    if(fwdtable.find(row) != fwdtable.end()){
        fwdtable.erase(row);
    }
    else return;
}

// Retrieve the host's name
string HostObj::getHostname(){
    return hostname;
}

// Retrieve the weight of the direct connection to a neighbour
int HostObj::getWeight(string dest){
    map<string, link >::iterator relevantLink = links.find(dest);

    if(relevantLink == links.end())
        return -1;
    else
        return relevantLink->second.weight;
}

// Retrieve the port to send a packet, given a destination
int HostObj::getsendPort(string dest){
    map<string, link >::iterator relevantLink = links.find(dest);

    if(relevantLink == links.end())
        return -1;
    else
        return relevantLink->second.port;
}

// Retrieve the port where we take incoming data
int HostObj::getreceivePort(){
    return receiveport;
}

// Return a pointer to a list of all neighbours
std::map< std::string, link >* HostObj::getLinks(){
    return &links;
}

// Return a string which lists a specified distance vector
// Used to sending a distance vector in a message
// Also used for detecting if incoming DVs have changed
string HostObj::getDistanceVector(string source){
    // For navigating through table of distance vectors
    map< string, map<string, pair<string, int> > >::iterator DistVec;
    // For navigating through a distance vector
    map<string, pair<string, int> >::iterator itr;

    // Start with empty output
    string output = "";

    // Find the relevant distance vector, given a name
    DistVec = fwdtable.find(source);

    // If this distance vector doesn't exist
    if(DistVec == fwdtable.end())
      return "Distance vector doesn't exist";

    // Add each path in the distance vector to the outgoing message
    for(itr = DistVec->second.begin(); itr != DistVec->second.end(); ++itr){
        output += source;                           // Add source node
        output += itr->first;                       // Add destination
        output += itr->second.first;                // Add path node
        output += to_string(itr->second.second);    // Add weight of this path
        output += " ";                              // Space indicates end of path details
    }

    output = output.substr(0, output.length() - 1);     // Remove the excess space char at the end of the message
    output += "\n";                                     // Add newline to indicate the end of the distance vector

    return output;
}
