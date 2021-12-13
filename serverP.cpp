#include "minheap.h"
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
#include <bits/stdc++.h>
#include <cmath>
#include <map>
#include <set>
#include <string>
#include <iterator>
#include <iostream>
#include <vector>

#define LOCAL "127.0.0.1"                   // Local host address
#define ServerP_UDP 23888                   // Central to server T port number

using namespace std;

void msgHelper(int &sock, struct sockaddr_in &addr, string &msg);   // Function with receiving a message
void sendMsg(int &sock, string msg, struct sockaddr_in &addr);      // Function to send a message
double round(double x);                                             // Function to round to the nearest hundredth (referenced from geeksforgeeks)

void msgHelper(int &sock, struct sockaddr_in &addr, string &msg){
    char buf[32];
    socklen_t addr_size = sizeof(addr);
    if(::recvfrom(sock, &buf, sizeof(buf), 0, (struct sockaddr *) &addr, &addr_size) < 0){
        exit(1);
    }
    msg = buf;
}

void sendMsg(int &sock, string msg, struct sockaddr_in &addr){
    char *msgSend = &msg[0];
    socklen_t addr_size = sizeof(addr);
    if (::sendto(sock, msgSend, sizeof(msgSend),  0, (struct sockaddr *) &addr, addr_size) < 0){
        exit(1);
    }
}

double round(double x)
{
    return floor(x * 100 + 0.5) / 100;
}

