#include "hostobj_Connected.h"
#include "link.h"
#include <string>
#include <map>
#include <iostream>

using namespace std;

HostObj::HostObj(std::string name){
    hostname = name;
    receiveport = name[0] + 9935;
    /*
    for(int i = 0; i < 6; i++)
        for(int j = 0; j < 6; j++)
            if(i == j)
                fwdtable[i][j] = 0;             // Distance of zero from itself
            else
                fwdtable[i][j] = 999999999;     // All other hosts are at infinity
                */

    map <string, int> newMap;
    newMap.insert(pair<string, int>(hostname, 0));
    fwdtable.insert(pair<string, map<string, int> >(hostname, newMap));
}

void HostObj::addLink(string dest, int port, int weight){
    link newLink;
    newLink.port = port;
    newLink.weight = weight;
    newLink.active = false;
    // Maybe add a struct

    links.insert(pair<string, link >(dest, newLink));

    //fwdtable[hostname[0] - 65][dest[0] - 65] = weight;
    //cout << "Inserting link " << dest << endl;

    //fwdtable.find(hostname)->second.insert(pair<string, int>(dest, weight));

    return;
}

void HostObj::activateNeighbour(string neighbour){
    int weight;

    if(links.find(neighbour) == links.end())
      return;

    links.find(neighbour)->second.active = true;
    weight = links.find(neighbour)->second.weight;

    fwdtable.find(hostname)->second.insert(pair<string, int>(neighbour, weight));
}

void HostObj::deleteNeighbour(string neighbour){
    if(links.find(neighbour) == links.end())
      return;

    links.find(neighbour)->second.active = false;
    fwdtable.erase(neighbour);
}

void HostObj::printLinks(){
    map<string, link >::iterator itr;

    cout << "Here are the links associated with " << hostname << " at port " << receiveport << ":\n";

    for(itr = links.begin(); itr != links.end(); ++itr){
        cout << "Dest = " << itr->first << " Port = " << itr->second.port << " Weight = " << itr->second.weight << "\n";
    }

    return;
}

void HostObj::printTable(){
    cout << "Forwarding table for host " << hostname << ":\n";
    string src, dest;

    for(int i = 0; i < 6; i++){
        src = char(i + 65);
        if(fwdtable.find(src) != fwdtable.end()){
            for(int j = 0; j < 6; j++){
                dest = char(j + 65);
                if(fwdtable.find(src)->second.find(dest) != fwdtable.find(src)->second.end())
                    cout << fwdtable.find(src)->second.find(dest)->second << " ";
                else
                    cout << "- ";
            }
            cout << endl;
        }
        else
            cout << "- - - - - -\n";
    }

    /*
    for(int i = 0; i < 6; i++){
        for(int j = 0; j < 6; j++)
            if(fwdtable[i][j] != 999999999)
                cout << fwdtable[i][j] << " ";
            else
                cout << "- ";
        cout << endl;
    }*/

    return;
}

void HostObj::updateTable(std::string src, std::string dest, int newWeight){
    //fwdtable[src[0] - 65][dest[0] - 65] = newWeight;

    if(fwdtable.find(src) == fwdtable.end()){ // This is a new source
        map<string, int> newMap;
        newMap.insert(pair<string, int>(src, 0));
        newMap.insert(pair<string, int> (dest, newWeight));
        fwdtable.insert(pair<string, map<string, int> >(src, newMap));
    }
    else {
        if(fwdtable.find(src)->second.find(dest) == fwdtable.find(src)->second.end()) // This is a new destination
            fwdtable.find(src)->second.insert(pair<string, int> (dest, newWeight));
        else  // Changing an existing entry
            fwdtable.find(src)->second.find(dest)->second = newWeight;
    }

    return;
}

