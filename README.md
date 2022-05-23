# Table of Contents
<ul>
  <li><a href="#intro">Intro</a></li>
  <li><a href="#clients">Clients</a></li>
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

<div id="clients">
  <h1>Clients</h1>

  The two client programs share the same functionality. When the entire program is run, two client terminals will allow a user to input a specific name. That name will be relayed to the central server via a TCP connection. After some time, the two clients will both receive a compatibily score calculated using the two names.
</div>
<br>
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
  
</div>
