/* Author: Jonathan Robinson
   CS 585

   server.c: implementation of both TCP and UDP network sockets
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <string.h> 
#include <arpa/inet.h>
#include <unistd.h> 
#include <sys/stat.h>
#include <sys/types.h> 
#include <netinet/in.h>
#include <netdb.h>

char* getFile(char* filepath){
    char *header = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: ";

    printf("Searching for: \n%s\n", filepath);
    //char* htmlText;
    //char* filename = "./HTML/Lab2AWebServerModel.html";
    char* returnData = malloc(1026);
    char filename[100];
    strcpy(filename, ".");
    strcat(filename, filepath);
    printf("result of cat is: %s\n", filename);
    struct stat fileStat;
    if(stat(filename, &fileStat) == -1){
        strcpy(returnData, "error getting stat\n");
        return "ERROR: FILE NOT FOUND";
    }
    
    FILE* fptr = fopen(filename, "r");
    char* htmlText = malloc(sizeof(fileStat.st_size));
    char size[4] = {'\0'};
    sprintf(size, "%lld", fileStat.st_size);

    //create buffer for html text, read file data to it
    //htmlText[fileStat.st_size];
    printf("success creating buffer!!\n");
    printf("File size: %s\n", size);
    fread(htmlText, 1, fileStat.st_size, fptr);

    //create buffer for all data needed to be sent to client
    //char* returnData = malloc(1026);
    printf("Size of htmlText: %lu\n", sizeof(htmlText));
    printf("Size of return data is %lu\n", sizeof(returnData));
    snprintf(returnData, 1026, "%s%s\n\n%s", header, size, htmlText);
    printf("Data: \n%s\n", returnData);
    free(htmlText);
    fclose(fptr);
    return returnData;
    

}

//if get request, returns resonse message
//else returns null string
char* parseMsg(char* msg){

    //default response contains header info
    //need to change content length and add html file contents to end
    char *response = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 12\n\n";
    //char* newMsg; //contains html as plain text



    char reqType[10] = {'\0'};
    char arg[1024] = {'\0'};
    char httpType[100] = {'\0'};
    printf("Message from client: %s\n", msg); 
    char c; 
    //get request type
    int i = 0;
    int j = 0;
    while(msg[j] != ' '){
        reqType[i] = msg[j];
        i++;
        j++;
    }
    j++;
    printf("Request type is %s\n", reqType);

    //get argument
    i = 0;
    while(msg[j] != ' '){
        arg[i] = msg[j];
        i++;
        j++;
    }
    j++;
    printf("Argument is: %s\n", arg);
    //get http type
    /*i = 0;
    while(msg[j] != NULL ){
        httpType[i] = msg[j];
        i++;
        j++;
    }
    printf("HTTP type is: %s\n", httpType);*/

    if(strcmp(reqType, "GET") == 0){ //change to "Get"
        //perform get
        printf("MATCH\nLooking for: %s", arg);
        char* newMsg = getFile(arg);
        //printf("Data from parse is: %s\n", newMsg);
        return newMsg;
    }
    else
        return NULL;    
}

void runServerUDP(int port){
    char ipAdd[INET_ADDRSTRLEN]; //buffer for IP address, set to length of address
    //int sockfd;
    char bufferIn[1024];
    int newPort = port;
    struct sockaddr_in srvrInfo, clntInfo; //creating struct w/ ip, port info for server and client
    socklen_t clntAddLen; //stores lenght of client address for recvfrom function


    int newSocket = socket(PF_INET, SOCK_DGRAM, 0); //NOTE: use STREAM for TCP, DGRAM for UDP
    //memset(&srvrInfo, 0, sizeof(srvrInfo)); why?
    //memset(&clntInfo, 0, sizeof(clntInfo)); 


    srvrInfo.sin_addr.s_addr = INADDR_ANY; //set ip to localhost
    srvrInfo.sin_family = AF_INET;
    srvrInfo.sin_port = htons(newPort);

    //the next portion of code creates the socket, listens and accepts incomming connection requests
    if(bind(newSocket, (struct sockaddr *)&srvrInfo, sizeof(srvrInfo)) < 0){
        perror("Server: Error binding socket\n");
        exit(EXIT_FAILURE);
    }

    //display ip/port info
    char hostname[100]; 
    char *ipNum; 
    struct hostent *host_info; 
    int ret; 
    ret = gethostname(hostname, sizeof(hostname)); 
    host_info = gethostbyname(hostname); 
    ipNum = inet_ntoa(*((struct in_addr*) host_info->h_addr_list[0])); 
    printf("Host IP: %s\n", ipNum); 
    printf("Port is: %d\n", newPort);

    //UDP stuff, different from TCP
    clntAddLen = sizeof(clntInfo);
    for(;;){ //loop until closed
    ssize_t end = recvfrom(newSocket, bufferIn, sizeof(bufferIn), MSG_WAITALL, (struct sockaddr *) &clntInfo, &clntAddLen);
    bufferIn[end] = '\0';
    printf("Message from client: %s\n", bufferIn);
    char* returnMsg = getFile("/HTML/Lab2AWebServerModel.html");
    //char* retMsg2 = "Hello from server";
    sendto(newSocket, (const char *) returnMsg, strlen(returnMsg), 0, (const struct sockaddr *) &clntInfo, clntAddLen );
    //printf("Message to send UDP is %s\n", returnMsg);
    printf("Server: UDP message sent\n");
    free(returnMsg);
    }
    printf("UDP Server done\n");


}

