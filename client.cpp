#include <stdio.h>
#include <string.h>    
#include <stdlib.h>    
#include <sys/socket.h>
#include <arpa/inet.h> 
#include <unistd.h>   
#include <pthread.h>
#include <stdbool.h>

void create_client(short connect_to,int client_time);

int main(int argc , char *argv[])
{
   short server_port, connect_to;
   int *new_sock;

   //char[] *COORDINATOR = "coordinator";
   if(argc < 3){
    printf("Enter the following parameters <no_of_nodes> <node_type> <server_port> <self_port>\n");
   }

   int client_time = atoi(argv[1]);
   
      connect_to = atoi(argv[3]);
      create_client(connect_to,client_time);
   
   return 0;
}


void create_client(short connect_to, int client_time){

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
  
  int time_stamp = client_time;
  printf("Local timestmp is :%d\n", time_stamp);
  
  int offset_time = time_stamp - server_time_received;
  printf("Offset time is :%d\n", offset_time);

  snprintf(message, sizeof(message), "%d", offset_time);
  int n=send(sock,&message,sizeof(message),0); 
    if(n<0)
      printf("Error in send");
    int new_time =0;
  recv(sock , server_reply , 2000 , 0);
   new_time = atoi(server_reply);

  printf("Server offset time by %d\n",new_time);

  int new_client_time = time_stamp + new_time;
  printf("Client will switch from %d to %d now.\n",time_stamp,new_client_time );

}