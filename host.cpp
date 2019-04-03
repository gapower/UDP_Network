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


string constructString(HostObj this_host, int messageType){
      string temp;
      string finalString;
      bool dv = true;



      if(messageType==1){//this is distance vector
        finalString += "type:dv\nsrc:";
        finalString += this_host.getHostname();
        finalString += "\ndata:";
        finalString += this_host.getDistanceVector(this_host.getHostname());
      }
      else{
        finalString += "type:msg\nsrc:";
        finalString += this_host.getHostname();
        finalString += "\ndata:";
        finalString += this_host.getDistanceVector(this_host.getHostname());
        finalString += "dest:";
      }

      return finalString;

}



void messageReceived(HostObj *this_host, string input){

  cout << "Input:" << input << endl;

    int point1,point2,requiredLength;
    string temp,subString,tempInput;
    bool dv = false;
    string type,sourceNode;


    tempInput = input;


    int stringlength=tempInput.length();
    //Checking whether it is receiving a distance vector or not
    temp= "type:";
    point1=tempInput.find(temp);
    point1=point1+temp.length();
    point2=tempInput.find("\n");

    type=tempInput.substr(point1,point2-point1);

    //cout << "Type(dv or data): " << subString << endl;


cout << "1" << endl;


    stringlength -= point2;

    tempInput = tempInput.substr(point2+1,stringlength);

    temp= "src:";
    point1=tempInput.find(temp);
    point1=point1+temp.length();
    point2=tempInput.find("\n");

    sourceNode=tempInput.substr(point1,point2-point1);
    cout << "SourceNode:" << sourceNode << endl;
    cout << this_host->getDistanceVector(sourceNode) << endl;
    stringlength -= point2;
    tempInput = tempInput.substr(point2+1,stringlength);

    cout << "2" << endl;


    this_host->activateNeighbour(sourceNode);




    temp= "data:";
    point1=tempInput.find(temp);
    point1=point1+temp.length();
    point2=tempInput.find("\n");

    subString=tempInput.substr(point1,point2-point1);

    cout << "3" << endl;


    //Let's deal with some incoming DV
    if((type=="dv")&&(subString!=this_host->getDistanceVector(sourceNode))){
      cout << "here?\n" ;
      // New data is to be inputted
      // WRITE CURRENT TABLE TO FILE !!!!!
      this_host->clearRow(sourceNode);
      // At this stage subString will hold a list of DVs in the form AB1 AE4
      int subStringLength = subString.length();
      string tempDV;

      cout << "4" << endl;


      while(point2!=-1){
        point1 = subString.find(sourceNode);
        point2 = subString.find(" ");

        tempDV = subString.substr(point1,point2-point1);//tempDV will hold individual Distance Vectors
        int weight = stoi(string(1,tempDV[2])); // get weight from third character of DV
        this_host->updateTable(string(1,tempDV[0]),string(1,tempDV[1]),weight);

        subStringLength-=point2;

        subString = subString.substr(point2+1,stringlength);
        cout << "5" << endl;


        }
        this_host->regenTable();
        // WRITE NEW TABLE TO FILE !!!!!


        this_host->printTable();

    }
    else if(dv)
    cout <<"DV received, no change...\n";
    else
    cout << "Data received: " << subString << endl;



}




void sendPacket(int myPort);

void DistinguishPacket(char buffer[], int n);


