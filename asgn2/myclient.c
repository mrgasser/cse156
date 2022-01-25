#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <err.h>


// CLIENT
int main(int argc, char *argv[]) {

    // need to get these from program arguments
    int ip_addr = 10;
    int port = 80;

    // Create UDP socket
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) { //error creating socketfd
        err(EXIT_FAILURE, "socket error error");
    }

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof addr);
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port); // Add port to socket addr struct

    //Convert IP str into hex and store into sin_addr of socket struct
    if (inet_pton(AF_INET, ip_addr, &addr.sin_addr) <= 0) { // check if IP address is valid
        err(EXIT_FAILURE, "invalid ip address");
    }

    if (connect(sockfd, (struct sockaddr*) &addr, sizeof addr)) {
        err(EXIT_FAILURE, "connection error");
    }

}