void runServerTCP(int port){
    char ipAdd[INET_ADDRSTRLEN]; //buffer for IP address, set to length of address
    char msgBuffer[1024] = {'\0'}; //store message from client, initialize values 
    
    //create new socket
    int newPort = port;
    int newSocket = socket(PF_INET, SOCK_STREAM, 0); //NOTE: use STREAM for TCP, DGRAM for UDP
    int new_Socket;
    struct sockaddr_in addInfo; //creating struct w/ ip, port info
    int addrlen = sizeof(addInfo); //store size of addInfo

    addInfo.sin_addr.s_addr = INADDR_ANY; //set ip to localhost
    addInfo.sin_family = AF_INET;
    addInfo.sin_port = htons(newPort);

    //the next portion of code creates the socket, listens and accepts incomming connection requests
    if(bind(newSocket, (struct sockaddr *)&addInfo, sizeof(addInfo)) < 0){
        perror("Server: Error binding socket\n");
        exit(EXIT_FAILURE);
    }

    //display ip/port info
    char hostname[100]; 
    char *ipNum; 
    struct hostent *host_info; 
    int ret; 
    ret = gethostname(hostname, sizeof(hostname)); 
    host_info = gethostbyname(hostname); 
    ipNum = inet_ntoa(*((struct in_addr*) host_info->h_addr_list[0])); 
    printf("Host IP: %s\n", ipNum); 
    
    /*printf("Server: new socket created\n");
    const char* result = inet_ntop(AF_INET, &addInfo, ipAdd, INET_ADDRSTRLEN);*/
    printf("Port is: %d\n", newPort);

    
    

    //wait for connection from client
        if (listen(newSocket, 100) < 0) 
        { 
            perror("Server: Error waiting for message\n");
            exit(EXIT_FAILURE); 
        }

        //blocking, wait for connection from client
        //TODO: can i use newSocket or do i need to use new_socket?
    for(;;){
        if ((new_Socket = accept(newSocket, (struct sockaddr *)&addInfo,  (socklen_t*)&addrlen))<0) 
        { 
            perror("Server: Error Accepting Client\n");
            exit(EXIT_FAILURE); 
        } 

        //read recieved message
        int readReturn = read(new_Socket, msgBuffer, 1024); //gets val < 0 if error, otherwise size of msg in bytes
        if(read < 0)
            printf("Server: Error reading message\n");
        else{
            //parseMsg will generate this message in actual server
            //char* msg = parsMsg(msgBuffer);
            char* returnMsg = parseMsg(msgBuffer);
            //char *hello = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 12\n\nHello world!";
            if(returnMsg != NULL){
                write(new_Socket, returnMsg, strlen(returnMsg));
                free(returnMsg);
            } else //file wasnt found or something went wrong
            {
                char* err = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 36\n\nERROR: FILE NOT FOUND OR BAD REQUEST\n";
                write(new_Socket, err, strlen(returnMsg));
                free(returnMsg);
            }            
        
        }

        close(new_Socket);
            
    }
    printf("Server: Done\n");

}

int main(int argc, char *argv[]){
    char* servType = argv[1];
    int port = atoi(argv[2]);
    if(strcmp(servType, "TCP") == 0 || strcmp(servType, "tcp") == 0  ){
        printf("New port is: %d\n", port);
        printf("Server: TCP chosen\n");
        runServerTCP(port);
    } else if(strcmp(servType, "UDP") == 0 || strcmp(servType, "udp") == 0  ){
        printf("New port is: %d\n", port);
        printf("Server: UDP chosen\n");
        runServerUDP(port);
    }
    //printf("New port is: %d\n", port);
    //runServerTCP(port);
    return 0;
}