// Main function
int main()
{
    int sockfdUDP;   // Datagram socket ()
    struct sockaddr_in serverP_addr, central_addr;

    sockfdUDP = socket(AF_INET, SOCK_DGRAM, 0); // Create a UDP socket
    if (sockfdUDP < 0) {
        exit(1);    // Exit the program if the socket can't be created
    }

    memset(&serverP_addr, 0, sizeof(serverP_addr));     // make sure the struct is empty
    serverP_addr.sin_family = AF_INET;                  // Use IPv4 address family
    serverP_addr.sin_addr.s_addr = inet_addr(LOCAL);    // Host IP address
    serverP_addr.sin_port = htons(ServerP_UDP);         // Convert Central Server Port# to network short

    // Bind the socket
    if (::bind(sockfdUDP, (struct sockaddr *) &serverP_addr, sizeof(serverP_addr)) < 0) {
        exit(1);
    }
    // Terminal message
    printf("The Server P is up and running using UDP on port <%d>. \n", ServerP_UDP);


    while(true){
        MinHeap<string> dijkstra(2);    // Min heap for implementing Dijkstra's algorithm
        vector<vector<string>> graph;     // Vector of vectors to represent the graph in 2d
        set<string> list;                 // A set of names (will come in handy for Dijkstra's)
        map<string, double> scores;       // A map to store the name and score pairs

        // Receive the start name and end name
        string startName, endName;
        msgHelper(sockfdUDP, central_addr, startName);
        msgHelper(sockfdUDP, central_addr, endName);

        // push an initial vector in to not cause problems
        if(1){
            vector<string> temp;
            graph.push_back(temp);
        }

        // Receive the scores first
        bool scoreLoop = true;
        // Using the reference to the select() method from
        // https://stackoverflow.com/questions/25888914/linux-c-socket-select-loop
        while(scoreLoop){
            int recVal = 0;
            struct timeval tv;
            tv.tv_usec = 0.0;
            tv.tv_sec = 1.0;
            int loopCount = 0;

            while(loopCount != 5)
            {
                fd_set rfds;
                FD_ZERO(&rfds);
                FD_SET(sockfdUDP, &rfds);
                recVal = select(sockfdUDP+1, &rfds, nullptr, nullptr, &tv);

                if(recVal == 0){
                    loopCount++;
                }
                else if(recVal < 0){
                    exit(1);
                }
                else{
                    string msg1, msg2;
                    msgHelper(sockfdUDP, central_addr, msg1);
                    msgHelper(sockfdUDP, central_addr, msg2);

                    // If we receive the 2 END's, we have finished receiving the scores
                    if(msg1 == "END" && msg2 == "END"){
                        scoreLoop = false;
                        break;
                    }
                    list.insert(msg1);      // Insert each name into a set
                    scores.insert(pair<string, double>(msg1, stof(msg2)));  // Insert each name and score pair into a map
                }
            }
        }

        graph[0].push_back(startName); // Push back the starting nam
        // Receive the topology (Same loop reference by this site)
        // https://stackoverflow.com/questions/25888914/linux-c-socket-select-loop
        while(true){
            int recVal = 0;
            struct timeval tv;
            tv.tv_usec = 0.0;
            tv.tv_sec = 1.0;
            int loopCount = 0;

            while(loopCount != 5)
            {
                fd_set rfds;
                FD_ZERO(&rfds);
                FD_SET(sockfdUDP, &rfds);
                recVal = select(sockfdUDP+1, &rfds, nullptr, nullptr, &tv);

                if(recVal == 0){
                    loopCount++;
                }
                else if(recVal < 0){
                    exit(1);
                }
                else{
                    string name1, name2;
                    msgHelper(sockfdUDP, central_addr, name1);
                    msgHelper(sockfdUDP, central_addr, name2);

                    // Insert the two names into the graph
                    bool name1Found = false, name2Found = false;
                    for(int i = 0; i < (int)graph.size(); ++i){
                        // If the name is already in the graph
                        if(name1 == graph[i][0]){
                            name1Found = true;
                            // Simply append the matrix to add another connected node in
                            if(name2 != startName){
                                // Then push the name back specifically for name1
                                graph[i].push_back(name2);
                            }
                        }
                        // Repeat with name 2
                        if(name2 == graph[i][0]){
                            name2Found = true;
                            if(name1 != startName){
                                graph[i].push_back(name1);
                            }
                        }
                    }
                    // If the names weren't found, add them into the vector
                    if(!name1Found){
                        vector<string> tempName1;
                        tempName1.push_back(name1);
                        graph.push_back(tempName1);
                    }
                    if(!name2Found){
                        vector<string> tempName2;
                        tempName2.push_back(name2);
                        graph.push_back(tempName2);
                    }
                }
            }
            break;
        }

        // Terminal message
        printf("The ServerP received the topology and score information\n");

        // Dijkstra's algorithm
        dijkstra.add(startName, 0.0f);  // Insert the starting Name with the value of 0 into the heap
        map<string, pair<string, double>> level;   // Use this map to keep track of the values, and the previous name
        level.insert(pair<string, pair<string, double>>(startName, pair<string, double>("", 0.0f)));

        // Loop until the heap is empty
        while(!dijkstra.isEmpty()){
            // Get the values from the top of the heap then remove it
            string topOfHeap = dijkstra.peek();
            double minVal = dijkstra.peekVal();
            dijkstra.remove();

            // If the top of the heap was the ending node, then break because we found the path
            if(topOfHeap == endName){
                list.erase(topOfHeap);
                break;
            }

            // If the name is still in the heap, then we haven't visited it
            if(list.find(topOfHeap) != list.end()){
                int i;
                // search the graph to find the name's other connected nodes
                for(i = 0; i < (int)graph.size(); ++i){
                    if(graph[i][0] == topOfHeap){
                        break;
                    }
                }
                //
                double currScore = scores.find(topOfHeap)->second;
                for(int j = 1; j < (int)graph[i].size(); ++j){
                    string tempName = graph[i][j];
                    // Check and see if this node has already been visited
                    if(list.find(tempName) != list.end()){
                        double nextScore = scores.find(tempName)->second;

                        double connection = abs(currScore-nextScore)/(currScore+nextScore);     // |a-b|/(a+b)
                        double total = minVal+connection;   // Add it to the previous value of the previous name

                        map<string, pair<string,double>>::iterator it = level.find(tempName);
                        // If it hasn't been added to the map to help us keep track, then either insert it,
                        // or update its values
                        if(it != level.end()){
                            double temp = it->second.second;

                            if(temp > total){
                                it->second.first = topOfHeap;
                                it->second.second = total;
                            }
                        }
                        else{
                            level.insert(pair<string, pair<string, double>>(tempName, pair<string, double>(topOfHeap, total)));
                        }
                        dijkstra.add(tempName, total);  // Add to the heap
                    }
                }
                list.erase(topOfHeap);  // Erase the node we just visited from the set
            }
        }

        // If we realize that the end name hasn't been visited, then there is no path
        if(list.find(endName) != list.end()){
            string NA = "NA";       // Send NA
            sendMsg(sockfdUDP, NA, central_addr);
        }
        // Else use the map and retrace the steps we took to get to the end and send that path
        else{
            map<string, pair<string,double>>::iterator it = level.find(endName);
            string path = endName;
            double x = round(it->second.second);
            string compatibility = to_string(x);
            while(path != ""){
                sendMsg(sockfdUDP, path, central_addr);
                path = it->second.first;
                it = level.find(path);
            }
            sendMsg(sockfdUDP, compatibility, central_addr);
        }
        // Terminal message
        printf("The ServerP finished sending the results to the Central\n");

    }
    close(sockfdUDP);
    return 0;
}