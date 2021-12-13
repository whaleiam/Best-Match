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
#define Central_ClientA_TCP_PORT 25888      // Central port number

// Main function
int main(int argc, char *argv[])
{
    struct sockaddr_in central;         //the sockaddr struct variable for the central server address

    // Exit the program if there are no usernames provided
    if(argc == 1)
    {
        printf("User did not input a username\n");
        exit(1);
    }

    // Create TCP stream socket
    int clientA_to_Central = socket(AF_INET, SOCK_STREAM, 0);

    // if unable to establish socket, output an error message and exit with return code 1
    if(clientA_to_Central < 0)
    {
        exit(1);    // If there is an error detected, exit the program
    }

    // Taken from Beej’s guide to network programming - 9.24. struct sockaddr and pals
    memset(&central, 0, sizeof(central));               // Dynamically allocate central server address
    central.sin_family = AF_INET;                       // Initialize to use IPv4
    central.sin_addr.s_addr = inet_addr(LOCAL);         // Internet address
    central.sin_port = htons(Central_ClientA_TCP_PORT); // Convert Central Server Port# to network short

    // If the connection fails, exit the program
    if (connect(clientA_to_Central, (struct sockaddr *) &central, sizeof(central)) < 0) {
        close(clientA_to_Central);    // Close the socket before exiting the program
        exit(1);                // If there is an error detected, exit the program
    }

    // Booting up message
    printf("The client is up and running\n");

    // Send the inputted name to the Central server
    char *sendName;
    sendName = argv[1];
    if (send(clientA_to_Central, sendName, sizeof(sendName), 0) < 0) {
        close(clientA_to_Central);    // Close the socket before exiting the program
        exit(1);                // If there is an error detected, exit the program
    }

    // This is just formatting the output to the terminal
    printf("The client sent %s to the Central Server\n", argv[1]);

    vector<string> path;    // Vector to hold all the names in the path
    bool compat = true;     // To see if a match is possible
    // Loop to receive the names
    while(true){
        char names[32];
        if (recv(clientA_to_Central, names, sizeof(names), 0) < 0) {
            close(clientA_to_Central);    // Close the socket before exiting the program
            exit(1);                // If there is an error detected, exit the program
        }
        string name = names;

        // To see if we get a delimiter from the central server
        if(name == "1"){
            break;
        }
        // If there was no compatibility possible, then break the loop
        if(name == "NA"){
            compat = false;     // Compatibility not possible
            break;
        }
        path.push_back(name);   // Push back the name into a vector
    }

    // Getting the last name to write to the terminal
    string lastName = path.back();

    // If there is a compatibility match
    if(compat){
        char dec[32];
        // Receive the compatibility score
        if (recv(clientA_to_Central, dec, sizeof(dec), 0) < 0) {
            close(clientA_to_Central);
            exit(1);
        }
        string decimal = dec;
        // Terminal message
        printf("Found compatibility for %s and %s:\n", sendName, lastName.c_str());

        // On the next line, format and output all the names in the path
        for(int i = 0; i < (int)path.size(); ++i){
            if(i != (int)path.size()-1){
                printf("%s --- ", path[i].c_str());
            }else{
                printf("%s\n", path[i].c_str());
            }
        }

        // Output the compatibility score
        double gap = stod(decimal, nullptr);
        printf("Compatibility score: %.2f\n", gap);
    }
    else{
        // Otherwise print that there is no compatibility
        printf("Found no compatibility for %s and %s\n", sendName, lastName.c_str());
    }

    close(clientA_to_Central);      // Close the socket
    return 0;
}