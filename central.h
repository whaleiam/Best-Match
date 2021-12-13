#ifndef EE450_PROJECT_CENTRAL_H
#define EE450_PROJECT_CENTRAL_H
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
#include <set>
#include <sstream>
#include <string>
#include <iterator>
#include <iostream>
#include <vector>

#define LOCAL "127.0.0.1"                   // Local host address
#define Central_UDP 24888                   // Central port number
#define Central_ClientA_TCP_PORT 25888      // Central to client A port number
#define Central_ClientB_TCP_PORT 26888      // Central to client B port number
#define ServerT_UDP 21888                   // Central to server T port number
#define ServerS_UDP 22888                   // Central to server S port number
#define ServerP_UDP 23888                   // Central to server P port number

// Begin implementation for the Socket Object Class
class SocketObj{
public:
    // Function to create the socket
    void createSocket(int &sockfd, struct sockaddr_in &central_addr, unsigned short portNumber)
    {
        port = portNumber;              // Variable to hold the port Number
        this->socketType(sockfd);    // Use a virtual function to create socket

        if(sockfd < 0){
            exit(1);    // If there is an error detected, exit the program
        }

        memset(&central_addr, 0, sizeof(central_addr));     // Dynamically allocate central server address
        central_addr.sin_family = AF_INET;                  // Initialize to use IPv4
        central_addr.sin_addr.s_addr = inet_addr(LOCAL);    // Internet address
        central_addr.sin_port = htons(port);                // Convert Central Server Port# to network short

        // Bind the socket
        if (::bind(sockfd, (struct sockaddr *) &central_addr, sizeof(central_addr)) < 0) {
            exit(1);  // If there is an error detected, exit the program
        }
    }
protected:
    unsigned short port;    // Protected member

private:
    // Since inheriting client class will need to change this to Sock_stream, leave it virtual
    virtual void socketType(int &sockfd){
        sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    }
};


