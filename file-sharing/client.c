#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>


#define KILOBYTE 1024


int main(int argc, char* argv[]) {

    struct addrinfo hints, *res;
    
    int sockfd, file_size, status, bytes_remained, bytes_received;

    char buff[KILOBYTE];

    // create the file and open it in write mode
    FILE *file = fopen(argv[3], "w");


    if (argc != 3) {
        fprintf(stderr, "invalid optio format\n");
        fprintf(stderr, "USAGE: getfile [ IP | Domain ] [ Port ] [ Dest File ]\n");
    }


    /// Preparing To Construct The linked list

    // reset "hints" to 0
    memset(&hints, 0, sizeof hints);
    
    // IPV4 or IPV6
    hints.ai_family = AF_UNSPEC;

    // TCP
    hints.ai_socktype = SOCK_STREAM;
    

    // create the linkedlist based on the first arg(IP), second arg(port no.) and the info in "hints"
    if ((status = getaddrinfo(argv[1], argv[2], &hints, &res)) != 0) {
        fprintf(stderr, "error: %s", gai_strerror(status));
        exit(1);
    }

    // create a socket with specified details in the first struct of the linkedlist
    if ((sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) == -1) {
        fprintf(stderr, "error: %s\n", strerror(errno));
        exit(2);
    } else {
        fprintf(stdout, "socket is created\n");
    }

    // connect the socket to the server specified in "res->ai_addr"
    if (connect(sockfd, res->ai_addr, res->ai_addrlen) == -1) {
        fprintf(stderr, "error: %s\n", strerror(errno));
        exit(3);
    } else {
        fprintf(stdout, "connected to %s\n", argv[1]);
    }
    
    // receive file size
    if (recv(sockfd, &file_size, sizeof(int), 0) == -1) {
        fprintf(stderr, "error: %s\n", strerror(errno));
        exit(4);
    } else {
        fprintf(stdout, "file size is received\n");
        fprintf(stdout, "file size %i\n", file_size);
    }


    bytes_remained = file_size;

    // continue receiving until there is less than 1K bytes
    while (bytes_remained >= KILOBYTE) {
        if ((bytes_received = recv(sockfd, buff, KILOBYTE, 0)) == -1) {
            fprintf(stderr, "error: %s", strerror(errno));
            exit(5);
        }
        fwrite(buff, sizeof(char), bytes_received, file);

        bytes_remained -= bytes_received;
    }

    // receive and store the last remaining bytes
    while (bytes_remained != 0) {
        if ((bytes_received = recv(sockfd, buff, bytes_remained, 0)) == -1) {
            fprintf(stderr, "error: %s", strerror(errno));
            exit(5);
        }

        fwrite(buff, sizeof(char), bytes_remained, file);

        bytes_remained -= bytes_received;
    }


    // Cleaning Up

    // close the file
    fclose(file);
    
    // close the socket
    close(sockfd);

    // free the memory of linked list
    freeaddrinfo(res);

    return 0;
}