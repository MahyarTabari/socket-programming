#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>


#define BACKLOG 20
#define KILOBYTE 1024


int file_size(FILE*);

int main(int argc, char* argv[]) {

    struct addrinfo hints, *res;
    
    // for storing client address info
    struct sockaddr_storage client_addr;

    int fs, listener_fd, client_server_fd, status, sent_bytes, bytes_left, client_addrsize;

    // to read from the local file in the server
    char buff[KILOBYTE], ip_str[INET_ADDRSTRLEN];


    // reset hints to 0
    memset(&hints, 0, sizeof hints);

    FILE *file = fopen(argv[2], "r");

    fs = file_size(file);

    fprintf(stdout, "file \"%s\" is going to be transferred.\nsize: %i bytes\n", argv[2], fs);



    /// Preparing To Construct The linked list


    // ipv4 or ipv6
    hints.ai_family = AF_UNSPEC;

    // TCP
    hints.ai_socktype = SOCK_STREAM;

    // uncomment this line and set the "localhost" to NULL for usage in real server
    // hints.ai_flags = AI_PASSIVE;

    
    // getaddrinfo() constructs the linkedlist res containing the addrinfo structs matching the info in 'hints'
    if (status = getaddrinfo("localhost", argv[1], &hints, &res) != 0) {
        fprintf(stderr, "error: %s\n", gai_strerror(status));
        exit(1);
    }

    // create a socket and store the returned file discriptor
    fprintf(stdout, "creating listener socket...\n");
    if ((listener_fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) == -1) {
        fprintf(stderr, "error: can not create the socket\n");
        exit(2);
    } else {
        fprintf(stdout, "socket is created\n");
    }

    // bind the created socket to the port specified in "res->ai_addr"
    fprintf(stdout, "binding the socket to the port %s", argv[1]);
    if (bind(listener_fd, res->ai_addr, res->ai_addrlen) == -1) {
        fprintf(stderr, "error: %s\n", strerror(errno));
        exit(3);
    } else {
        fprintf(stdout, "binding is done successfully\n");
    }
    

    // listen on the specified socket, with pending queue of maximum size "BACKLOG" 
    if (listen(listener_fd, BACKLOG) == -1) {
        fprintf(stderr, "error: %s\n", strerror(errno));
        exit(4);
    } else {
		fprintf(stderr, "listening on the port %s...\n", argv[1]);
	}

    client_addrsize = sizeof client_addr;

    // accept the connection request of the first client in the queue,
    // and store the address of the client in "client_addr"
    if ((client_server_fd = accept(listener_fd, (struct sockaddr *)&client_addr, &client_addrsize)) == -1) {
        fprintf(stderr, "error: %s\n", strerror(errno));
        exit(5);
    } else {

		// get the presentation form of the client ip
    
        // IPV4
		if (((struct sockaddr*)(&client_addr))->sa_family == AF_INET) {

		    inet_ntop(AF_INET, &(((struct sockaddr_in*)(&client_addr))->sin_addr), ip_str, INET_ADDRSTRLEN); 

        } 

        // IPV6
        else if (((struct sockaddr*)(&client_addr))->sa_family == AF_INET6){

		    inet_ntop(AF_INET6, &(((struct sockaddr_in6*)(&client_addr))->sin6_addr), ip_str, INET_ADDRSTRLEN);

        }
        
		fprintf(stdout, "connected to %s\n", ip_str); 
	}

    // send the file size to the client
    if (send(client_server_fd, &fs, sizeof(int), 0) == -1) {
        fprintf(stderr, "error: can not receive\n");
        exit(6);
    }

    bytes_left = fs;
    fprintf(stdout, "sending the file to %s\n", ip_str);

    // continue reading from the local file in blocks of 1K bytes
    while (bytes_left >= KILOBYTE) {
        
        // read from the file in blocks of 1K byte and put them into "buff"
        fread(buff, sizeof(char), KILOBYTE, file);

        // send the data from "buff" to the client,
        // and store the no. of sent bytes in "sent_bytes"
        if ((sent_bytes = send(client_server_fd, buff, KILOBYTE, 0)) == -1) {
            fprintf(stderr, "error: %s", strerror(errno));
            exit(6);
        }

        bytes_left -= sent_bytes;
    }
    
    // send the remained bytes(which are less than 1K bytes)
    if (bytes_left != 0) {
        fread(buff, sizeof(char), bytes_left, file);
        if ((sent_bytes = send(client_server_fd, buff, bytes_left, 0)) == -1) {
            fprintf(stderr, "error: %s", strerror(errno));
            exit(7);
        }
        
        bytes_left -= sent_bytes;
    }

    fprintf(stdout, "file is sent successfully\n");

        

    // Cleaning Up

    // close the sockets
    close(listener_fd);
    close(client_server_fd);

    // close the file
    fclose(file);

    // free the memory of the linkedlist
    freeaddrinfo(res);

    return 0;

}

/// returns file size of the given file in bytes
int file_size(FILE* fp) {

    // set the file indicator to the end of the file
    fseek(fp, 0L, SEEK_END);

    // tell the current position of file indicator
    int size = ftell(fp);
    
    // put the file descriptor to the beginning of the file
    fseek(fp, 0L, SEEK_SET);

    return size;
}
