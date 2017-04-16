#include <stdio.h>
#include <string.h>    
#include <stdlib.h>    
#include <sys/socket.h>
#include <arpa/inet.h> 
#include <unistd.h>   
#include <pthread.h>
#include <stdbool.h>


int network_node_count, nodes_connected, server_time; 
int new_time =0;
int receiving_count =0;
int client_timestamps[2];

void create_server(short server_port);
void *berkeleys_algorithm(void *socket_desc);
int calculate_average();
int calculate_sync_time(int new_time);

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
      create_server(server_port);
    }

   return 0;
}

void create_server(short server_port){
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

        if(network_node_count == nodes_connected){
        	printf("Creating a new thread for time synchronization.\n");
            pthread_t sniffer_thread;
            for(int i=0;i<network_node_count;i++){
	            new_sock = (int *) malloc(1);
	            *new_sock = connected_clients[i];

	                      //sleep till all the clients connect.
	            if( pthread_create( &sniffer_thread , NULL ,  berkeleys_algorithm , (void*) new_sock) < 0)
	            {
	              perror("could not create thread");
	             
	            } 

	            int val = pthread_join( sniffer_thread , NULL);
				printf("Thread endss %d\n", val);
				
				for(int i=0;i<network_node_count;i++){
					int sync_time = (client_timestamps[i] * -1 ) + new_time;

				printf("Sync time is :%d\n", sync_time);
				snprintf(message, sizeof(message), "%d", sync_time);
				int n0 = send(connected_clients[i], &message, sizeof(message),0);
				if(n0 < 0){
				  printf("Error in Sending\n");
					}
				}  
            }
              
        }

                
    }    
    if (client_sock < 0)
    {
        perror("accept failed");
       
    }
    close(socket_desc);
}

int calculate_sync_time(int client_time){
    printf("Client time is %d and reciving time %d\n", client_time,receiving_count);
    client_timestamps[receiving_count] = client_time;
    printf("Client timestamp is : %d\n", client_timestamps[receiving_count]);
    receiving_count++;
    if(receiving_count == network_node_count){
    	int new_time= calculate_average();
    	return new_time;
    }
}

int calculate_average(){
	int average_time =0,sum=0; 
	int sync_time =0;
	
	for(int i=0;i<receiving_count;i++){
	printf("Inside loop client_timestamps %d\n",client_timestamps[i]);
	    sum = client_timestamps[i] + sum;
	    printf("Sum is %d\n", sum);
	}
	average_time = sum / (network_node_count+1);
	printf("Average time is %d\n", average_time);
	int new_time = average_time + server_time;
	printf("Server will now make its time to %d\n", new_time);
	return average_time;
}

void *berkeleys_algorithm(void *socket_desc){

  	//Get the socket descriptor
    int sock = *(int*)socket_desc;
    int read_size;
    char node_message[2000];
    char message[2000], client_message[2000];
        
    
    printf("New thread created\n");
    
    if(network_node_count == nodes_connected){
    	snprintf(message, sizeof(message), "%d", server_time);
	    printf("Sending time to client %s\n",message);
	    int n = send(sock,&message,sizeof(message),0); 
	    if(n<0)
	        printf("Error in send");
	    
	    int recn = recv(sock, client_message,2000,0);
	    printf("Client offset is %s\n", client_message);
	    int client_time = atoi(client_message);
	    
	    int new_time =calculate_sync_time(client_time);
	     
  }
}