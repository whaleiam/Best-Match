# EE 450 Socket Programming

Name: William Chung <br>
ID: 9797191888

##What I Have Done (SUMMARY)
In this sockets programming assignment, I have completed all the necessary components up to phase 3. The two client programs take in a name from the command line, and are sent to directly to the central server. Once arrived, the central server will first contact server T to gather the graph that includes the provided names. Once complete, the central server looks at the names in the graph, and requests the scores of the users in the graph from server S. With all the available information, the central server then requests server P to process and calculate the compatibility match between the given user-names as wells as the data provided from the two preceding servers. 

## <central.cpp central.h>
The central header file, as well as the central c++ file are both responsible for the functionality of the central server. The main takeaway is that the header files defines the various objects that are used to help manage the many sockets in this assignment. Otherwise, the functionality is pretty straightforward. ***NOTE...when testing the central server, I noticed that trying to send in large buffers through the sockets seemed to heavily affect the contents of the message, giving out garbage values. In order to remediate this, I had implemented the select function with a time delay between each read of the file descriptor, hence, there will be a visible delay between contacting each server**.

##<clientA.cpp clientB.cpp>
Pretty straightforward explanation. These two files handle the TCP connection of the two clients and passes in the name given from the command line. Nothing else is to be reported.


##<serverS.cpp & serverT.cpp>
Here, I use the unique implementation of reading the names into a set and reading each file depending on their status in the set. This is an important implementation due to the fact that the set gives an O(logn) runtime when traversing through the file, hence the runtime of the two programs, given a large input size and graph, will always be O(nlogn). Here the n represents the number of contents of the two txt files that hold the information to our graph and the scores. The majority of the delay comes from the select method used to read from the file descriptor on both ends of server communication. 

##<serverP.cpp minheap.h>
ServerP in this implementation is the processing server; taking all the edges and scores from server T and server S respectively, server P uses the given data to find the shortest relative gap given the starting name from clientA. The implementation uses Dijkstra's algorithm to find the shortest connection between two names of a weight graph. The Minheap.h class, defines the Min Binary heap that allows server P to traverse the graph in the most effecient way without being given any heuristic detail about each edge. the graph traversal using this algorithm should take O(E +VlogV), where E is the edges in the graph, and V are the individual names presented.

###Implementations
Much of the code written across these files are heavily guided from various sources. These include sample sources of code from Beej's Guide to Socket Programming, StackOverflow, geeksforgeeks, as well as other c++ reference modules on the internet. The MinHeap implementation is mine truly, as it is the same copy of a programming assignment I had turned in for CSCI104 here at USC. Whilst, there are some other sources that were utilized, none were as heavily referenced to as the sources provided in this statement.
