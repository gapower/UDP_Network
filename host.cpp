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

string constructString(HostObj this_host){
      string temp;
      string finalString;
      bool dv = true;
      cout << "Constructing Message to send...\n";
      cout << "Is this a Distance Vector? (yes/no) ";
      getline(cin,temp);
      if (temp=="no"){
        dv = false;
        temp="data";
      }
      else
        temp ="dv";

      finalString += "type:";
      finalString += temp;
      finalString += "\n";
      temp = this_host.getHostname();
      finalString += "src:";
      finalString += temp;//add source, senderName
      finalString += "\n"; //While testing, these didnt make sense, I think because there ended up being two \n s

      if(dv)
        cout << "please enter the distance vectors that\nyou would like to update in the form AB1 separated by a space:";
      else
        cout << "Please enter the string that you would like to send";

      /*
      getline(cin,temp);
      if(temp=="a")
      temp = "AB1 AB2 AB3 AB5";
      finalString += "data:";
      finalString += temp;
      finalString += "\n"; //needed?
      */
      finalString += "data:";
      temp=this_host.getDistanceVector(this_host.getHostname());//seems unneessary to have to include the string as an arg
      cout << temp ;
      finalString += temp;

      return finalString;

}



void messageReceived(HostObj host, string input){


  //  cout <<input << endl ;

    int point1,point2,requiredLength;
    string temp,subString,tempInput;
    bool dv = false;
    string sourceNode;


    tempInput = input;


    int stringlength=tempInput.length();

    temp= "type:";
    point1=tempInput.find(temp);
    point1=point1+temp.length();
    point2=tempInput.find("\n");

    subString=tempInput.substr(point1,point2-point1);

    cout << "Type(dv or data): " << subString << endl;

    if(subString== "dv")
      dv=true;

    stringlength -= point2;

    tempInput = tempInput.substr(point2+1,stringlength);

    temp= "src:";
    point1=tempInput.find(temp);
    point1=point1+temp.length();
    point2=tempInput.find("\n");

    subString=tempInput.substr(point1,point2-point1);

    cout << "Source node: "<< subString << endl;
    sourceNode = subString;
    stringlength -= point2;

    tempInput = tempInput.substr(point2+1,stringlength);


    temp= "data:";
    point1=tempInput.find(temp);
    point1=point1+temp.length();
    point2=tempInput.find("\n");

    subString=tempInput.substr(point1,point2-point1);

    //Let's deal with some incoming DV
    if(dv){

      // At this stage subString will hold a list of DVs in the form AB1 AE4
      int subStringLength = subString.length();
      string tempDV;
      int counter = 0;
      //point1 = subString.find(sourceNode);
      //point2 = subString.find(" ");// Find function returns -1 if not found



      while(point2!=-1){
        point1 = subString.find(sourceNode);
        point2 = subString.find(" ");
        counter ++;

        tempDV = subString.substr(point1,point2-point1);//tempDV will hol
        int weight = stoi(string(1,tempDV[2])); // get weight from third character of DV
        host.updateTable(string(1,tempDV[0]),string(1,tempDV[1]),weight);

        //cout << counter << ": " << tempDV << endl;
        /*
        for(int i=0;i<3;i++){
          cout << tempDV[i] << endl ;
        }
        */
        subStringLength-=point2;

        subString = subString.substr(point2+1,stringlength);


          //need to update subString

        }

        host.printTable();

    }


    else
    cout << "Data: " << subString << endl;



}




int main(int argc, char* argv[])
{
    if(argc != 2){
        cout << "Incorrect arguments" << endl;
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

    std::map< std::string, struct link > links = thisHost.getLinks();
    map< std::string, struct link >::iterator itr;

    struct sockaddr_in nodeAddr;

    // initialising neighbouring port addresses
    for(itr = links.begin(); itr != links.end(); ++itr){
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

/*
    struct timeval myTime;
    int n, numSockets;
    socklen_t len;

    myTime.tv_sec = 0;
    myTime.tv_usec = 500000;

    fd_set rfds;
    fd_set wfds;
    FD_ZERO(&rfds);
    FD_ZERO(&wfds);

    int maxSockfd = sockfd;

    FD_SET(sockfd, &rfds);
    FD_SET(sockfd, &wfds);

    while(true){
        rfds = wfds;

        if(select(maxSockfd + 1, &rfds, &wfds, NULL, &myTime) < 0) {
            perror("select");
            return -1;
        }

        if(FD_ISSET(sockfd, &rfds)){
            n = recvfrom(sockfd, buffer, MAXBUF, 0, ( struct sockaddr *) &nodeAddr, &len);
            buffer[n] = '\0';
            cout << "Node: ";
            for(int i = 0; i < n; i++){
                cout << buffer[i];
            }
            cout << endl;
        }
        string RouterChar = argv[1];
        string m = "Router " + RouterChar;
        const char* message = m.c_str();

        if(FD_ISSET(sockfd, &wfds)){
            for(itrAddr = linkedAddrs.begin(); itrAddr != linkedAddrs.end(); ++itrAddr){

                sendto(sockfd, (const char *)message, strlen(message), 0, (const struct sockaddr *) &itrAddr->second,  sizeof(itrAddr->second));

            }
        }
    }
*/





    string command;
    int commandNumber = 0;
    int runtimes = 0;










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
                          */


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
                      */
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
