#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <netdb.h>

#include <map>

#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include <stdlib.h>
#include "hostobj.h"

#define MAXBUF 1024

using namespace std;

// CONSTRUCT STRING FUNCTION, CREATES THE MESSAGE SENT
string constructString(HostObj this_host, int messageType){
    string finalString;

    if(messageType==1){         // Message type 1 indicates a DV message
        finalString += "type:dv\nsrc:";
        finalString += this_host.getHostname();
        finalString += "\ndata:";
        finalString += this_host.getDistanceVector(this_host.getHostname());
    }
    else{                       // Other messages types are to be coded
        finalString += "type:msg\nsrc:";
        finalString += this_host.getHostname();
        finalString += "\ndata:";
        finalString += this_host.getDistanceVector(this_host.getHostname());
        finalString += "dest:";
    }

    //cout << "SENDING" << endl << finalString << endl;
    return finalString;

}


// MESSAGE RECEIVED FUNCTION, INTERPRETS AN INCOMING MESSAGE
void messageReceived(HostObj *this_host, string input){

    //cout << "INPUT" << endl << input << endl;

    // Maintains coordinates of characters in strings
    int point1, point2;

    // Track length of strings
    int stringLength, subStringLength;

    // temp holds holds flags which we search for in the string
    // tempInput holds the to be uncoded parts of the message
    string temp, tempInput;

    // These variables hold isolated message parameters
    string type, sourceNode, subString, tempDV;
    int weight;

    tempInput = input;

    // Keep track of the length of the message
    stringlength = tempInput.length();

    //Checking the type of message sent
    temp= "type:";
    point1=tempInput.find(temp);
    point1=point1+temp.length();
    point2=tempInput.find("\n");

    type=tempInput.substr(point1,point2-point1);

    stringlength -= point2;
    tempInput = tempInput.substr(point2 + 1,stringlength);

    // Checking which node sent the message
    temp= "src:";
    point1=tempInput.find(temp);
    point1=point1+temp.length();
    point2=tempInput.find("\n");

    sourceNode=tempInput.substr(point1,point2-point1);

    stringlength -= point2;
    tempInput = tempInput.substr(point2 + 1,stringlength);

    // Use the source information to reset the timeout of the source
    this_host->activateNeighbour(sourceNode);

    // Isolate the data of the message into subString
    temp= "data:";
    point1=tempInput.find(temp);
    point1=point1+temp.length();
    point2=tempInput.find("\n");

    subString = tempInput.substr(point1,point2-point1 + 1);

    // If the message is of type Distance Vector, update the table if data is new
    // Check if data is new by comparing DV sent to the current DV in memory
    if( (type=="dv") && (subString != this_host->getDistanceVector(sourceNode)) ){

        // Debug message, indicates why change was made
        cout << "RECIEVING CHANGED DISTANCE VECTOR" << endl;
        cout << "OLD DV = " << this_host->getDistanceVector(sourceNode) << endl;
        cout << "NEW DV = " << subString << endl;

        // At this point write the current host distance vector to a file
        // Must include timestamp and other requirements from handout

        // As we are replacing data, we delete the data we previously had
        this_host->clearRow(sourceNode);

        // At this stage, subString hold a list of DVs in the form AB1 AE4 (May need to change)
        subStringLength = subString.length();

        // While there are spaces left in the message
        while(point2!=-1){
            // Isolate a single Distance Value of the form AB1
            point1 = subString.find(sourceNode);
            point2 = subString.find(" ");

            // tempDV stores individual Distance Values
            tempDV = subString.substr(point1,point2-point1);

            // POSSIBLY TEMPORARY CHANGE:
            // If I know a router is turned off any message telling me it can get to that node is out-of-date
            // Therefore I dont update a node that I know is inactive

            // Update information on all nodes that I am not directly connected to
            if(this_host->getLinks()->find(string(1,tempDV[1])) == this_host->getLinks()->end()){
                // Get weight from final portion of tempDV
                weight = stoi(tempDV.substr(2, tempDV.length() - 2));

                // Update the host's information
                this_host->updateTable(string(1,tempDV[0]),string(1,tempDV[1]),weight);

                // Cut the Distance Value recorded from the rest of the subString
                subStringLength-=point2;
                subString = subString.substr(point2+1,stringlength);
            }
            // Else it's a neighbour, I only update if I know its active
            else if (this_host->getLinks()->find(string(1,tempDV[1]))->second.active) {
                // Get weight from final portion of tempDV
                weight = stoi(tempDV.substr(2, tempDV.length() - 2));

                // Update the host's information
                this_host->updateTable(string(1,tempDV[0]),string(1,tempDV[1]),weight);

                // Cut the Distance Value recorded from the rest of the subString
                subStringLength-=point2;
                subString = subString.substr(point2+1,stringlength);
            }
            // Else it's a deactivated neighbour, ignore that DV and move on
            else {
                // Cut this Distance Value from the rest of the subString
                subStringLength-=point2;
                subString = subString.substr(point2+1,stringlength);
            }
        }

        // Recalculate the Host's Distance Vector using this new information
        this_host->regenTable();

        // At this point write the new Distance Vector to the file

        // Debug message, view the Distance Vectors stored by the host
        this_host->printTable();
        cout << endl;

    }
    // Possible debug messages for if message is not recognised
    //else if(dv)
    //cout <<"DV received, no change...\n";
    //else
    //cout << "Data received: " << subString << endl;
}

