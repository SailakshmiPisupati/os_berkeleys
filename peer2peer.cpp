#include <stdio.h>
#include <string.h>    
#include <stdlib.h>    
#include <sys/socket.h>
#include <arpa/inet.h> 
#include <unistd.h>   
#include <pthread.h>
#include <stdbool.h>

void makeThisServer(short connect_to);
void makeThisClient(short connect_to);
void *berkeleys_algorithm(void *socket_desc);


int network_node_count, nodes_connected; 

int main(int argc , char *argv[])
{
   short server_port, connect_to;
   int *new_sock;

   char *COORDINATOR = "coordinator";
   if(argc < 4){
    printf("Enter the following parameters <no_of_nodes> <node_type> <server_port> <self_port>\n");
   }

   if(strcmp(argv[2],COORDINATOR) == 0){
      printf("Process started. Process type is coordinator.\n");
      network_node_count = atoi(argv[1]);
      printf("Number of nodes are %d\n", network_node_count);
      server_port = atoi(argv[4]);
      makeThisServer(server_port);
    }
   else{
      
      connect_to = atoi(argv[3]);
      makeThisClient(connect_to);
   }  

   return 0;
}

void makeThisClient(short connect_to){

  int sock;
  struct sockaddr_in server;
  char message[20] , server_reply[2000];

    //Create socket
    sock = socket(AF_INET , SOCK_STREAM , 0);
    if (sock == -1)
    {
        printf("Could not create socket");
    }
     
    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_family = AF_INET;
    server.sin_port = htons(connect_to);
 
    //Connect to remote server
    if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0)
    {
        perror("connect failed. Error");     
    }else{
      printf("Connection with the process daemon successful.\n");
    }


    recv(sock , server_reply , 2000 , 0);
    printf("Server says..%s\n",server_reply);



}

void makeThisServer(short server_port){
    int socket_desc , c , *new_sock;
    int client_sock;
    struct sockaddr_in server , client;
    nodes_connected = 0;
    char message[2000];

   //Create socket
    socket_desc = socket(AF_INET , SOCK_STREAM , 0);
    if (socket_desc == -1)
    {
        printf("Could not create socket\n");
    }
    printf("Server up and running....\n");
     
    //Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons( server_port );
     
    //Bind
    if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
    {
        //print the error message
        perror("bind failed. Error");
    }
    printf("bind done\n");
     
    //Listen
    listen(socket_desc , 3);
     
    //Accept and incoming connection
    printf("Waiting for communication with the other nodes.\n");
    c = sizeof(struct sockaddr_in);
    
     
    //Accept and incoming connection
   
    c = sizeof(struct sockaddr_in);
    while( (client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c)) )
    {
        nodes_connected++;
        printf("New client connected. Node connected count %d\n",nodes_connected);
        printf("Network node in total:%d\n", network_node_count);
        
            printf("Creating a new thread for time synchronization.\n");
            pthread_t sniffer_thread;
            new_sock = (int *) malloc(1);
            *new_sock = client_sock;

            sleep(300);           //sleep till all the clients connect.
            if( pthread_create( &sniffer_thread , NULL ,  berkeleys_algorithm , (void*) new_sock) < 0)
            {
              perror("could not create thread");
             
            }

            //Now join the thread , so that we dont terminate before the thread
            //pthread_join( sniffer_thread , NULL);
            //printf("Assigned new thread connection");
        // if(nodes_connected == network_node_count)
        // { 
        //   printf("getAverageOfTime();");
        // }
        
    }    
    if (client_sock < 0)
    {
        perror("accept failed");
       
    }
}

void *berkeleys_algorithm(void *socket_desc){

  //Get the socket descriptor
    int sock = *(int*)socket_desc;
    int read_size;
    char node_message[2000];
    char message[2000];

     if(nodes_connected == network_node_count)
     { 
        printf("getAverageOfTime();");
        printf("Berkeleys algorithm\n");
        strcpy(message,"Send local time.");
        send(sock,&message,sizeof(message),0);  
     }
    
}