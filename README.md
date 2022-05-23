# Table of Contents
<ul>
  <li><a href="#intro">Intro</a></li>
  <li><a href="#serverT">Server T</a></li>
  <li><a href="#serverS">Server S</a></li>
  <li><a href="#serverP">Server P</a></li>
  <li><a href="#central">Central Server</a></li>
  <li><a href="#clients">Clients</a></li>
  <li><a href="#run">Run Best Match</a></li>
</ul>

<div id="intro">
  <h1>Best Match</h1>
  Best Match is a simple terminal program written by Whaleiam that utilizes sockets in a Linux environment. The purpose of this project is to connect client programs to   server programs via socket communication to output a compatibility score between two users.

  <img src="https://github.com/whaleiam/Best-Match/blob/main/bestmatch.png" alt="Best Match Diagram"/>
  <br>
  <b>(Figure 1)</b>
  <br>
  <br>
  The diagram above showcases the mapped functionality of the Best Match program and will be explained in full detail in the sections below.
  <br>
  <br>
</div>


<div id="serverT">
  <h1>Server T</h1>

  <b>The Topology Server</b>
  <br>
  <span>
    The Topology Server, or Server T for short, is the sole server responsible for holding user data, and its respective user mappings. Server T has sole access to a text file, which holds pairings of names on each line. Each line represents an edge between two users at a given time, and the entire file itself, when profused into its respective topology, represents a network of users. Server T recieves two names from the <a href="central">Central Server</a>. Server T reads from the topography file and uses a set to store each line. T then takes one name as starting point, and finds the name within the set. The pair of names that T's dictionary provides is stored in a vector, waiting to be transmitted. Every subsequent name in the pair is then stored in the queue, and every instance of that name that occurs in the file is then stored into our vector. If by the end of the search, we have reached the second name, then we begin transmitting all the pairs of names to our <a href="central">Central Server</a>. If our search does not return the second name before transmission, the entire program is terminated, and the clients have a chance to re-enter a user name.
  </span>
</div>
<br>
<div id="serverS">
  <h1>Server S</h1>
  
  <b>The Score Server</b>
  <br>
  <span>
    The Score Server, a.k.a Server S, is the server responsible for accessing the scores of each edge. In our original topography, each edge represents a compatibility between a pair of users. The job of the Score Server is to provide each of these matchings with a compatibility score. Server S recieves the set of pairings from the <a href="central">Central Server</a>, the same set of pairs that is generated, and provided by <a href="serverT">Server T</a>. Server S runs through each pair of names, and for each pair, the server itself accesses a file, available only to server S, and updates each dictionary in the set with a compatibility score. Each edge should now have a score associated with it. The new set is then passed to the <a href="central">Central Server</a> via UDP connection.
  </span>
</div>
<br>
<div id="serverP">
  <h1>Server P</h1>
  
  <b>The Processing Server</b>
  <br>
  <span>
    Server P in this implementation is the processing server; taking all the edges and scores generated from <a href="serverT">Server T</a> and <a href="serverS">Server S</a> respectively, server P uses the given data to find the shortest relative gap given a starting name. The implementation uses Dijkstra's algorithm to find the shortest connection between two names of a weight graph. The Minheap.h class, defines the Min Binary heap that allows server P to traverse the graph in the most effecient way without being given any heuristic detail about each edge. The graph traversal using this algorithm should take O(E +VlogV), where E is the edges in the graph, and V are the individual names presented. Once server P finds the shortest path, it calculates a compatibility score using the weights of the shortest path, and retransmits that score back to the <a href="central">Central Server</a>.
  </span>
</div>
<br>
<div id="central">
  <h1>Central Server</h1>

  <b>The Central Server</b>
  <br>
  <span>
    The central focal point of all sub-programs: All servers communicate their results to the Central Server via UDP connections, and connect to the client programs via TCP connections. The sole responsibility of the Central Server is to relay each information properly to the individual working programs. The Central Server recieves a pair of names from the clients, and relays those names to <a href="serverT">Server T</a>. Once it recieves a topology from <a href="serverT">Server T</a>, the topology is sent to <a href="serverS">Server S</a> to retrieve the scores, and the final weighted graph is sent from Central, to <a href="serverP">Server P</a>. The final compatibility score is sent back to Central, where Central will then relay the scores back to the client programs.
  </span>
</div>
<br>
<div id="clients">
  <h1>Clients</h1>

  The two client programs share the same functionality. When the entire program is run, two client terminals will allow a user to input a specific name. That name will be relayed to the central server via a TCP connection. After some time, the two clients will both receive a compatibily score calculated using the two names.
</div>
<br>
<div id="run">
  <h1>Run the Program</h1>  
</div>
  In order to run the program, the following must be done: <br>
  Make sure that the provided **makefile** is in the same directory as the rest of the project. Then open 6 terminals running in a Linux environment. 
  <br>
  Run:
  ```
  make all
  ```
  in order to first create the object files of each client and server program. Then on each open terminal, run a single program instance with:
  ```
  ./central
  ./serverT
  ./serverS
  ./serverP
  ./clientA
  ./clientB
  ```
  Then in the terminals running the client program, type any two user names and you have successfully run the program.
  
  <br>
  <br>
  ## Note
  The graphs and scores text files can be modified to your liking.
