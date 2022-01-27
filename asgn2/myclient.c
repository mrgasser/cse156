#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <err.h>

// this function reads data from a file and
// sends said data to the server
void send_file(int sockfd, FILE* in_file, FILE* out_file, int mtu, const struct sockaddr* dest_addr, socklen_t servlen) {
    int bytes_recv;
    int bytes_sent;
    char send_buffer[mtu]; 
    char recv_buffer[mtu + 1];
    memset(&send_buffer[0], 0, sizeof(send_buffer));
    memset(&recv_buffer[0], 0, sizeof(recv_buffer));

    // Loop throught file reading mtu bytes from in_file
    // Sending bytes to server, receiving bytes back from server
    // Writing received bytes to out_file
    while (fgets(send_buffer, mtu, in_file) != NULL) {
        printf("send buffer: %s\n", send_buffer); // print out buffer

        // send packet to server (size mtu)
        bytes_sent = sendto(sockfd, send_buffer, strlen(send_buffer), 0, (struct sockaddr*) &dest_addr, servlen);

        printf("bytes sent: %i\n", bytes_sent);
        if (bytes_sent == -1) {
            err(EXIT_FAILURE, "Error sending data to server");
        }

        // receive packets back from server
        bytes_recv = recvfrom(sockfd, recv_buffer, mtu, 0, NULL, NULL);

        recv_buffer[bytes_recv] = 0; // Null Terminate String
        fputs(recv_buffer, out_file); // write string to out_file
    }
}

// CLIENT
int main(int argc, char *argv[]) {

    // need 6 arguments 
    if (argc != 6) {
        err(EXIT_FAILURE, "Incorrect Number of Arguments");
    }

    // get program arguments
    // format:
    // ./myclient server_ip server_port mtu in_file_path out_file_path

    // get program arguments
    char* ip_addr = argv[1];
    int port = atoi(argv[2]);
    int mtu = atoi(argv[3]);
    char *in_file = argv[4];
    char *out_file = argv[5];

    printf("Arguments:\n%s %i %i %s %s\n", ip_addr, port, mtu, in_file, out_file);

    printf("Creating Client Socket\n");
    // Create UDP socket
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
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

    //open files
    FILE* in_fp = fopen(in_file, "r"); // open read only
    FILE* out_fp = fopen(out_file, "w"); // open write only
    
    if (in_fp == NULL || out_file == NULL) { // check if error opening files
        err(EXIT_FAILURE, "Error Opening files");
    }

    // call funtion to read and send file to server
    send_file(sockfd, in_fp, out_fp, mtu, (struct sockaddr*) &addr, sizeof(addr)); 

    exit(0); //exit successful
}
