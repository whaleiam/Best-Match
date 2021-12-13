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
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

// Define constants
#define LOCAL "127.0.0.1"                   // Host address
#define Central_ClientB_TCP_PORT 26888      // Central port number

// Main function
int main(int argc, char *argv[])
{
    struct sockaddr_in central; //the sockaddr struct variable for the central server address


    // Exit the program if there are no usernames provided
    if(argc == 1)
    {
        printf("User did not input a username\n");
        exit(1);
    }

    string name = argv[1];  // String Variable holds the first username

    // Create TCP stream socket
    int clientB_to_Central = socket(AF_INET, SOCK_STREAM, 0);

    // if unable to establish socket, exit with return code 1
    if(clientB_to_Central < 0)
    {
        exit(1);
    }

    // Taken from Beej’s guide to network programming - 9.24. struct sockaddr and pals
    memset(&central, 0, sizeof(central));               // Dynamically allocate central server address
    central.sin_family = AF_INET;                       // Initialize to use IPv4
    central.sin_addr.s_addr = inet_addr(LOCAL);         // Internet address
    central.sin_port = htons(Central_ClientB_TCP_PORT); // Convert Central Server Port# to network short

    // If the connection fails, exit the program
    if (connect(clientB_to_Central, (struct sockaddr *) &central, sizeof(central)) < 0) {
        close(clientB_to_Central);
        exit(1);
    }

    // Booting up message
    printf("The client is up and running\n");

    // Send the inputted name to the Central server
    char *sendName;
    sendName = argv[1];
    if (send(clientB_to_Central, sendName, sizeof(sendName), 0) < 0) {
        close(clientB_to_Central);
        exit(1);
    }

    // This is just formatting the output to the terminal
    printf("The client sent %s to the Central Server\n", argv[1]);

    // Everything is mostly identical to clientA with a few formatting turnarounds
    vector<string> path;
    bool compat = true;
    while(true){
        char names[32];
        if (recv(clientB_to_Central, names, sizeof(names), 0) < 0) {
            close(clientB_to_Central);
            exit(1);
        }
        string name = names;
        if(name == "1"){
            break;
        }
        if(name == "NA"){
            compat = false;
            break;
        }
        path.push_back(name);
    }

    string firstName = path.back();
    if(compat){
        char dec[32];
        if (recv(clientB_to_Central, dec, sizeof(dec), 0) < 0) {
            close(clientB_to_Central);
            exit(1);
        }
        string decimal = dec;
        printf("Found compatibility for %s and %s:\n", firstName.c_str(), sendName);
        for(int i = 0; i < (int)path.size(); ++i){
            if(i != (int)path.size()-1){
                printf("%s --- ", path[i].c_str());
            }else{
                printf("%s\n", path[i].c_str());
            }
        }
        double gap = stod(decimal, nullptr);
        printf("Compatibility score: %.2f\n", gap);
    }
    else{
        printf("Found no compatibility for %s and %s\n", firstName.c_str(), sendName);
    }

    close(clientB_to_Central);      // Close the socket
    return 0;
}