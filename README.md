# Berkeley’s Algorithm and Totally Ordered Messages

[Berkeley's Algorithm](https://en.wikipedia.org/wiki/Berkeley_algorithm)

Part 1
======

Implementation
--------------

The idea of part 1 is to implement Berkeley’s algorithm for Time Synchronization between the difference processes in the network. We have made use of TCP/IP for the purpose.
For the project following are the considerations made:
1.	The number of processes in the network are set as command line arguments. The number of nodes have been keep constant for testing as mentioned in the project description file.
2.	A separate code has been written for the daemon process, which will poll the other nodes in the network. The other nodes in the network are supplied as command line arguments.
3.	The other nodes in the network take 2 port arguments with, argument 1 is always the self-port (port of the node), and the second node is the server node. 
4.	The daemon process starts the polling, the server waits for all the nodes to get connected to it, and then sends out its server time to all the nodes in the network.
5.	Upon receiving the server time, the nodes will calculate the offset from its time and server time and send out the offset to the server.
6.	The server receives the offset, waits for offset from all nodes and then calculates the average time. A separate thread will calculate the average and send messages.
7.	The server then adjust itself to the average time and sends out the adjust time for each process in the network.
8.	Upon receiving the adjusting time, the processes adjust and print their new time.

>Learning from the project – Part1
>=================================
>1.	To implement multicasting in TCP/IP
>2.	IPC and leader election
>3.	To implement Berkeley’s algorithm
>4.	To understand how to implement time synchronization in case of distributed systems.

>#####Issues Faces
>1.	Multicasting in case of TCP/IP

Part 2
======

Total Ordered Messaging
-----------------------

You may like to read [Lamport Timestamps](https://en.wikipedia.org/wiki/Lamport_timestamps) before reading Total Ordered Messages.

The idea of the part 2 of the project is to work on the totally ordered messages in a distributed system where multicasting is done. For comparing the non-totally ordered messages and totally-ordered messages, two separate programs have been written.
Implementation – Non-Totally Ordered Messages
For the project following are the considerations made:
1.	The number of processes in the network are 4. The number of nodes have been keep constant for testing as mentioned in the project description file.
2.	A single program server.cpp has been written wherein the nodes in the network are provided as command line arguments. The first argument being the self-port (port on which the node will be communicating) and following being the nodes in the network.
3.	The non-totally ordered implementation follows the FIFO messaging wherein the processes print out their messages as soon as it is received. 
4.	Each process sends out a message using a sender thread and receives the messages using the receiver thread.
5.	Log files have been generated for each process, and we can view the ordering of messages being delivered.
6.	Observation – The process state during each message received is inconsistent, i.e. the processes are not in synchronized state when it comes to the messages being received.

Implementation – Totally Ordered Messages
>========================================

For the project following are the considerations made:
1.	The number of processes in the network are 4. The number of nodes have been keep constant for testing as mentioned in the project description file.
2.	For total-ordering of messages, sequencer algorithm has been used.
3.	Two programs have been written to implement the totally-ordering of messages. The sequencer program and the multicasting program. The multicasting program will send its messages to all the nodes in the network as well as to the sequencer program.
4.	The sequencer program has a global-sequence number (initially set to 0). Upon receiving a message, the sequencer will append the global-sequence number to the message and multicast to the other processes in the node.
5.	The other nodes in the network receive two types of messages, one being the message from the other process and one being the sequencer sent message. The processes will different the message type based on the second parameter it receives.
6.	If the second parameter is null (i.e. does not contain a global sequence number) it will add that message to its message buffer, while if it contains a second parameter, it will check it with its local sequence number and print it out.
7.	Log files have been generated for each process, and we can view the ordering of messages being delivered.
8.	Observation – The process state during each message received is consistent, i.e. the processes are in synchronized state when it comes to the messages being received. The message buffer ensures that the messages are not dropped on receiving.

>Learning from part 2
>====================

>1.	Totally ordering of messages
>2.	Multicasting in case of TCP/IP
>3.	Sequencer algorithm to achieve total-ordering of messages.

>P.S. -  An attempt has been made to make use of locks that acquire and release on a shared file, to get the counter. The program however needs to be tested for completeness. (File can be read – multicasting_with_locks.cpp).


HOW TO RUN THE PROGRAMS
======================

PART 1
======

Run the following command in the make file
| Command       | Description                                         |
| ------------- |:---------------------------------------------------:|
| make compile	| To compile both the client and the server programs. |
| make run	    | To run the daemon process program.                  |
| make runp1    | To run process 1 program in the network             |
| make runp2    | To run process 2 program in the network             |
| make clean    |	To remove the binary files in the directory.        |


PART 2 – Non- Totally Ordered Messages.
=======================================

Run the following command in the make file
make compile	To compile the server program.
make run1	To run process 1 program in the network
make run2	To run process 2 program in the network
make run3	To run process 3 program in the network
make run4	To run process 4 program in the network

PART 2 – Totally Ordered Messages.
==================================

Run the following command in the make file
make compile	To compile the process and sequencer program.
make runseq	To run sequencer program in the network
make runmulti1	To run process 1 program in the network
make runmulti2	To run process 2 program in the network
make runmulti3	To run process 3 program in the network



