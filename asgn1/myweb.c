#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>

/*
// Function parses program arguments
void parse_args(argc, argv) {
    // get program arguments
    int c;
    while ((c = getopt(argc, argv, "h")) != -1) {
        switch (c)
        {
        case 'h':
            // Enable non printing flag
            break;
        
        default:
            break;
        }
    }
}
*/

int create_socket (int port, char* IP) {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) { //error creating socketfd
        return -1;
    }

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof addr);
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port); // Add port to socket addr struct

    //Convert IP str into hex and store into sin_addr of socket struct
    if (inet_pton(AF_INET, IP, &addr.sin_addr) <= 0) { // check if IP address is valid
        return -1;
    }

    if (connect(sockfd, (struct sockaddr*) &addr, sizeof addr)) {
        return -1;
    }
    return sockfd;
}

int main(int argc, char *argv[]) {
    printf("argc: %i\n", argc);
    
    //printing out args
    char s[100];
    for (int i = 1; i < argc; i++) {
        memset(&s[0], 0, sizeof(s));
        strcpy(s, argv[i]);
        printf("%s ", s);
    }
    printf("\n");

    int clientfd = create_socket(80, "93.184.216.34");

    printf("socket Created: %i\n", clientfd);

    //sending request
    char request[1042];
    memset(&request[0], 0, sizeof(request));
    sprintf(request, "GET /index.html HTTP/1.1\r\nHost: www.example.com\r\n\r\n");

    int num_sent = send(clientfd, request, strlen(request), 0);
    printf("Bytes sent: %i\n", num_sent);

    char response[1042];
    memset(&response[0], 0, sizeof(response));
    int bytes_recv = recv(clientfd, response, 1024, 0);

    printf("Bytes Recieved: %i", bytes_recv);
    printf("%s\n", response);


}


