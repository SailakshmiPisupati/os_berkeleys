#include <stdio.h>
#include <string.h>    
#include <stdlib.h>    
#include <sys/socket.h>
#include <arpa/inet.h> 
#include <unistd.h>   
#include <pthread.h>
#include <stdbool.h>


void create_server(short server_port);
void *sendmessages(void *socket_desc);
void *receivemessages(void *socket_desc);
void create_sender_thread();
void create_receiver_thread();

int main(int argc , char *argv[]){
    short server_port, connect_to;
    int *new_sock;
    int number_of_nodes = 4;
    int node_ports[number_of_nodes] ={8098,8099,8067,8077};
    //char[] *COORDINATOR = "coordinator";

    create_sender_thread();
    create_receiver_thread();

    return 0;
}

void create_sender_thread(){
    
    pthread_t sniffer_thread;
    if( pthread_create( &sniffer_thread , NULL ,  sendmessages , (void*) new_sock) < 0){
      perror("Could not create sender thread");
    }
}

void create_receiver_thread(){
    pthread_t sniffer_thread;
    if( pthread_create( &sniffer_thread , NULL ,  receivemessages , (void*) new_sock) < 0){
      perror("Could not create receiver thread");
    }
}

void create_server(short server_port){
    int socket_desc , c , *new_sock;
    int client_sock;
    
    struct sockaddr_in server , client;
   
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
    
    while( (client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c)) )
    {
       
       
        printf("New client connected. Node connected count %d\n",client_sock);
        

        
        	printf("Creating a new thread for time synchronization.\n");
            pthread_t sniffer_thread;
           
	            new_sock = (int *) malloc(1);
	            *new_sock = client_sock;

	                      //sleep till all the clients connect.
	            if( pthread_create( &sniffer_thread , NULL ,  sendmessages , (void*) new_sock) < 0)
	            {
	              perror("could not create thread");
	             
	            } 

                if( pthread_create( &sniffer_thread , NULL ,  receivemessages , (void*) new_sock) < 0)
                {
                  perror("could not create thread");
                 
                }   
        

                
    }    
    if (client_sock < 0)
    {
        perror("accept failed");
       
    }
    close(socket_desc);
}


void *sendmessages(void *socket_desc){

  	//Get the socket descriptor
    int sock = *(int*)socket_desc;
    int read_size;
    char node_message[2000];
    char message[2000], client_message[2000];
        
    
    printf("New thread created for sendmessages\n");
    
}


void *receivemessages(void *socket_desc){

    //Get the socket descriptor
    int sock = *(int*)socket_desc;
    int read_size;
    char node_message[2000];
    char message[2000], client_message[2000];
        
    
    printf("New thread created for receivemessages\n");
    
}