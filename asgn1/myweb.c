#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>

#define BUFFER_SIZE 4096 //Buffer size
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

/*
char* handle_args(int argc, char *argv[]) {
    char s[1024];
    char host[1024];
    char ip_adr[1024];

    // check for correct number of args
    if (argc < 3) {
        // error wrong number of argument
    }

    // get host, check if its valid
    // the get IP, check if IP has a port,
    // parse of item
    
    // loop through and build up string of arguments
    for (int i = 2; i < argc; i++) {
        memset(&s[0], 0, sizeof(s));
        strcat(s, argv[i]);
        //printf("%s ", s);
    }
    printf("arguments: %s", s);

    // Parse the host from the arguments
    sscanf(s, "%s %*s", host);

    const char c [2] = "/";

    char* first_token = strtok(s, c);

} 
*/

char* recv_header(int clientfd, int type) {
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

    if(type == 1) {
        finish_get(clientfd, content_len);
        //Get request
    }

    char *str = "";
    strcpy(str, response);

    return str;
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
    char* header = recv_header(clientfd, 1);
    // Receive response
    // Write response to output.dat
    printf("Header completed: %s\n", header);
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
    // check socket for failure ***

    handle_get(clientfd, "/index.html", "www.example.com");

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
    

    close(clientfd); // close the connection
}


