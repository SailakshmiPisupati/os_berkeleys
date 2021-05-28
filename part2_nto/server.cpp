/*This is the code for Part 2 - Non - Totally Ordered Messages.*/
 
 
#include <stdio.h>
#include <string.h>   
#include <stdlib.h>   
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>  
#include <pthread.h>
#include <stdbool.h>
 
void *sendmessages(void *socket_desc);
void *receivemessages(void *socket_desc);
void create_sender_receiver_thread(short server_port, short connect_to_new);
void *sendmessage_toclient(void *socket_desc);
 
short server_port, connect_to;
int node_ports[4]; int number_of_nodes = 4;
int incrementor;
 
int main(int argc , char *argv[]){
  
   int *new_sock;
  
   int i=0;
   node_ports[0] = atoi(argv[1]);
   node_ports[1] = atoi(argv[2]);
   node_ports[2] = atoi(argv[3]);
   node_ports[3] = atoi(argv[4]);
   incrementor = atoi(argv[5]);
   printf("In main thread.\n");
  
   //connect_to = atoi(argv[4]);             // port number to which the process will send the information
   //server_port = atoi(argv[3]);
   create_sender_receiver_thread(server_port, connect_to);               //sender is the server
 
   return 0;
}
 
 
void create_sender_receiver_thread(short server_port,short connect_to){
   short *server_port_new;
   server_port_new = (short *) malloc(1);
   *server_port_new = server_port;
 
   short *connect_to_new;
   connect_to_new = (short *) malloc(1);
   *connect_to_new = connect_to;
 
   printf("In create receiver thread\n");
   pthread_t sniffer_thread;
    // a thread to receive the incoming messages from other processes.
   int n = pthread_create( &sniffer_thread , NULL ,  receivemessages , NULL);     
   if( n  < 0){
     perror("Could not create receiver thread");
   }
   sleep(10);
   pthread_t sender_thread;
   if( pthread_create( &sender_thread , NULL ,  sendmessage_toclient , NULL) < 0){
     perror("could not create thread");
   }
   pthread_join(sender_thread, NULL);
   pthread_join(sniffer_thread, NULL);
}
 
void *receivemessages(void *args){
   //int server_port = *(int*)server_port_new;
   int socket_desc , c , *new_sock;
   int client_sock;
   struct sockaddr_in server , client;
   char message[2000];
   FILE *filename;
   printf("In receive messages thread.\n");
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
       server.sin_port = htons(node_ports[0]);
       printf("Node port is %d\n", node_ports[0]);
       //Bind
       if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0){
           //print the error message
           perror("bind failed. Error");
       }
       printf("bind done\n");
      
       //Listen
       listen(socket_desc , 3);
   while(1){    
       //Accept and incoming connection
      
       //sleep(5);
       c = sizeof(struct sockaddr_in);
      
       client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c);
       if (client_sock < 0){
           perror("accept failed");
       }
      
      
       int recn = recv(client_sock, message,2000,0);
       printf("%s\n", message);
       char* timestamp = strtok(message," ");
       char* port_no_recv = strtok(NULL," ");
      
       printf("******************************************************************\n");
       printf("Received message %s from %s\n", timestamp, port_no_recv);
       printf("******************************************************************\n");
       //printf("Process received message %s sent by %d\n", message, client_sock);
       char fname[200];char content[100];
       snprintf(content, sizeof(content), "%s %s %s", "Message received is",timestamp,"\n");
       snprintf(fname, sizeof(fname), "%d%s ",node_ports[0] ,".log");
       filename = fopen(fname, "a+");
       if (filename == NULL) { /* Something is wrong   */}
       fprintf(filename, content);
       fclose(filename);
       close(client_sock);
      
      
   } 
   close(socket_desc);
  
}
 
int number;
 
void *sendmessage_toclient(void *socket_desc){
   //Get the socket descriptor
   //int sock = *(int*)socket_desc;
   int read_size;
   char node_message[2000];
   char message[2000];
   struct sockaddr_in server;
   int sock,i;
   while(1){
   //Create socket
 
       for(i=0;i<number_of_nodes;i++){
           sock = socket(AF_INET , SOCK_STREAM , 0);
           if(sock == -1){
             printf("Could not create socket");
           }
 
           server.sin_addr.s_addr = inet_addr("127.0.0.1");
           server.sin_family = AF_INET;
           server.sin_port = htons(node_ports[i]);
 
           //Connect to remote server
           if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0){
           perror("connect failed. Error");
           }else{
           printf("Connection with the process %d successful.\n",sock);
           }
 
           number = number + incrementor;
           snprintf(message, sizeof(message), "%d, %d", number,node_ports[0]);
           printf("Sending message %s to port %d\n",message,node_ports[i]);
           sleep(3);
           int n = send(sock,&message,sizeof(message),0);
           if(n<0)
               printf("Error in send");
           sleep(5);
           close(sock);
       }
   }
  
  
  
}

