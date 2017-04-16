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


int network_node_count, nodes_connected, server_time; 

int main(int argc , char *argv[])
{
   short server_port, connect_to;
   int *new_sock;

   //char[] *COORDINATOR = "coordinator";
   if(argc < 3){
    printf("Enter the following parameters <no_of_nodes> <node_type> <server_port> <self_port>\n");
   }

   if(strcmp(argv[2],"coordinator") == 0){
      printf("Process started. Process type is coordinator.\n");
      network_node_count = atoi(argv[1]);
      printf("Number of nodes are %d\n", network_node_count);
      server_port = atoi(argv[3]);
      makeThisServer(server_port);
    }else{ 
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
  if(sock == -1){
      printf("Could not create socket");
  }
   
  server.sin_addr.s_addr = inet_addr("127.0.0.1");
  server.sin_family = AF_INET;
  server.sin_port = htons(connect_to);

  //Connect to remote server
  if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0){
    perror("connect failed. Error");
  }else{
    printf("Connection with the process daemon successful.\n");
  }

  recv(sock , server_reply , 2000 , 0);
  int server_time_received = atoi(server_reply);
  printf("Server time received :%d\n",server_time_received);

  srand(time(NULL));
  
  int time_stamp = rand() % 30;
  printf("Local timestmp is :%d\n", time_stamp);
  
  int offset_time = time_stamp - server_time_received;
  printf("Offset time is :%d\n", offset_time);

  snprintf(message, sizeof(message), "%d", offset_time);
  int n=send(sock,&message,sizeof(message),0); 
    if(n<0)
      printf("Error in send");

  recv(sock , server_reply , 2000 , 0);
  printf("Server says..%s\n",server_reply);
}


void makeThisServer(short server_port){
    int socket_desc , c , *new_sock;
    int client_sock;
    int connected_clients[network_node_count];
    struct sockaddr_in server , client;
    nodes_connected = 0;
    char message[2000];

    int nodeTimeStamps[network_node_count];

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
    if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0){
        //print the error message
        perror("bind failed. Error");
    }
    printf("bind done\n");
     
    //Listen
    listen(socket_desc , 3);
     
    //Accept and incoming connection
    printf("Waiting for communication with the other nodes.\n");
    c = sizeof(struct sockaddr_in);
    srand(time(NULL));
    //server_time = rand() % 50;
    server_time = 50;
    printf("Server time is %d\n", server_time);
    //Accept and incoming connection
   
    c = sizeof(struct sockaddr_in);
    while( (client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c)) )
    {
        connected_clients[nodes_connected]= client_sock;
        printf("Socket of connected client is %d\n", client_sock);
        nodes_connected++;
        printf("New client connected. Node connected count %d\n",nodes_connected);
        printf("Network node in total:%d\n", network_node_count);
        printf("Berkeleys algorithm\n");


            printf("Creating a new thread for time synchronization.\n");
            pthread_t sniffer_thread;
            new_sock = (int *) malloc(1);
            *new_sock = client_sock;

                      //sleep till all the clients connect.
            if( pthread_create( &sniffer_thread , NULL ,  berkeleys_algorithm , (void*) new_sock) < 0)
            {
              perror("could not create thread");
             
            }        
    }    
    if (client_sock < 0)
    {
        perror("accept failed");
       
    }
}

int receiving_count =0;

void *berkeleys_algorithm(void *socket_desc){

  //Get the socket descriptor
    int sock = *(int*)socket_desc;
    int read_size;
    char node_message[2000];
    char message[2000], client_message[2000];
    
    int client_timestamps[network_node_count];
    snprintf(message, sizeof(message), "%d", server_time);
    
    
    if(network_node_count == nodes_connected){
    printf("Sending time to client %s\n",message);
    int n = send(sock,&message,sizeof(message),0); 
    if(n<0)
        printf("Error in send");
    sleep(10); 
    int recn = recv(sock, client_message,2000,0);
    printf("Client offset is %s\n", client_message);
    int client_time = atoi(client_message);
    printf("Client time is %d and reciving time %d\n", client_time,receiving_count);
    client_timestamps[receiving_count] = client_time;
    printf("Client timestamp is : %d\n", client_timestamps[receiving_count]);
    receiving_count++;
    
    int average_time =0,sum=0;
    if(receiving_count == network_node_count){ 
      
      int sync_time =0;
      printf("last client connected");
      for(int i=0;i<receiving_count;i++){
        printf("Inside loop client_timestamps %d\n",client_timestamps[i]);
            sum = client_timestamps[i] + average_time;
            printf("Sum is %d\n", sum);
      }
      average_time = sum / (network_node_count+1);
      printf("AVerage time is %d\n", average_time);
      int new_time = average_time + server_time;
      printf("Server will now make its time to %d\n", new_time);
      for(int i=0;i<network_node_count;i++){
        sync_time = (client_timestamps[i] * -1 )+ average_time;
        printf("Sync time is :%d\n", sync_time);
        snprintf(message, sizeof(message), "%d", sync_time);
        int n = send(sock, &message, sizeof(message),0);
        if(n < 0){
          printf("Error in Sending\n");
      }}
    }
  }
}