void HostObj::regenTable(){

    int cost;
    bool change = true; // If the table updates, links involved in other calculations may have been changed and need to be updated

    map<string, struct link>::iterator itr1;
    map<string, int>::iterator itr2;
    map<string, int> hostTable;
    map<string, int>* currTable;

    fwdtable.erase(hostname);
    hostTable.insert(pair<string, int> (hostname, 0));

    // NEED TO REMODEL REGEN TABLE
    // MUST HANDLE A LINK BEING REMOVED
    // MUST REMOVE PATHS THAT USE A DEACTIVATED LINK
    // AT THE MOMENT FUNCTION UPDATES THE TABLE EACH TIME, NEEDS TO COMPLETELY GENERATE THE TABLE
    while(change){
      change = false;
      for(itr1 = links.begin(); itr1 != links.end(); ++itr1){   // For each link to the host
        //cout << "Checking link " << itr1->first << endl;
        if(itr1->second.active){   // If this link is active
          if(hostTable.find(itr1->first) != hostTable.end())
            cost = hostTable.find(itr1->first)->second;  // Grab lowest cost to this node
          else
            cost = links.find(itr1->first)->second.weight; // If there is no established link, refer to base link

          //cout << "Cost to " << itr1->first << " is " << cost << endl;

          if(fwdtable.find(itr1->first) != fwdtable.end()){
            //cout << "Checking " << itr1->first << "'s links" << endl;
            currTable = &fwdtable.find(itr1->first)->second;  // Analyse fwdtable of the current node

            for(itr2 = currTable->begin(); itr2 != currTable->end(); ++itr2){ // Check all nodes the current node has connections to
              //cout << itr1->first << " is connected to " << itr2->first << " with a weight of " << itr2->second << endl;
              if(hostTable.find(itr2->first) == hostTable.end()){ // If the host node has no link to this destination
                change = true;
                //cout << "Adding " << itr2->first << " with weight " << cost + itr2->second << endl;
                hostTable.insert(pair<string, int>(itr2->first, cost + itr2->second));  // Add a link through currNode
              } else if(cost + itr2->second < hostTable.find(itr2->first)->second){  // If the path through currNode is cheaper
                change = true;
                //cout << hostTable.find(itr2->first)->first << " with weight " << hostTable.find(itr2->first)->second;
                //cout << " is now ";
                //cout << hostTable.find(itr2->first)->first << " with weight " << cost + itr2->second << endl;
                hostTable.find(itr2->first)->second = cost + itr2->second;  // Set advertised path to path through currNode
              }

            }
          } else {
            // No entry for this node in the forward table
            // This would indicate that a node is active, yet we have no path information
            // Is this an error state?
            //cout << endl << "ERROR SCENARIO" << endl;
          }
        }
      }
    }

    fwdtable.insert(pair<string, map <string, int> > (hostname, hostTable));

    return;
/*
    do{
        change = false;
        for(itr1 = links.begin(); itr1 != links.end(); ++itr1){ // Set currNode as one of the hosts links
            if(hostTable->find(itr1->first) != hostTable->end()){ // If the link has been activated
              cost = hostTable->find(itr1->first)->second; // Cost is cost from host to currNode
              cout << "Cost to " << itr1->first << " is " << cost << endl;

              if(fwdtable.find(itr1->first) != fwdtable.end()){
                  cout << "Checking " << itr1->first << "'s links\n";
                  currTable = &fwdtable.find(itr1->first)->second; // Analyse fwdtable of currNode

                  for(itr2 = currTable->begin(); itr2 != currTable->end(); ++itr2){ // Check all nodes currNode is connected to
                      cout << itr1->first << " is connected to " << itr2->first << " with a weight of " << itr2->second << endl;
                      if(hostTable->find(itr2->first) == hostTable->end()){ // If the host has no link to this node
                          change = true;
                          cout << "Adding " << itr2->first << " with weight " << cost + itr2->second << endl;
                          hostTable->insert(pair<string, int>(itr2->first, cost + itr2->second));  // Add a link through currNode
                      } else if(cost + itr2->second < hostTable->find(itr2->first)->second){  // If the path through currNode is cheaper
                          change = true;
                          cout << hostTable->find(itr2->first)->first << " with weight " << hostTable->find(itr2->first)->second;
                          cout << " is now ";
                          cout << hostTable->find(itr2->first)->first << " with weight " << cost + itr2->second << endl;
                          hostTable->find(itr2->first)->second = cost + itr2->second;  // Set advertised path to path through currNode
                      }
                  }
              }
            }
        }*/

        /*
        for(int i = 0; i < 6; i++)
            if(i != hostname[0] - 65){
                // Cost to get from host to curr node
                cost = fwdtable[hostname[0] - 65][i];
                //cout << "Cost from " << hostname << " to " << currNodeName << " is " << cost << endl;

                for(int j = 0; j < 6; j++)
                    if(cost + fwdtable[i][j] < fwdtable[hostname[0] - 65][j]){
                        fwdtable[hostname[0] - 65][j] = cost + fwdtable[i][j];
                        change = true;
                    }

            }*/
   // } while (change);

}

string HostObj::getHostname(){
    return hostname;
}

int HostObj::getWeight(string dest){
    map<string, link >::iterator relevantLink = links.find(dest);

    if(relevantLink == links.end())
        return -1;
    else
        return relevantLink->second.weight;
}

int HostObj::getsendPort(string dest){
    map<string, link >::iterator relevantLink = links.find(dest);

    if(relevantLink == links.end())
        return -1;
    else
        return relevantLink->second.port;
}

int HostObj::getreceivePort(){
    return receiveport;
}

std::map< std::string, link > HostObj::getLinks(){
    return links;
}
