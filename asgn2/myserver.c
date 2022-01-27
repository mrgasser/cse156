#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <err.h>

#define MAX_LINE 32768

// function handles receiving packets from 
void handle_packets(int sockfd, struct sockaddr* clientaddr, socklen_t clientlen) {
    int bytes_recv;
    socklen_t clen;
    char buffer[MAX_LINE];
    
    printf("Ready for packets\n");

    // infite loop to accept and echo back packets
    while(1) {
        clen = clientlen; 

        // receive data packet from client
        bytes_recv = recvfrom(sockfd, buffer, MAX_LINE, 0, clientaddr, &clen);

        printf("Bytes Received: %i\n", bytes_recv);
        printf("BUFFER RECEIVED:\n%s\n", buffer);

        // echo data right back to client
        sendto(sockfd, buffer, bytes_recv, 0, clientaddr, clen); 
    }
}

// SERVER
int main(int argc, char *argv[]) {

    if (argc != 2) { //check for correct number of arguments
        err(EXIT_FAILURE, "Incorrect Number of Arguments");
    }

    int port = atoi(argv[1]); // save port from args

    printf("Running on port: %i\n", port);

    // create  UDP socket
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0); // UDP socket
    if (sockfd < 0) { //error creating socketfd
        err(EXIT_FAILURE, "socket error error");
    }

    //create socket structs
    struct sockaddr_in serveraddr, clientaddr;
    memset(&serveraddr, 0, sizeof serveraddr);
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(port); // Add port to socket addr struct
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    bind(sockfd, (struct sockaddr*) &serveraddr, sizeof(serveraddr));

    // call function to echo packets
    handle_packets(sockfd, (struct sockaddr*) &clientaddr, sizeof(clientaddr));

    exit(0);
}

