#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <err.h>

#define BUFFER_SIZE 4096 //Buffer size

int create_socket (int port, char* IP) {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) { //error creating socketfd
        err(EXIT_FAILURE, "socket error error");
    }

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof addr);
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port); // Add port to socket addr struct

    //Convert IP str into hex and store into sin_addr of socket struct
    if (inet_pton(AF_INET, IP, &addr.sin_addr) <= 0) { // check if IP address is valid
        err(EXIT_FAILURE, "invalid ip address");
    }

    if (connect(sockfd, (struct sockaddr*) &addr, sizeof addr)) {
        err(EXIT_FAILURE, "connection error");
    }
    return sockfd;
}

// Loop through remaining bytes(content_len) from response and
// write them to output file
void finish_get(int clientfd, int content_len) {
    printf("finishing get request\n");
    int bytes_recv;
    char* buffer[BUFFER_SIZE];
    memset(&buffer[0], 0, sizeof(buffer));

    // open the file if it exists, otherwise create it 
    int file = open("output.dat", O_WRONLY | O_TRUNC | O_CREAT, 00700);

    // file exitst/was created
    if (file != -1) {
        for (int i = 0; i < (content_len / BUFFER_SIZE) + 1; i++) {
      
        if (i < content_len / BUFFER_SIZE) { // if i < byteCount / buffSize we can fill entire buffer
            bytes_recv = recv(clientfd, buffer, BUFFER_SIZE, 0); 
            write(file, buffer, bytes_recv);
        } else {                               // else we read byteCount - (buffSize * i) bytes
            bytes_recv = recv(clientfd, buffer, content_len - (BUFFER_SIZE * i), 0);
            write(file, buffer, bytes_recv);
        }
            memset(&buffer[0], 0, sizeof(buffer));    // clear buffer for next read/write
        }
        close(file);
    } else {
        err(EXIT_FAILURE, "File failed to open");
    }
    return;
}

void recv_header(int clientfd, int type) {
    int recv_bytes;
    int content_len;
    int end_of_line = 0;
    char buffer[10];
    char cont_length_buff[200];
    char line[BUFFER_SIZE];
    char response[BUFFER_SIZE];

    // set buffers to 0's
    memset(&cont_length_buff[0], 0, sizeof(cont_length_buff));
    memset(&buffer[0], 0, sizeof(buffer));
    memset(&line[0], 0, sizeof(line));
    memset(&response[0], 0, sizeof(response));

    while(1) {
        recv_bytes = recv(clientfd, buffer, 1, 0); // receive 1 byte at a time

         // if prev at end of line and next character is \r we move on
        if ((strcmp(&buffer[0], "\r") == 0) && end_of_line) {
            recv_bytes = recv(clientfd, buffer, 1, 0); // one last recv to get last \n
            strcat(response, buffer); //append last \n to string
            break;
        } else {
            end_of_line = 0;
        }

        strcat(line, buffer); // append buffer to line string

        // when we reach a newline character we know we are at the end of a line
        if (strcmp(&buffer[0], "\n") == 0) {

            printf("Current Line: %s\n", line);

            end_of_line = 1; // set end of line character to true
        
            //Parse line for centent length
            sscanf(line, "%s", cont_length_buff);
            if (strcmp(cont_length_buff, "Content-Length:") == 0) {
                sscanf(line, "%*s %i", &content_len); // get content_len value from line
            }
            strcat(response, line); //concatonte line onto entire response
            memset(&line[0], 0, sizeof(line)); // reset line string for next line
        }
    }

    printf("response: %s\n", response);
    printf("%i\n", content_len);

    if(type == 1) { // type 1 we have a GET request
        finish_get(clientfd, content_len);
    } else { // else we have a HEAD request
        // print out entire header to stdout
        printf("%s",response);
    }

    return;
}

void handle_get (int clientfd, char* item, char* host) {
    char request[1042];
    memset(&request[0], 0, sizeof(request));

    // Format request string
    sprintf(request, "GET %s HTTP/1.1\r\nHost: %s\r\n\r\n", item, host);
    printf("SENDING: %s", request);
    int num_sent = send(clientfd, request, strlen(request), 0); // send request
    printf("bytes sent: %i\n", num_sent);
    // check num_sent ***

    //receive the header
    recv_header(clientfd, 1);
    return;
}

