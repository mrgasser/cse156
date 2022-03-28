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

int alarm_bool = 0; //global bolean for alarm

// function sends the output file path to the server
void send_output_file(int sockfd, char* out_file, int mtu, const struct sockaddr* dest_addr, socklen_t servlen) {
    int bytes_sent;
    char buffer[1024];
    memset(&buffer[0], 0, sizeof(buffer));

    // maybe need to check mtu before sending output file name *********
    // Path name should be less than mtu *****

    //place output file path into buffer with 0 seq number
    sprintf(buffer, "0 %s", out_file);

    // send file path to server
    bytes_sent = sendto(sockfd, buffer, strlen(buffer), 0, dest_addr, servlen);
    if (bytes_sent == -1) {
            err(EXIT_FAILURE, "Error sending data to server");
    }

}

void resend_window() {
    alarm_bool = 1; // set alarm bool to true
}

void go_back_n(int sockfd, int windsz, int mtu, FILE* in_file, const struct sockaddr* dest_addr, socklen_t servlen) {
    int send_base = 0;
    int nextseqnum = 0;
    int recv_bytes;
    int sent_bytes;
    int ACKnum;
    char buffer[mtu - HEADER_SIZE];
    char send_buffer[mtu];
    char recv_buffer[mtu + 1];
    char response[1024];

    // window array to store mtu sized messages to be sent
    char *window[windsz];

    struct timeval read_timeout;
    read_timeout.tv_sec = 0;
    read_timeout.tv_usec = 10; // set timeout for 10 microseconds
    setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &read_timeout, sizeof(read_timeout)); // add recv timeout on socket

    signal(SIGALRM, resend_window); //Register alarm signal to call sig_handler function

    while (fgets(buffer, mtu - HEADER_SIZE, in_file) != NULL) {
        if(nextseqnum < send_base + windsz) {
            sprintf(send_buffer, "%i DATA %s", nextseqnum, buffer); // add seqeunce number and data to packet
            memcpy(window[nextseqnum % windsz], send_buffer, strlen(send_buffer)); // copy packet into window
            printf("window String %s\n", window[nextseqnum % windsz]); 
            
            // send packet
            sent_bytes = sendto(sockfd, send_buffer, strlen(send_buffer), 0, dest_addr, servlen);
            if (sent_bytes == -1) {
                err(EXIT_FAILURE, "Error sending data to server");
            }
            alarm(30); // 30 sec timeout for recieving ACKS
            nextseqnum = nextseqnum + 1;
        }
        
        //non blocking recv
        recv_bytes = recvfrom(sockfd, recv_buffer, mtu, 0, NULL, NULL);
        if (recv_bytes != -1) {
            sscanf(buffer, "%i %s %*s", ACKnum, response); // parse for ack num and reponse
            send_base = send_base + 1;
            if (send_base == nextseqnum) {
                alarm(0);
                memset(&window[ACKnum % windsz], 0 , sizeof(window[ACKnum % windsz])); // reset ACK address
            }
            //succesful recv from 
            //reset timer
            //remove recv seq num from array
        }
        if(alarm_bool) {
            int temp = send_base;
            while(temp < nextseqnum) {
                memcpy(send_buffer, window[temp % windsz], strlen(window[temp % windsz])); // copy packet from windo
                sent_bytes = sendto(sockfd, send_buffer, strlen(send_buffer), 0, dest_addr, servlen);
                if (sent_bytes == -1) {
                    err(EXIT_FAILURE, "Error sending data to server");
                }
            }
            alarm_bool = 0; //reset alarm boolean
        }
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
    go_back_n(sockfd, window_size, mtu, in_fp, (struct sockaddr*) &addr, sizeof(addr));

    exit(0); //exit successful
}
