#include "central.h"

using namespace std;

int main()
{
    int sockfdclientA_TCP, sockfdclientB_TCP, sockfdUDP;     // Parent socket variables for both TCP connections and one UDP connection
    int clientA_child, clientB_child;                // Child socket variables for connecting to either clients

    // Transport address for when central acts as a server
    struct sockaddr_in clientA_central_addr, clientB_central_addr, UDP_central_addr;
    // Transport address for when central acts as a client (central_clientA_addr, central_clientB_addr)
    struct sockaddr_in central_T_addr, central_S_addr, central_P_addr, central_clientA_addr, central_clientB_addr;

    // Initialize ClientA and ClientB as objects
    ClientSock ClientA, ClientB;

    // Create the client sockets and allow incoming connection
    ClientA.createSocket(sockfdclientA_TCP, clientA_central_addr, Central_ClientA_TCP_PORT);
    ClientA.listenPort(sockfdclientA_TCP);
    ClientB.createSocket(sockfdclientB_TCP, clientB_central_addr, Central_ClientB_TCP_PORT);
    ClientB.listenPort(sockfdclientB_TCP);

    // Create UDP Socket Object
    SocketObj UDP;
    UDP.createSocket(sockfdUDP, UDP_central_addr, Central_UDP);

    printf("The Central server is up and running \n");  // Boot up message

    // Infinite loop
    while(true)
    {
        char startName[32], endName[32];

        // Accept connections for Client A and B
        ClientA.acceptCon(sockfdclientA_TCP, clientA_child, central_clientA_addr);
        ClientB.acceptCon(sockfdclientB_TCP, clientB_child, central_clientB_addr);

        // Retrieve the start and end names from Client A and Client B respectively
        ClientA.receive(clientA_child, startName);
        ClientB.receive(clientB_child, endName);

        // Create and initialize server objects
        Server T(sockfdUDP, central_T_addr, ServerT_UDP);
        Server S(sockfdUDP, central_S_addr, ServerS_UDP);
        Server P(sockfdUDP, central_P_addr, ServerP_UDP);

        string start = startName, end = endName;

        set<string> usernames;              // Create a set which contains the names within a given graph
        usernames.insert(start);            // Insert the start and end names into the set
        usernames.insert(end);

        // Retrieve the topology from server T
        T.request(usernames, "request", 'T');
        vector<string> topography;
        T.receive(topography, usernames, 'T');

        // Retrieve the scores from Server S
        S.request(usernames, "request", 'S');
        vector<string> scores;
        S.receive(scores, usernames, 'S');

        // Request for the processing calculation from server P
        P.process(start, end, topography, scores);
        vector<string> results;
        P.processedInfo(results);

        // Send the processed results over to the Clients
        ClientA.sendResults(clientA_child, results, 'A', central_clientA_addr, start, end);
        ClientB.sendResults(clientB_child, results, 'B', central_clientB_addr, start, end);

    }

    // Close the sockets
    close(sockfdclientA_TCP);
    close(sockfdclientB_TCP);
    close(sockfdUDP);
    return 1;
}
