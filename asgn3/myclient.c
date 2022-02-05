#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <err.h>
#include <signal.h>

#define HEADER_SIZE 20

// this function is called on a timeout 
void sig_handler() {
    err(EXIT_FAILURE, "client timed out");
}

// this function reads data from a file and
// sends said data to the server
void send_file(int sockfd, FILE* in_file, FILE* out_file, int mtu, const struct sockaddr* dest_addr, socklen_t servlen) {
    int bytes_recv;
    int bytes_sent;
    char send_buffer[mtu]; 
    char recv_buffer[mtu + 1];
    memset(&send_buffer[0], 0, sizeof(send_buffer));
    memset(&recv_buffer[0], 0, sizeof(recv_buffer));

    signal(SIGALRM, sig_handler); //Register alarm signal to call sig_handler function

    // Loop throught file reading mtu bytes from in_file
    // Sending bytes to server, receiving bytes back from server
    // Writing received bytes to out_file
    while (fgets(send_buffer, mtu - HEADER_SIZE, in_file) != NULL) {

        alarm(60); // after 60 seconds, we exit program with a transmission error

        // send packet to server (size mtu)
        bytes_sent = sendto(sockfd, send_buffer, strlen(send_buffer), 0, dest_addr, servlen);
        if (bytes_sent == -1) {
            err(EXIT_FAILURE, "Error sending data to server");
        }
        alarm(0); // reset alarm for successful send

        alarm(60); // after 60 seconds, we exit program with a transmission error
        bytes_recv = recvfrom(sockfd, recv_buffer, mtu, 0, NULL, NULL); // receive packets back from server
        alarm(0); // reset alarm on successful reply from server

        recv_buffer[bytes_recv] = 0; // Null Terminate String
        fputs(recv_buffer, out_file); // write string to out_file
    }
}


int main(int argc, char *argv[]) {

    // need 7 arguments 
    if (argc != 7) {
        err(EXIT_FAILURE, "Incorrect Number of Arguments");
    }

    // get program arguments
    // ./myclient server_ip server_port mtu winsz in_file_path out_file_path
    char* ip_addr = argv[1];
    int port = atoi(argv[2]);
    int mtu = atoi(argv[3]);
    int window_size = atoi(argv[4]);
    char *in_file = argv[5];
    char *out_file = argv[6];

    // check if mtu size is valud
    // ** this will depend on headersize, 
    if (mtu < 22) {
        err(EXIT_FAILURE, "Required minimum MTU 22");
    }

    //check if input and output files are different
    if (strcmp(in_file, out_file) == 0) {
        err(EXIT_FAILURE, "Input and Output files must differ");
    }
    

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