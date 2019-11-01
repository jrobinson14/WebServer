/*
Author: Jonathan Robinson

clinet for lab2
gets url from command line arg, uses to send GET command to server
format for url is: [IP number]:[port number]/[filepath]
Example: 127.0.0.1:12345/HTML/Lab2doc.html
*/

#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <string.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h> 



int clientTCP(char* ip, int port, char* msg){
    printf("Sending command to TCP server: %s\n", msg);

    struct sockaddr_in serv_addr; 
    //char *hello = "GET /HTML/Lab2AWebServerModel.html http"; 
    char buffer[1024] = {0}; 
    int newSocket;

    if ((newSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
    { 
        printf("Client: Error Creating Socket\n"); 
        return -1; 
    } 
   
    serv_addr.sin_family = AF_INET; 
    serv_addr.sin_port = htons(port); 
       
    
    if(inet_pton(AF_INET, ip, &serv_addr.sin_addr)<=0)  
    { 
        printf("Client: Error Converting IP string\n"); 
        return -1; 
    } 
   
    if (connect(newSocket, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) 
    { 
        printf("Client: Error Connecting\n"); 
        return -1; 
    } 
    //send message, read response
    send(newSocket , msg , strlen(msg) , 0 ); 
    printf("Client: Message sent\n"); 
    int ret = read(newSocket, buffer, 1024); 
    printf("Message From Server: \n%s\n",buffer ); 
    close(newSocket); //close socket
    return 0; 
}

int clientUDP(char* ip, int port, char* msg){
    printf("Sending command to UDP server: %s\n", msg);

    int newSocket; 
    char buffer[1024]; 
    struct sockaddr_in servaddr; 
  
    // Create new socket
    if ((newSocket = socket(AF_INET, SOCK_DGRAM, 0)) < 0) { 
        perror("Client: Error Creating Socket"); 
        exit(EXIT_FAILURE); 
    } 
  
    memset(&servaddr, 0, sizeof(servaddr)); 
      
    
    servaddr.sin_family = AF_INET; 
    servaddr.sin_port = htons(port); 
    servaddr.sin_addr.s_addr = INADDR_ANY; 

    if(inet_pton(AF_INET, ip, &servaddr.sin_addr)<=0)  
    { 
        printf("Client: Error Converting IP string\n"); 
        return -1; 
    } 
      
    int n; 
    socklen_t len = 1024; 
    //len = sizeof(servaddr);
      
    sendto(newSocket, (const char *)msg, strlen(msg), 0, (const struct sockaddr *) &servaddr,  sizeof(servaddr)); 
    printf("Client: Message sent.\n"); 
          
    n = recvfrom(newSocket, (char *)buffer, 1024,  MSG_WAITALL, (struct sockaddr *) &servaddr, &len); 
    //buffer[n] = '\0'; 
    printf("Message From Server: \n%s\n", buffer); 
  
    close(newSocket); 
    return 0; 
}


int main(int argc, char *argv[]){


    //char* url = malloc(strlen(argv[1]));
    //strncpy(url, argv[1], strlen(url));
    char url[1024] = {'\0'};
    strcpy(url, argv[1]);
    char* protocol = argv[2];
    printf("buffer created\n");

    //get info from url
    char* ip = malloc(24);
    ip[23] = '\0';
    char* port = malloc(10);
    port[9] = '\0';
    char* filepath = malloc(100);
    filepath[99] = '\0';
    char* toServer = malloc(1024);
    int i = 0;
    int j = 0;

    while(url[i] != ':'){
        ip[j] = url[i];
        i++, j++;
    }
    //printf("IP is: %s\n", ip);
    j = 0;
    i++;

    while(url[i] != '/'){
        port[j] = url[i];
        i++, j++;
    }
    //printf("IP is: %s\nPort is %s\n", ip, port);
    j = 0;
    while(url[i] != '\0'){
        filepath[j] = url[i];
        i++, j++;
    }

    printf("IP is: %s\nPort is %s\nFilepath is %s\n", ip, port, filepath);
    printf("Protocol is: %s\n", protocol);

    //make http command to send to server
    snprintf(toServer, 1024, "%s%s%s", "GET ", filepath, " HTTP/1.1\0");
    printf("Command is %s\n", toServer);

    if(strcmp(protocol, "TCP") == 0 || strcmp(protocol, "tcp") == 0  ){
        int newPort = atoi(port);
        printf("New port is: %d\n", newPort);
        printf("Client: TCP chosen\n");
        clientTCP(ip, newPort, toServer);
    } else if(strcmp(protocol, "UDP") == 0 || strcmp(protocol, "udp") == 0  ){
        int newPort = atoi(port);
        printf("New port is: %d\n", newPort);
        printf("Client: UDP chosen\n");
        clientUDP(ip, newPort, toServer);
    }

    free(ip);
    free(port);
    free(filepath);
    free(toServer);
    return 0;

}