// Client socket object implementation inherits from the socket object above
class ClientSock : public SocketObj{
public:
    // Function to listen
    void listenPort(int& sockfd){
        if (listen(sockfd, 10) < 0) {
            exit(1);    // If there is an error detected, exit the program
        }
    }
    // Function to accept child socket (Referenced from Beej's Guide)
    void acceptCon(int &sockfd, int &child_sockfd, struct sockaddr_in &child_addr){
        socklen_t addr_size = sizeof(child_addr);
        child_sockfd = ::accept(sockfd, (struct sockaddr *) &child_addr, &addr_size);
        if (child_sockfd < 0) {
            exit(1);    // If there is an error detected, exit the program
        }
    }
    // Function to receive the name inputted from the clients
    void receive(int &child, char *name){
        if (recv(child, name, 32, 0) < 0) {
            exit(1);    // If there is an error detected, exit the program
        }
        // Terminal message
        printf("The Central server received input=\"%s\" from the client using TCP over port <%d> \n", name, port);
    }
    // Function to send the processed results back to the client
    void sendResults(int &child_sockfd, std::vector<std::string> results, char server, struct sockaddr_in &addr, std::string start, std::string end){
        // If server P sent NA, that means there were no possible connections
        if(results[0] == "NA"){
            helperSender(child_sockfd, end, addr);          // Send the destination name
            helperSender(child_sockfd, results[0], addr);   // Send the NA message
        }
        // These if statements control how the path is sent given the which clients they are sent to
        if (server == 'A'){
            for(int i = (int)results.size()-2; i >= 0; --i){
                helperSender(child_sockfd, results[i], addr);   // Use helper function to send each name at a time
            }

        }
        if(server == 'B'){
            for(int i = 0; i < (int)results.size() - 1; ++i){
                helperSender(child_sockfd, results[i], addr);   // Use helper function to send each name at a time
            }
        }
        helperSender(child_sockfd, "1", addr);        // Send a 1 to signify we are done transmitting names
        std::string compute = results[results.size()-1];        // Send the compatibility score
        helperSender(child_sockfd, compute, addr);
        printf("The Central server sent the results to client %c\n", server);  // Terminal message
    }

private:
    // Helper function to send a word at a time to the client
    void helperSender(int &child_sockfd, std::string msg, struct sockaddr_in &addr){
        char* out;
        out = &msg[0];
        if (sendto(child_sockfd, out, sizeof(out), 0, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
            exit(1);
        }
    }
    // Function to creat the socket
    void socketType(int &sockfd){
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
    }
};

// Server Object
class Server{
public:
    // Constructor to initialize a few data members
    Server(int &sock, struct sockaddr_in &addr, unsigned short port) : sockfd(sock), dest_addr(addr), portNumber(port){
        this->initialize_Server();  // Initialize the server
    }
    ~Server(){}
    // Use this function to request from central
    void request(std::set<std::string> &listOfNames, std::string op, char server){
        std::set<std::string>::iterator it;     // Use a set to send the list of names to the servers to get specific data
        for(it = listOfNames.begin(); it != listOfNames.end(); ++it){
            std::string temp = *it;
            sendHelper(temp);   // Use helper function to retrieve the data
        }
        // Terminal message
        printf("The Central server sent a %s to Backend-Server %c\n", op.c_str(), server);
    }
    // https://stackoverflow.com/questions/25888914/linux-c-socket-select-loop <- referenced this website
    // for this select() method (as well as Beej's guide)
    void receive(std::vector<std::string> &fileLines, std::set<std::string> &list, char server) {
        int recVal = 0, countLoop = 0;
        struct timeval tv;
        tv.tv_usec = 0.0;
        tv.tv_sec = 1.0;
        while (fileLines.empty()) {
            while (countLoop != 5) {
                fd_set rfds;
                FD_ZERO(&rfds);
                FD_SET(sockfd, &rfds);
                recVal = select(sockfd + 1, &rfds, NULL, NULL, &tv);

                if (recVal == 0) {
                    countLoop++;
                } else if (recVal < 0) {
                    exit(1);
                } else {
                    char msg1[32], msg2[32];
                    socklen_t addr_size = sizeof(dest_addr);
                    // Receive two messages at a time
                    // If it's from server S, get the name and the score
                    // server T, get two names at once
                    if (::recvfrom(sockfd, &msg1, sizeof(msg1), 0, (struct sockaddr *) &dest_addr, &addr_size) < 0) {
                        exit(1);
                    }
                    if (::recvfrom(sockfd, &msg2, sizeof(msg2), 0, (struct sockaddr *) &dest_addr, &addr_size) < 0) {
                        exit(1);
                    }
                    std::string msgOne = msg1, msgTwo = msg2;
                    std::string fullLine = msgOne + " " + msgTwo;   // Append the two strings to get the full line
                    fileLines.push_back(fullLine);                  // Push back into a vector
                    if(server == 'T'){
                        list.insert(msgOne);        // Since we get the names first from server T it's important
                        list.insert(msgTwo);        // that we get all the names associated with a given graph
                    }
                }
            }
        }
        // Terminal Print
        printf("The Central server received information from Backend-Server %c using UDP over port <%d>\n", server, portNumber);
    }
    // Function to send all the data for server P to process
    void process(std::string start, std::string end, std::vector<std::string> &top, std::vector<std::string>&score){
        // First send the start and end string
        sendHelper(start);
        sendHelper(end);
        vectorHelper(score);
        sendHelper("END");  // This signifies the end of sending the scores
        sendHelper("END");
        vectorHelper(top);
        // Terminal message
        printf("The Central server sent a processing request to Backend-Server P \n");
    }
    // function to retrieve the processed info -> Once again, referenced
    // https://stackoverflow.com/questions/25888914/linux-c-socket-select-loop
    void processedInfo(std::vector<std::string> &result){
        int countLoop = 0;
        int recValue;
        struct timeval tv;
        tv.tv_usec = 0.0;
        tv.tv_sec = 1.0;
        while (result.empty()) {
            while (countLoop != 10000) {
                fd_set rfds;
                FD_ZERO(&rfds);
                FD_SET(sockfd, &rfds);
                recValue = select(sockfd + 1, &rfds, NULL, NULL, &tv);

                if (recValue == 0) {
                    countLoop++;
                    if(!result.empty()){
                        break;
                    }
                } else if (recValue < 0) {
                    exit(1);
                } else {
                    char msg1[32];
                    socklen_t addr_size = sizeof(dest_addr);
                    if (::recvfrom(sockfd, &msg1, sizeof(msg1), 0, (struct sockaddr *) &dest_addr, &addr_size) < 0) {
                        exit(1);
                    }
                    std::string msgOne = msg1;
                    result.push_back(msgOne);
                }
            }
        }
        // Terminal message
        printf("The Central server received the results from backend server P\n");
    }
private:
    int &sockfd;
    struct sockaddr_in &dest_addr;
    unsigned short portNumber;
    // Function to initialize ther server
    void initialize_Server(){
        memset(&dest_addr, 0, sizeof(dest_addr));       // Dynamically allocate central server address
        dest_addr.sin_family = AF_INET;                 // Initialize to use IPv4
        dest_addr.sin_addr.s_addr = inet_addr(LOCAL);   // Internet address
        dest_addr.sin_port = htons(portNumber);         // Convert Central Server Port# to network short
    }
    // Helper function to send a word
    void sendHelper(std::string msg1){
        char* msg = &msg1[0];
        if (sendto(sockfd, msg, sizeof(msg), 0, (const struct sockaddr *) &dest_addr, sizeof(dest_addr)) < 0) {
            exit(1);
        }
    }
    // Helper function to separate a full string into two words
    void vectorHelper(std::vector<std::string> &msg){
        for(std::vector<std::string>::iterator it = msg.begin(); it != msg.end(); ++it){
            std::istringstream iss(*it);
            std::string msg1, msg2;
            iss >> msg1;
            iss >> msg2;
            sendHelper(msg1);
            sendHelper(msg2);
        }
    }
};
// End of Server Class
#endif //EE450_PROJECT_CENTRAL_H