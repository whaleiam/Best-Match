#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/select.h>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <set>
#include <sstream>
#include <string>

#define LOCAL "127.0.0.1"                   // Local host address
#define ServerS_UDP 22888                   // Central to server T port number

using namespace std;

int main()
{
    int sockfdUDP; // Datagram socket
    struct sockaddr_in serverS_addr, central_addr;
    ifstream scores;


    sockfdUDP = socket(AF_INET, SOCK_DGRAM, 0); // Create a UDP socket
    if (sockfdUDP < 0) {
        exit(1);
    }

    memset(&serverS_addr, 0, sizeof(serverS_addr));     //  make sure the struct is empty
    serverS_addr.sin_family = AF_INET;                  // Use IPv4 address family
    serverS_addr.sin_addr.s_addr = inet_addr(LOCAL);    // Host IP address
    serverS_addr.sin_port = htons(ServerS_UDP);         // Convert Central Server Port# to network short

    // Bind the socket
    if (::bind(sockfdUDP, (struct sockaddr *) &serverS_addr, sizeof(serverS_addr)) < 0) {
        exit(1);
    }

    // Terminal message
    printf("The Server S is up and running using UDP on port <%d>. \n", ServerS_UDP);

    // Same Loop (or similar at least) with that of the loop from this website
    // https://stackoverflow.com/questions/25888914/linux-c-socket-select-loop
    while(true){
        set<string> listofNames;
        int recVal = 0;
        struct timeval tv;
        tv.tv_usec = 0.0;
        tv.tv_sec = 1.0;
        int loopCount = 0;

        while(loopCount != 5){
            fd_set rfds;
            FD_ZERO(&rfds);
            FD_SET(sockfdUDP, &rfds);
            recVal = select(sockfdUDP+1, &rfds, NULL, NULL, &tv);

            if(recVal == 0){
                loopCount++;
            }
            else if(recVal < 0){
                exit(1);
            }
            else{
                char names[32];
                socklen_t addr_size = sizeof(central_addr);
                if(::recvfrom(sockfdUDP, &names, sizeof(names), 0, (struct sockaddr *) &central_addr, &addr_size) < 0){
                    exit(1);
                }
                string nametoFind = names;
                listofNames.insert(nametoFind);
            }
        }

        // Check to see if the set is empty
        if(!listofNames.empty()){
            printf("The ServerS received a request from Central to get the scores\n");      // Terminal message
            scores.open("scores.txt");  // Open up the file with the scores
            string line;
            // Get each line
            while(getline(scores, line)){
                istringstream iss(line);
                string name, score;
                iss >> name;
                iss >> score;
                // If the name exists from the set received from central
                if(listofNames.find(name) != listofNames.end()){
                    char *nametoSend, *scoretoSend;
                    nametoSend = &name[0];
                    scoretoSend = &score[0];
                    // Send both the name and the score
                    socklen_t addr_size = sizeof(central_addr);
                    if (sendto(sockfdUDP, nametoSend, sizeof(nametoSend),  0, (struct sockaddr *) &central_addr, addr_size) < 0){
                        exit(1);
                    }
                    if (sendto(sockfdUDP, scoretoSend, sizeof(scoretoSend),  0, (struct sockaddr *) &central_addr, addr_size) < 0){
                        exit(1);
                    }
                }
            }
            scores.clear();
            scores.seekg(0);    // Restart position to the top of the file
            scores.close();
            // Terminal message
            printf("The ServerS finished sending the scores to Central\n");
        }
    }

    close(sockfdUDP);

    return 0;
}
//cd Downloads/Unix_Sockets