int main(int argc, char* argv[])
{
    if(argc != 2){
        cout << "Incorrect arguments" << endl;
        return 0;
    }


    if(0){

        sendPacket((atoi(argv[1]) + 9935));

        return 0;

    }

    fstream datafile;
    string tempHostname, tempDest, tempPort, tempWeight, currInput;
    HostObj thisHost(argv[1]);

    // Open or create the neibhbourhood topology file
    datafile.open("neighbourhood.xml", ios::in);
    if(!datafile){
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

    // XML reader
    datafile.open("neighbourhood.xml", ios::in);

    while(datafile >> currInput) {
        tempHostname = "";
        //cout << "READING IN " << currInput << endl;
        if(currInput == "<hostname>"){
            datafile >> tempHostname;
            //cout << "Found host " << tempHostname << endl;
        }

        if(tempHostname == thisHost.getHostname()){
            while(datafile >> currInput){
                //cout << "READING IN " << currInput << endl;
                if(currInput == "</host>")
                    break;

                while(datafile >> currInput){
                    //cout << "READING IN " << currInput << endl;
                    if(currInput == "</link>")
                        break;

                    if(currInput == "<destination>")
                        datafile >> tempDest;
                    if(currInput == "<port>")
                        datafile >> tempPort;
                    if(currInput == "<weight>")
                        datafile >> tempWeight;
                }
                //cout << "Creating a link with properties " << tempDest << " " << tempPort << " " << tempWeight << endl;
                thisHost.addLink(tempDest, atoi(tempPort.c_str()), atoi(tempWeight.c_str()));
            }
        }
    }

    datafile.close();

    // CONNECTION TESTING
    // Set up listening socket
    //Initialize the command-line arguments
    int myPort = thisHost.getreceivePort();
    //int NodePort = atoi(argv[2]);
    //Initialize otehr variables
    int sockfd;
    char buffer[MAXBUF];
    //Socket addresses for THIS node and the node we wish to connect to
    struct sockaddr_in myAddr;//, nodeAddr;
    //Create socket
    if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
        perror("Failed to create socket");
        exit(EXIT_FAILURE);
    }
    //(I think this sets the addresses to 0)
    memset(&myAddr, 0, sizeof(myAddr));
    //memset(&nodeAddr, 0, sizeof(nodeAddr));
    //Information of MY node
    myAddr.sin_family    = AF_INET; // IPv4
    myAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    myAddr.sin_port = htons(myPort);
    // Bind the socket with MY NODE's address
    if ( bind(sockfd, (const struct sockaddr *)&myAddr, sizeof(myAddr)) < 0 ){
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    std::map< std::string, struct link >* links = thisHost.getLinks();
    map< std::string, struct link >::iterator itr;

    struct sockaddr_in nodeAddr;

    // initialising neighbouring port addresses
    for(itr = links->begin(); itr != links->end(); ++itr){
        memset(&nodeAddr, 0, sizeof(nodeAddr));

        nodeAddr.sin_family = AF_INET;
        nodeAddr.sin_port = htons(itr->second.port);
        if (inet_aton("127.0.0.1", &nodeAddr.sin_addr) == 0){
            fprintf(stderr, "Error when using 'inet_aton()'\n");
            exit(1);
        }

        itr->second.address = nodeAddr;
    }


    //
    thisHost.printTable();

// SELECT STATEMENT ATTEMPT

    struct timeval myTime;
    int n, numSockets;
    socklen_t len;



    myTime.tv_sec = 5;
    myTime.tv_usec = 0;

    fd_set rfds;
    fd_set wfds;
    fd_set erfds;

    FD_ZERO(&rfds);
    FD_ZERO(&wfds);
    FD_ZERO(&erfds);

    int maxSockfd = sockfd;

    FD_SET(sockfd, &wfds);

    string RouterChar = argv[1];

    string m = constructString(thisHost,1); //1 denotes DV
    //string m = "type:dv\nsrc:" + RouterChar+ "\ndata:" + thisHost.getDistanceVector(RouterChar);
    const char* message = m.c_str();

    while(true){
        rfds = wfds;

        erfds = wfds;
        numSockets = 0;

        if((numSockets = select(maxSockfd + 1, &rfds, NULL, NULL, &myTime)) < 0) {

            perror("select");
            return -1;
        }
        if(numSockets == 0){

            myTime.tv_sec = 5;

            for(int fd = 0; fd <= maxSockfd; fd++){
                for(itr = links->begin(); itr != links->end(); ++itr){
                    sendto(fd, (const char *)message, strlen(message), 0, (const struct sockaddr *) &itr->second.address,  sizeof(itr->second.address));
                }
            }


            for(itr = links->begin(); itr != links->end(); ++itr){


                if(itr->second.active){

                    itr->second.lifetime -= 5;

                    if(itr->second.lifetime < 0){

                        //char temp = (itr->second.port - 9935);
                        cout << "Detected Router " << itr->first << " as INACTIVE" << endl << endl;
                        thisHost.deleteNeighbour(itr->first);

                    }
                }

            }
        }
        else{
            for(int fd = 0; fd <= maxSockfd; fd++){
                if((n = recvfrom(fd, buffer, MAXBUF, 0, ( struct sockaddr *) &nodeAddr, &len)) > 0){

                    buffer[n] = '\0';

                    string input = string(buffer);

                    messageReceived(&thisHost, input);


                    //string temp = "src:";

                    //int marker = input.find(temp);
                    //char source = buffer[marker+temp.length()];
                    //string src (1, source);

                    //DistinguishPacket(buffer, n);
/*
                    for(itr = links->begin(); itr != links->end(); ++itr){

                        if(!(itr->second.active)){

                            if((itr->first) == src){

                                cout << endl << "Detected Router " << source << " as active" << endl << endl;


                                thisHost.activateNeighbour(src);

                                //if(itr->second.active)
                                //    cout << "Active in main" << endl;

                                //itr->second.active = true;
                                //itr->second.lifetime = 15;




                                sendto(fd, (const char *)message, strlen(message), 0, (const struct sockaddr *) &itr->second.address, sizeof(itr->second.address));
                            }

                        }

                        if((itr->second.active)){


                            if((itr->second.port - 9935) == source){

                                itr->second.lifetime = 15;

                            }

                        }
                    }

                    cout << "MESSAGE\n";
                    for(int i = 0; i < n; i++){
                        cout << buffer[i];
                    }
                    cout << endl << endl;


                    */




                }
            }
        }
    }







/*

  string command;
    int commandNumber = 0;
    int runtimes = 0;
    int command = 0;



     cout << endl << "[1] LISTENER " << endl << "[2] SENDER " << endl << "[3] EXIT ";
     bool validCommand = false;



     while (!validCommand){
       getline (cin,command);//Will try use getline as often as possible instead of cin >>
       if(command.length()>1)
         cout<< "Not a valid command, re-enter: ";
       else
         validCommand = true;
     }


     commandNumber = atoi(command.c_str());







    while (runtimes<5){







                int n;
                socklen_t len;
                //Node is set to LISTEN for incoming messages
                if (commandNumber== 1){

                    cout << "Listening on port " << argv[1] << "... " << endl;

                    n = recvfrom(sockfd, buffer, MAXBUF, 0, ( struct sockaddr *) &nodeAddr, &len);
                    buffer[n] = '\0';

                    //Need code that parses the incoming messages and follows standards to deal with the messages
                    string receivedString(buffer);
                    //messageReceived(receivedString);

                    messageReceived(thisHost,receivedString);
                    //cout << "Node: ";
                    /*
                    for(int i = 0; i < n; i++){
                        cout << buffer[i];

                    }
                        cout << endl;



                }

                else if(commandNumber== 2){

                    string stringToSend;
                    const char* cMessage;

                    string desiredPort;

                    cout << "Enter the host name you'd like to contact: ";
                    getline(cin,desiredPort);

                    //looptofind desiredPort
                    bool desiredHostFound = false;
                    itr = links.begin();
                    while(itr!= links.end()&& !desiredHostFound){

                        if((itr->first) == desiredPort){
                          cout << "Host " << itr->first << " found succesfully.\n";
                          desiredHostFound=true;
                        }
                        else {
                          ++itr;
                        }

                    }

                    if(!desiredHostFound){
                      cout << "Failed to find Host " << desiredPort<< " .\n";

                    }
                    else{
                      /*
                      cout << "Enter the message that you wish to send: ";
                      getline(cin,stringToSend);
                      cout << "stringToSend: " << stringToSend << "\n";

                      cMessage = stringToSend.c_str();

                      cout << "cMessage: " << cMessage <<"\n";

                      //I would like to test sending to specific port,
                      stringToSend = constructString(thisHost);
                      cMessage = stringToSend.c_str();
                      //for(itrAddr = linkedAddrs.begin(); itrAddr != linkedAddrs.end(); ++itrAddr){

                          sendto(sockfd, (const char *)cMessage, strlen(cMessage), 0, (const struct sockaddr *) &itr->second.address,  sizeof(itr->second.address));

                      //}

                  }

                }
                else if(commandNumber== 3)
                break;


      //  runtimes++;
  }//End of running process









*/


/*
    // DISTANCE VECTOR TESTING
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

void sendPacket(int myPort){

    int NodePort = 10000;
    char m[MAXBUF];
    char *message;
    fstream packet;
    packet.open("packet.txt", ios::in);
    if (packet.is_open()){
        int i = 0;
        while (!packet.eof()){

            m[i] = packet.get();
            i++;
        }
        //close the file and return 200 OK
        packet.close();
    }
    message = &m[0];

    //Initialize other variables
    int sockfd;
    //Socket addresses for THIS node and the node we wish to connect to
    struct sockaddr_in myAddr, nodeAddr;
    //Create socket
    if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
        perror("Failed to create socket");
        exit(EXIT_FAILURE);
    }
    //(I think this sets the addresses to 0)
    memset(&myAddr, 0, sizeof(myAddr));
    memset(&nodeAddr, 0, sizeof(nodeAddr));
    //Information of MY node
    myAddr.sin_family    = AF_INET; // IPv4
    myAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    myAddr.sin_port = htons(myPort);
    // Bind the socket with MY NODE's address
    if ( bind(sockfd, (const struct sockaddr *)&myAddr, sizeof(myAddr)) < 0 ){
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }
    //Information of the node we wish to connect to
    nodeAddr.sin_family = AF_INET;
    nodeAddr.sin_port = htons(NodePort);
    if (inet_aton("127.0.0.1", &nodeAddr.sin_addr) == 0){
        fprintf(stderr, "Error when using 'inet_aton()'\n");
        exit(1);
    }

    int n;
    socklen_t len;

    sendto(sockfd, (char *)message, strlen(message), 0, (const struct sockaddr *) &nodeAddr,  sizeof(nodeAddr));
}

void DistinguishPacket(char buffer[], int n){

    string input = string(buffer);
    string temp = "type:";

    int marker = input.find(temp);
    char type = buffer[marker+temp.length()];

    if(type == 'p'){

        //sendpacket(buffer);

    }

    else{

        //distanceVector(buffer);

    }
}
