#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <err.h>

// SERVER
int main(int argc, char *argv[]) {

    //check arguments
    if (argv > 2) {
        err(EXIT_FAILURE, "Incorrect Number of Arguments");
        printf("BAP\n");
    }
    int port = 80;

    // create  UDP socket
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0); // UDP socket
    if (sockfd < 0) { //error creating socketfd
        err(EXIT_FAILURE, "socket error error");
    }

    struct sockaddr_in serveraddr, clientaddr;
    memset(&serveraddr, 0, sizeof serveraddr);
    
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(port); // Add port to socket addr struct
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);

    bind(sockfd, (struct sockaddr*) &serveraddr, sizeof(serveraddr));

    // Do echo stuff

    exit(0);

}

