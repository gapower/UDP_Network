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
#include "hostobj_Connected.h"

#define MAXBUF 1024

//More testing by Peter...
using namespace std;

string constructString(string this_host){
      string temp;
      string finalString;
      bool dv = false;
      cout << "Constructing Message to send...\n";
      cout << "Is this a Distance Vector? (yes/no)";
      getline(cin,temp);
      if (temp=="yes"){
        dv = true;
        temp="dv";
      }

      finalString += "type:";
      finalString += temp;
      finalString += "\n";
      temp = this_host;
      finalString += "src:";
      finalString += temp;//add source, senderName
      finalString += "\n"; //While testing, these didnt make sense, I think because there ended up being two \n s

      if(dv)
        cout << "please enter the distance vectors that\nyou would like to update in the form AB1 separated by a space:";
      else
        cout << "Please enter the string that you would like to send";

      getline(cin,temp);
      if(temp=="a")
      temp = "AB1 AB2 AB3 AB5";
      finalString += "data:";
      finalString += temp;
      //finalString += "\n";



      return finalString;

}

void messageReceived(string input){
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
    cout << "Type(yes or no): " << subString << endl;

    if(subString== "yes")
      dv=true;

    stringlength -= point2;

    tempInput = input.substr(point2+1,stringlength);

    temp= "src:";
    point1=tempInput.find(temp);
    point1=point1+temp.length();
    point2=tempInput.find("\n");

    subString=tempInput.substr(point1,point2-point1);

    cout << "Source node: "<< subString << endl;

    stringlength -= point2;

    tempInput = input.substr(point2+1,stringlength);


    temp= "data:";
    point1=tempInput.find(temp);
    point1=point1+temp.length();
    point2=tempInput.find("\n");

    subString=tempInput.substr(point1,point2-point1);

    cout << "Data: " << subString << endl;



}


//MAIN//
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


    //Code for reading through the xml file and inputting the data for the given
    //argument, i.e the Host name (A,B,C...)
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

    std::map< std::string, std::pair<int, int> > links = thisHost.getLinks();
    map< std::string, std::pair<int, int> >::iterator itr;
    map< std::string, struct sockaddr_in > linkedAddrs;
    map< std::string, struct sockaddr_in > ::iterator itrAddr;
    struct sockaddr_in nodeAddr;

    // initialising neighbouring port addresses
    for(itr = links.begin(); itr != links.end(); ++itr){
        memset(&nodeAddr, 0, sizeof(nodeAddr));

        nodeAddr.sin_family = AF_INET;
        nodeAddr.sin_port = htons(itr->second.first);
        if (inet_aton("127.0.0.1", &nodeAddr.sin_addr) == 0){
            fprintf(stderr, "Error when using 'inet_aton()'\n");
            exit(1);
        }

        linkedAddrs.insert(pair<string, struct sockaddr_in> (itr->first, nodeAddr));
    }













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

                    messageReceived(receivedString);
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
                    itrAddr = linkedAddrs.begin();
                    while(itrAddr != linkedAddrs.end()&& !desiredHostFound){

                        if((itrAddr->first) == desiredPort){
                          cout << "Host " << itrAddr->first << " found succesfully.\n";
                          desiredHostFound=true;
                        }
                        else {
                          ++itrAddr;
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
                      stringToSend = constructString(thisHost.getHostname());
                      cMessage = stringToSend.c_str();
                      //for(itrAddr = linkedAddrs.begin(); itrAddr != linkedAddrs.end(); ++itrAddr){

                          sendto(sockfd, (const char *)cMessage, strlen(cMessage), 0, (const struct sockaddr *) &itrAddr->second,  sizeof(itrAddr->second));

                      //}

                  }

                }
                else if(commandNumber== 3)
                break;


      //  runtimes++;
  }//End of running process






/*




        UDP message

        string Sender
        //int senderPortNumber
        string messageType; ("DV" or "DATA")
        optional:
        string data
        string destinationNode;
        string weight;

        string messageObj (string messageType,string Sender, string data){
          if (messageType == "DV")
      }





      void messageReceived(string input){
      int point1,point2,requiredLength;
      string temp,subString,tempInput;
      bool dv = false;
      string sourceNode;


      tempInput = input;

      int stringlength=tempInput.length();

      temp= "type:";
      point1=tempInput.find(temp);
      point1=point1+temp.length();
      point2=tempInput.find("\n")

      subString=tempInput.substr(point1,point2-point1);
      cout << "Type(yes or no): " << subString << endl;

      if subString = "yes"
        dv=true;

      stringlength -= point2;

      tempInput = input.substr(point2+1,stringlength);

      temp= "src:";
      point1=tempInput.find(temp);
      point1=point1+temp.length();
      point2=tempInput.find("\n")

      subString=tempInput.substr(point1,point2-point1);

      cout << "Source node: "<< subString << endl;

      stringlength -= point2;

      tempInput = input.substr(point2+1,stringlength);


      temp= "data:";
      point1=tempInput.find(temp);
      point1=point1+temp.length();
      point2=tempInput.find("\n")

      subString=tempInput.substr(point1,point2-point1);

      cout << "Data: " << subString << endl;



    }


      find()///The position of the first character of the first match.



        upon receipt of a dv message print time to console


        //fucntion that will send fwding table to all neighbours.. so it can be called periodically

        string.substr(starting number location, plus this many characters);

    cout << endl;
    thisHost.printLinks();

    cout << endl << "ORIGINAL TABLE NO ALTERATIONS" << endl;
    thisHost.printTable();

    // Simulate an update message
    cout << endl << "TABLE WITH INCOMING DATA" << endl;
    thisHost.updateTable("B","A", 4);
    thisHost.updateTable("B","C", 3);
    thisHost.updateTable("B","E", 2);
    thisHost.updateTable("B","F", 1);

    thisHost.updateTable("E","A", 1);
    thisHost.updateTable("E","B", 2);
    thisHost.updateTable("E","F", 3);

    thisHost.printTable();

    cout << endl << "TABLE REGENERATED TO ACCOMODATE DATA" << endl;
    thisHost.reGenTable();
    thisHost.printTable();
    cout << endl;
*/
    datafile.close();

	return 0;
}