int main(int argc, char* argv[])
{
    // Check if a host was passed to the program
    if(argc != 2){
        cout << "Incorrect arguments" << endl;
        return 0;
    }


    // DECLARE VARIABLES
    // datafile allows file manipulation
    fstream datafile;
    // These strings temporarily store object attributes as they are read in from a file
    string tempHostname, tempDest, tempPort, tempWeight, currInput;
    // Store the hostname from the user input
    HostObj thisHost(argv[1]);


    // READ IN DATA CODE
    // Open or create the neighbourhood topology file
    datafile.open("neighbourhood.xml", ios::in);
    if(!datafile){
        // Neighbourhood topology does not exist and must be created
        datafile.open("neighbourhood.xml", ios::out);

        // Declare links attached to A
        datafile << "<host>" << endl;
        datafile << "<hostname>" << endl;
        datafile << "A" << endl;
        datafile << "</hostname>" << endl;

        datafile << "<link>" << endl;
        datafile << "<destination>" << endl;
        datafile << "B" << endl;
        datafile << "</destination>" << endl;
        datafile << "<port>" << endl;
        datafile << "10001" << endl;
        datafile << "</port>" << endl;
        datafile << "<weight>" << endl;
        datafile << "4" << endl;
        datafile << "</weight>" << endl;
        datafile << "</link>" << endl;

        datafile << "<link>" << endl;
        datafile << "<destination>" << endl;
        datafile << "E" << endl;
        datafile << "</destination>" << endl;
        datafile << "<port>" << endl;
        datafile << "10004" << endl;
        datafile << "</port>" << endl;
        datafile << "<weight>" << endl;
        datafile << "1" << endl;
        datafile << "</weight>" << endl;
        datafile << "</link>" << endl;

        datafile << "</host>" << endl << endl;

        // Declare links attached to B
        datafile << "<host>" << endl;
        datafile << "<hostname>" << endl;
        datafile << "B" << endl;
        datafile << "</hostname>" << endl;

        datafile << "<link>" << endl;
        datafile << "<destination>" << endl;
        datafile << "A" << endl;
        datafile << "</destination>" << endl;
        datafile << "<port>" << endl;
        datafile << "10000" << endl;
        datafile << "</port>" << endl;
        datafile << "<weight>" << endl;
        datafile << "4" << endl;
        datafile << "</weight>" << endl;
        datafile << "</link>" << endl;

        datafile << "<link>" << endl;
        datafile << "<destination>" << endl;
        datafile << "C" << endl;
        datafile << "</destination>" << endl;
        datafile << "<port>" << endl;
        datafile << "10002" << endl;
        datafile << "</port>" << endl;
        datafile << "<weight>" << endl;
        datafile << "3" << endl;
        datafile << "</weight>" << endl;
        datafile << "</link>" << endl;

        datafile << "<link>" << endl;
        datafile << "<destination>" << endl;
        datafile << "E" << endl;
        datafile << "</destination>" << endl;
        datafile << "<port>" << endl;
        datafile << "10004" << endl;
        datafile << "</port>" << endl;
        datafile << "<weight>" << endl;
        datafile << "2" << endl;
        datafile << "</weight>" << endl;
        datafile << "</link>" << endl;

        datafile << "<link>" << endl;
        datafile << "<destination>" << endl;
        datafile << "F" << endl;
        datafile << "</destination>" << endl;
        datafile << "<port>" << endl;
        datafile << "10005" << endl;
        datafile << "</port>" << endl;
        datafile << "<weight>" << endl;
        datafile << "1" << endl;
        datafile << "</weight>" << endl;
        datafile << "</link>" << endl;

        datafile << "</host>" << endl << endl;

        // Declare links attached to C
        datafile << "<host>" << endl;
        datafile << "<hostname>" << endl;
        datafile << "C" << endl;
        datafile << "</hostname>" << endl;

        datafile << "<link>" << endl;
        datafile << "<destination>" << endl;
        datafile << "B" << endl;
        datafile << "</destination>" << endl;
        datafile << "<port>" << endl;
        datafile << "10001" << endl;
        datafile << "</port>" << endl;
        datafile << "<weight>" << endl;
        datafile << "3" << endl;
        datafile << "</weight>" << endl;
        datafile << "</link>" << endl;

        datafile << "<link>" << endl;
        datafile << "<destination>" << endl;
        datafile << "D" << endl;
        datafile << "</destination>" << endl;
        datafile << "<port>" << endl;
        datafile << "10003" << endl;
        datafile << "</port>" << endl;
        datafile << "<weight>" << endl;
        datafile << "4" << endl;
        datafile << "</weight>" << endl;
        datafile << "</link>" << endl;

        datafile << "<link>" << endl;
        datafile << "<destination>" << endl;
        datafile << "F" << endl;
        datafile << "</destination>" << endl;
        datafile << "<port>" << endl;
        datafile << "10005" << endl;
        datafile << "</port>" << endl;
        datafile << "<weight>" << endl;
        datafile << "1" << endl;
        datafile << "</weight>" << endl;
        datafile << "</link>" << endl;

        datafile << "</host>" << endl << endl;

        // Declare links attached to D
        datafile << "<host>" << endl;
        datafile << "<hostname>" << endl;
        datafile << "D" << endl;
        datafile << "</hostname>" << endl;

        datafile << "<link>" << endl;
        datafile << "<destination>" << endl;
        datafile << "C" << endl;
        datafile << "</destination>" << endl;
        datafile << "<port>" << endl;
        datafile << "10002" << endl;
        datafile << "</port>" << endl;
        datafile << "<weight>" << endl;
        datafile << "4" << endl;
        datafile << "</weight>" << endl;
        datafile << "</link>" << endl;

        datafile << "<link>" << endl;
        datafile << "<destination>" << endl;
        datafile << "F" << endl;
        datafile << "</destination>" << endl;
        datafile << "<port>" << endl;
        datafile << "10005" << endl;
        datafile << "</port>" << endl;
        datafile << "<weight>" << endl;
        datafile << "3" << endl;
        datafile << "</weight>" << endl;
        datafile << "</link>" << endl;

        datafile << "</host>" << endl << endl;

        // Decale links attached to E
        datafile << "<host>" << endl;
        datafile << "<hostname>" << endl;
        datafile << "E" << endl;
        datafile << "</hostname>" << endl;

        datafile << "<link>" << endl;
        datafile << "<destination>" << endl;
        datafile << "A" << endl;
        datafile << "</destination>" << endl;
        datafile << "<port>" << endl;
        datafile << "10000" << endl;
        datafile << "</port>" << endl;
        datafile << "<weight>" << endl;
        datafile << "1" << endl;
        datafile << "</weight>" << endl;
        datafile << "</link>" << endl;

        datafile << "<link>" << endl;
        datafile << "<destination>" << endl;
        datafile << "B" << endl;
        datafile << "</destination>" << endl;
        datafile << "<port>" << endl;
        datafile << "10001" << endl;
        datafile << "</port>" << endl;
        datafile << "<weight>" << endl;
        datafile << "2" << endl;
        datafile << "</weight>" << endl;
        datafile << "</link>" << endl;

        datafile << "<link>" << endl;
        datafile << "<destination>" << endl;
        datafile << "F" << endl;
        datafile << "</destination>" << endl;
        datafile << "<port>" << endl;
        datafile << "10005" << endl;
        datafile << "</port>" << endl;
        datafile << "<weight>" << endl;
        datafile << "3" << endl;
        datafile << "</weight>" << endl;
        datafile << "</link>" << endl;

        datafile << "</host>" << endl << endl;

        // Declare links attached to F
        datafile << "<host>" << endl;
        datafile << "<hostname>" << endl;
        datafile << "F" << endl;
        datafile << "</hostname>" << endl;

        datafile << "<link>" << endl;
        datafile << "<destination>" << endl;
        datafile << "B" << endl;
        datafile << "</destination>" << endl;
        datafile << "<port>" << endl;
        datafile << "10001" << endl;
        datafile << "</port>" << endl;
        datafile << "<weight>" << endl;
        datafile << "1" << endl;
        datafile << "</weight>" << endl;
        datafile << "</link>" << endl;

        datafile << "<link>" << endl;
        datafile << "<destination>" << endl;
        datafile << "C" << endl;
        datafile << "</destination>" << endl;
        datafile << "<port>" << endl;
        datafile << "10002" << endl;
        datafile << "</port>" << endl;
        datafile << "<weight>" << endl;
        datafile << "1" << endl;
        datafile << "</weight>" << endl;
        datafile << "</link>" << endl;

        datafile << "<link>" << endl;
        datafile << "<destination>" << endl;
        datafile << "D" << endl;
        datafile << "</destination>" << endl;
        datafile << "<port>" << endl;
        datafile << "10003" << endl;
        datafile << "</port>" << endl;
        datafile << "<weight>" << endl;
        datafile << "3" << endl;
        datafile << "</weight>" << endl;
        datafile << "</link>" << endl;

        datafile << "<link>" << endl;
        datafile << "<destination>" << endl;
        datafile << "E" << endl;
        datafile << "</destination>" << endl;
        datafile << "<port>" << endl;
        datafile << "10004" << endl;
        datafile << "</port>" << endl;
        datafile << "<weight>" << endl;
        datafile << "3" << endl;
        datafile << "</weight>" << endl;
        datafile << "</link>" << endl;
        datafile << "</host>" << endl << endl;
    }
    datafile.close();

    // Read neighbourhood topology file
    datafile.open("neighbourhood.xml", ios::in);

    while(datafile >> currInput) {
        tempHostname = "";
        //cout << "READING IN " << currInput << endl;

        // Read input until the hostname tag is found
        if(currInput == "<hostname>"){
            datafile >> tempHostname;
            //cout << "Found host " << tempHostname << endl;
        }

        // If the hostname is thisHost's name, this information is relevant and must be read
        if(tempHostname == thisHost.getHostname()){
            while(datafile >> currInput){
                // This loop runs while reading information regarding a host
                // We leave this loop when the host endtag is read in

                //cout << "READING IN " << currInput << endl;

                // Leave if endtag found
                if(currInput == "</host>")
                    break;

                while(datafile >> currInput){
                    // This loop runs while reading information regarding a link
                    // Leave this loop when the link endtag is read in

                    //cout << "READING IN " << currInput << endl;

                    // Leave if endtag found
                    if(currInput == "</link>")
                        break;

                    // If endtag is not yet found, we then read in the attributes

                    // A link's detination
                    if(currInput == "<destination>")
                        datafile >> tempDest;

                    // A link's port
                    if(currInput == "<port>")
                        datafile >> tempPort;

                    // The weight associated with a link
                    if(currInput == "<weight>")
                        datafile >> tempWeight;
                }

                //cout << "Creating a link with properties " << tempDest << " " << tempPort << " " << tempWeight << endl;

                // Add a new link, with the attributes we've previously isolated
                thisHost.addLink(tempDest, atoi(tempPort.c_str()), atoi(tempWeight.c_str()));
            }
        }
    }

    // We are done reading the file, so we close it
    datafile.close();


    // CONNECTION SETUP CODE

    // The port which I listen to is stored in the host object
    int myPort = thisHost.getreceivePort();

    //Initialize other variables

    // sockfd is the ID of the listening socket
    // This is the socket which receives incoming data
    int sockfd;

    // The buffer stores the message which will be sent to other routers
    char buffer[MAXBUF];

    // Socket address for the host
    struct sockaddr_in myAddr;

    // Create UDP listening socket
    if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
        perror("Failed to create socket");
        exit(EXIT_FAILURE);
    }

    // Clear the socket address, ensure we are starting from a blank address
    memset(&myAddr, 0, sizeof(myAddr));

    //Information of the host node
    myAddr.sin_family    = AF_INET;                 // Use IPv4 format
    myAddr.sin_addr.s_addr = htonl(INADDR_ANY);     // Assign IP address to this socket
    myAddr.sin_port = htons(myPort);                // Assign the port to this socket

    // Bind the socket with the host's address
    if ( bind(sockfd, (const struct sockaddr *)&myAddr, sizeof(myAddr)) < 0 ){
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    // We want to access the host's direct neighbours
    // Use getLinks to get a linked list of all the host's neighbours
    std::map< std::string, struct link >* links = thisHost.getLinks();
    map< std::string, struct link >::iterator itr;

    // Store temporary address information in nodeAddr
    struct sockaddr_in nodeAddr;

    // Initialising neighbouring port addresses
    // For every neighbour the host has,
    for(itr = links->begin(); itr != links->end(); ++itr){
        // Start from blank address
        memset(&nodeAddr, 0, sizeof(nodeAddr));

        // Use IPV4
        nodeAddr.sin_family = AF_INET;
        // Use the current neighbour's port
        nodeAddr.sin_port = htons(itr->second.port);
        // Use localhost (AKA 127.0.0.1) as the IP address
        if (inet_aton("127.0.0.1", &nodeAddr.sin_addr) == 0){
            fprintf(stderr, "Error assigning IP address'\n");
            exit(1);
        }

        // Set the current neighbour's address to what is stored in nodeAddr
        itr->second.address = nodeAddr;
    }


    // HANDLING CONNECTIONS CODE

    // myTime stores the interval value that we wait for between messages
    struct timeval myTime;
    // n is the number of characters in a received message
    // numSockets is the number of active connections
    int n, numSockets;
    // len stores various lengths and values associated with the current socket
    socklen_t len;

    // Want interval between messages to be 2 seconds
    myTime.tv_sec = 2;
    myTime.tv_usec = 0;

    // fd_sets are sets of sockets
    fd_set rfds;    // The reading sockets
    fd_set wfds;    // The watching sockets
    fd_set erfds;   // The error sockets

    // All sets are initially empty
    FD_ZERO(&rfds);
    FD_ZERO(&wfds);
    FD_ZERO(&erfds);

    // Must track the maximum socket being used
    // More relevant if we were creating multiple sockets
    int maxSockfd = sockfd;

    // Add the listening socket to the sockets we watch
    FD_SET(sockfd, &wfds);

    // Run this loop forever, we are always listening for messages
    while(true){

        // The reading and error socket sets are initially the watching sockets
        rfds = wfds;
        erfds = wfds;
        // We are initially using zero sockets
        numSockets = 0;

        // Select sets the number of sockets we need to process
        // It returns 0 if no sockets are active in the time interval specified
        if((numSockets = select(maxSockfd + 1, &rfds, NULL, NULL, &myTime)) < 0) {
            perror("select");
            return -1;
        }

        // If no sockets were active for the time interval
        if(numSockets == 0){
            // Make the message we want to send
            // Use information from thisHost
            // The message will be of a distance vector type denoted by 1
            string m = constructString(thisHost,1);

            // Message can only be transmitted as a char pointer
            // Must convert as we do below
            const char* message = m.c_str();

            // Reset the interval so we once again wait 2 seconds to send a message
            myTime.tv_sec = 2;

            // For every active socket (Will just be 1 for us)
            for(int fd = 0; fd <= maxSockfd; fd++){
                // For every neighbour
                for(itr = links->begin(); itr != links->end(); ++itr){
                    // Send our distance vector message
                    sendto(fd, (const char *)message, strlen(message), 0, (const struct sockaddr *) &itr->second.address,  sizeof(itr->second.address));
                }
            }

            // Handle timeouts below
            // For every active link
            for(itr = links->begin(); itr != links->end(); ++itr){
                if(itr->second.active){

                    // Reduce the lifetime by the two seconds that have passed
                    itr->second.lifetime -= 2;

                    // If the lifetime has run out
                    if(itr->second.lifetime < 0){
                        cout << "Detected Router " << itr->first << " as INACTIVE" << endl << endl;

                        // Deactivate the link
                        thisHost.deleteNeighbour(itr->first);
                    }
                }
            }
        }
        else{
            // If a socket was active during the time interval
            // For every socket (1 in our case)
            for(int fd = 0; fd <= maxSockfd; fd++){
                // Read in the message
                if((n = recvfrom(fd, buffer, MAXBUF, 0, ( struct sockaddr *) &nodeAddr, &len)) > 0){
                    // Null-terminate the incoming message
                    buffer[n] = '\0';
                    // Convert the message to a string
                    string input = string(buffer);

                    // Handle the message that is received
                    // Note thisHost address passed
                    // This is to actually change the values stored by the host
                    messageReceived(&thisHost, input);
                }
            }
        }
    }

/*
    // OLD DISTANCE VECTOR TESTING CODE

    // Relevant HostObj Functions:
    // clearRow()           When receiving an update message, must disregard previous information using this function
    // activateNeighbour()  If a new neighbouring node comes online, implement it in fowarding table
    // deleteNeighbour()    If a neighbouring node times out, remove it from the forwarding table
    // updateTable()        Manual input into forwarding table
    // regenTable()         Recalculates the distance vector of the host, should perform after a table update (active, delete, update)
    // printTable()         Display the host's forwarding table in console
    // findSendLink()       Given a destination, provides link pointer to best sending port. If none exist, pointer is NULL

    cout << endl;
    thisHost.printLinks();

    cout << endl << "HOST STARTS UP" << endl;
    thisHost.printTable();

    cout << endl << "NEIGHBOUR B HAS COME ALIVE" << endl;
    thisHost.activateNeighbour("B");
    thisHost.updateTable("B","B", 0);
    thisHost.regenTable();
    thisHost.printTable();

    cout << endl << "RECEIVED DATA FROM B" << endl;
    thisHost.updateTable("B","A", 4);
    thisHost.updateTable("B","C", 3);
    thisHost.updateTable("B","F", 1);
    thisHost.printTable();

    cout << endl << "CALCULATE DISTANCE VECTORS" << endl;
    thisHost.regenTable();
    thisHost.printTable();

    cout << endl << "NEIGHBOUR E HAS COME ALIVE" << endl;
    thisHost.activateNeighbour("E");
    thisHost.updateTable("E","E", 0);
    thisHost.regenTable();
    thisHost.printTable();

    cout << endl << "RECEIVED DATA FROM E" << endl;
    thisHost.updateTable("E","A", 1);
    thisHost.updateTable("E","F", 3);
    thisHost.printTable();

    cout << endl << "RECALCULATE DISTANCE VECTORS" << endl;
    thisHost.regenTable();
    thisHost.printTable();

    cout << endl << "SEND PACKET TO ROUTER A" << endl;
    if(thisHost.findSendLink("A") != NULL)
      cout << "To send to router A, outgoing router is " << char(thisHost.findSendLink("A")->port - 9935) << endl;
    else cout << "No passage to A" << endl;

    cout << endl << "SEND PACKET TO ROUTER B" << endl;
    if(thisHost.findSendLink("B") != NULL)
      cout << "To send to router B, outgoing router is " << char(thisHost.findSendLink("B")->port - 9935) << endl;
    else cout << "No passage to B" << endl;

    cout << endl << "SEND PACKET TO ROUTER C" << endl;
    if(thisHost.findSendLink("C") != NULL)
      cout << "To send to router C, outgoing router is " << char(thisHost.findSendLink("C")->port - 9935) << endl;
    else cout << "No passage to C" << endl;

    cout << endl << "SEND PACKET TO ROUTER D" << endl;
    if(thisHost.findSendLink("D") != NULL)
      cout << "To send to router D, outgoing router is " << char(thisHost.findSendLink("C")->port - 9935) << endl;
    else cout << "No passage to D" << endl;

    cout << endl << "NODE E HAS CRASHED" << endl;
    thisHost.deleteNeighbour("E");
    thisHost.printTable();

    cout << endl << "RECALCULATE DISTANCE VECTORS" << endl;
    thisHost.regenTable();
    thisHost.printTable();

    cout << endl << "NODE C HAS CRASHED" << endl;
    thisHost.clearRow("B");
    thisHost.updateTable("B","A", 4);
    thisHost.updateTable("B","F", 1);
    thisHost.printTable();

    cout << endl << "RECALCULATE DISTANCE VECTORS" << endl;
    thisHost.regenTable();
    thisHost.printTable();

    cout << endl << "SEND PACKET TO ROUTER A" << endl;
    if(thisHost.findSendLink("A") != NULL)
      cout << "To send to router A, outgoing router is " << char(thisHost.findSendLink("A")->port - 9935) << endl;
    else cout << "No passage to A" << endl;

    cout << endl << "SEND PACKET TO ROUTER C" << endl;
    if(thisHost.findSendLink("C") != NULL)
      cout << "To send to router C, outgoing router is " << char(thisHost.findSendLink("C")->port - 9935) << endl;
    else cout << "No passage to C" << endl;

    cout << endl << "SEND PACKET TO ROUTER E" << endl;
    if(thisHost.findSendLink("E") != NULL)
      cout << "To send to router E, outgoing router is " << char(thisHost.findSendLink("E")->port - 9935) << endl;
    else cout << "No passage to E" << endl;

    cout << endl << "Distance Vector for F is: " << thisHost.getDistanceVector("F") << endl;
*/

	return 0;
}
