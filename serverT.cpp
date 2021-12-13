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
#include <vector>

#define LOCAL "127.0.0.1"                   // Local host address
#define ServerT_UDP 21888                   // Central to server T port number

using namespace std;

void sendLines(int &sockfd, string name, struct sockaddr_in &addr); // Function to send each line from the file

void sendLines(int &sockfd, string name, struct sockaddr_in &addr)
{
    socklen_t addr_size = sizeof(addr);
    char* msg = &name[0];
    if (sendto(sockfd, msg, sizeof(msg), 0, (struct sockaddr *) &addr, addr_size) < 0) {
        perror("[ERROR] Server A: fail to send write result to AWS server");
        exit(1);
    }
}

int main()
{
    int sockfdUDP;
    struct sockaddr_in serverT_addr, central_addr;
    ifstream topo;

    // Create socket
    sockfdUDP = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfdUDP < 0) {
        exit(1);
    }

    memset(&serverT_addr, 0, sizeof(serverT_addr));     //  make sure the struct is empty
    serverT_addr.sin_family = AF_INET;                  // Use IPv4 address family
    serverT_addr.sin_addr.s_addr = inet_addr(LOCAL);    // Host IP address
    serverT_addr.sin_port = htons(ServerT_UDP);         // Convert Central Server Port# to network short

    // Bind the socket
    if (::bind(sockfdUDP, (struct sockaddr *) &serverT_addr, sizeof(serverT_addr)) < 0) {
        exit(1);
    }
    // Terminal message
    printf("The Server T is up and running using UDP on port <%d>. \n", ServerT_UDP);

    // Same loop to receive the initial names to be looking for
    while(true){
        set<string> edges;
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
                edges.insert(nametoFind);
            }
        }

        // While looping, if the set of names isnt empty
        if(!edges.empty()){
            // Terminal Message
            printf("The ServerT received a request from Central to get the topology \n");
            topo.open("edgelist.txt");  // Open the file

            // Count how many lines there are
            size_t fileLines = count(istreambuf_iterator<char>(topo), istreambuf_iterator<char>(), '\n');
            int numLines = (int) fileLines + 1;
            set<int> lineNum;
            for(int i = 1; i <= numLines; ++i){
                lineNum.insert(i); // Use a set to determine which lines we have sent to ensure no repeats
            }

            string line;
            vector<string> leftOverNames;

            // Loop till the set is empty
            while(!edges.empty()){
                topo.clear();           // Go back to the first line of the file
                topo.seekg(0);
                string name1, name2;
                int j = 1;
                // Loop to get each line
                while(getline(topo, line)){
                    istringstream iss(line);
                    iss >> name1;
                    iss >> name2;

                    // First, did we already send this line?
                    if(lineNum.find(j) != lineNum.end()){
                        // If not, check to see if the names are part of the graph we want to send
                        if((edges.find(name1) != edges.end()) || (edges.find(name2) != edges.end())){
                            sendLines(sockfdUDP, name1, central_addr);
                            sendLines(sockfdUDP, name2, central_addr);

                            if(edges.find(name1) == edges.end()){
                                leftOverNames.push_back(name1);
                            }
                            else if(edges.find(name2) == edges.end()){
                                leftOverNames.push_back(name2);
                            }
                            lineNum.erase(j);   // Then erase line if sent
                        }

                    }
                    j++;
                }
                edges.erase(edges.begin(), edges.end()); // Refresh the set of names

                // All the leftovernames are used to populate the set
                for(int i = 0; i < (int)leftOverNames.size(); ++i){
                    edges.insert(leftOverNames[i]);
                    leftOverNames.erase(leftOverNames.begin()+i);
                }

            }

            // Terminal message
            printf("The ServerT finished sending the topology to Central\n");
            topo.close();
        }

    }

    close(sockfdUDP);

    return 0;
}