void handle_head(int clientfd, char* item, char* host) {
    char request[1042];
    memset(&request[0], 0, sizeof(request));

    // Format request string
    sprintf(request, "HEAD %s HTTP/1.1\r\nHost: %s\r\n\r\n", item, host);
    printf("SENDING: %s", request);
    int num_sent = send(clientfd, request, strlen(request), 0); // send request
    printf("bytes sent: %i\n", num_sent);
    // check num_sent ***

    //receive the header
    recv_header(clientfd, 0);
    return;
}

void handle_args(char *argv[], int head_flag) {
    printf("handling args\n");
    char str[1024];
    char host[1024];
    char ip[100];
    char ip_and_port[1024];
    char item[1024] = "/";
    char *token;
    int port;

    strcpy(host, argv[1]); // Save host from 1st arg
    strcpy(str, argv[2]); //save ip, port, and item in new string
    printf("Start of parsing: %s | %s\n", host, str);

    //parsing args for IP, Port, and Item
    token = strtok(str, "/"); //get first token
    printf("TOKEN: %s\n", token);
    if (strcmp(token, str) != 0) { // if token and orig string arnt equal, item exists
        strcpy(ip_and_port, token); // first token will be IP and Port
        printf("1000, ip_and_port: %s\n", ip_and_port);
        token = strtok(NULL, "/"); //get second token will be item
        printf("TOKEN: %s\n", token);
        strcat(item, token); // cat token onto "/" to get complete item
        printf("item: %s\n", item);
        token = strtok(ip_and_port, ":"); //tokenize for IP and port
        printf("TOKEN: %s\n", token);
        if (strcmp(token, ip_and_port) != 0) { //port exists if token != ip_and_port
            strcpy(ip, token); // first token will be IP
            token = strtok(NULL, ":"); //get second token will be port
            port = atoi(token); //covert port to int
        } else { // else there is no port
            strcpy(ip, ip_and_port); // Copy ip
            port = 80; // set port to default 80
        }
    } else { // else if token == str, item is not present
        strcpy(ip_and_port, token); // first token will be IP and Port
        strcpy(item, "/index.html"); // default item will be index.html
        token = strtok(ip_and_port, ":"); //tokenize for IP and port
        if (strcmp(token, ip_and_port) != 0) { //port exists
            strcpy(ip, token); // first token will be IP
            token = strtok(NULL, ":"); //get second token will be port
            port = atoi(token); //covert port to int
        } else { // else there is no port
            strcpy(ip, ip_and_port); // Copy ip
            port = 80; // set port to default 80
        }
    }
    printf("Port: %i\nIP: %s\n", port, ip);

    // create socket
    int clientfd = create_socket(port, ip);
    
    printf("Item: %s\nHost: %s\n", item, host);
    if (head_flag) { // if head flag true we send a head request
        handle_head(clientfd, item, host);
    } else { // else we send a get request
        handle_get(clientfd, item, host);
    }
    return;
} 

int main(int argc, char *argv[]) {
    printf("argc: %i\n", argc);

    // check for correct number of args
    if (argc < 3 || argc > 5) {
        err(EXIT_FAILURE, "Incorrect number of arguments");
    }

    int head_flag = 0;
    if (argc == 4) {
        if (strcmp(argv[3], "-h") == 0) { // if -h arg is present, we set the head_flag
            head_flag = 1;
        }
    }
    
    printf("Head_flag: %i\n", head_flag);

    // start by handling arguments
    handle_args(argv, head_flag);

    //int clientfd = create_socket(80, "93.184.216.34");
    //printf("socket Created: %i\n", clientfd);
    // check socket for failure ***

    //handle_get(clientfd, "/index.html", "www.example.com");

    /*
    //sending request
    char request[1042];
    memset(&request[0], 0, sizeof(request));
    sprintf(request, "GET /index.html HTTP/1.1\r\nHost: www.example.com\r\n\r\n");

    int num_sent = send(clientfd, request, strlen(request), 0);
    printf("Bytes sent: %i\n", num_sent);

    // receiving response
    char response[1042];
    memset(&response[0], 0, sizeof(response));
    int bytes_recv = recv(clientfd, response, 1024, 0);

    printf("Bytes Recieved: %i\n", bytes_recv);
    printf("%s\n", response);
    */
    
    exit(0);
}


