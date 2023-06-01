/*
 * this commandline program takes a domain name as an argument
 * and will return IPV4 and IPV6 addresses related to it
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>

int isInTrap(char*);

int main(int argc, char** argv)
{
    int status;
    struct addrinfo hint, *result, *address;

    // reset hint to 0
    memset(&hint, 0, sizeof hint);

    // TCP
    hint.ai_socktype = SOCK_STREAM;

    // ipv4 or ipv6
    hint.ai_family = AF_UNSPEC;


    if (argc != 2) {
        fprintf(stderr, "error: invalid format\nusage: showip [ DOMAIN ]\n");
        exit(1);
    }

    if (status = getaddrinfo(argv[1], NULL, &hint, &result) != 0) {
        fprintf(stderr, "error: %s", gai_strerror(status));
        exit(2);
    }
    


    // "address" is used to traverse the linkedlist constructed by getaddrinfo() function
    address = result;

    // traverse the linked list, until you have not reached NULL
    while (address) {
        unsigned short  address_family =  address->ai_addr->sa_family;


        // check whether the address is IPV4
        if (address_family == AF_INET){
            
            char ipv4_str[INET_ADDRSTRLEN];

            // casting sockaddr* into sockaddr_in*
            struct sockaddr_in *ipv4 = (struct sockaddr_in*)(address->ai_addr);

            // converting the network form of IPV4 address into presentation(printable) format
            inet_ntop(AF_INET, &(ipv4->sin_addr), ipv4_str, INET_ADDRSTRLEN);

            // check if you are trapped in local network
            if (isInTrap(ipv4_str)) {
                fprintf(stderr, "error: you are in a trap\ncan not proceed\n");
                exit(2);
            }

            fprintf(stdout, "ipv4: %s\n", ipv4_str);
        }

        // check whether the address is IPV6
        else if (address_family == AF_INET6){
            char ipv6_str[INET6_ADDRSTRLEN];

            // casting sockaddr* into sockaddr_in6(for IPV6)
            struct sockaddr_in6 *ipv6 = (struct sockaddr_in6*)(address->ai_addr);

            // converting the network form of IPV6 address into presentation(printable) format
            inet_ntop(AF_INET6, &(ipv6->sin6_addr), ipv6_str, INET6_ADDRSTRLEN);

            fprintf(stdout, "ipv6: %s\n", ipv6_str);
        }
        
        // go to next node(addrinfo struct) constructed by getaddrinfo() function
        address = address->ai_next;
    }

    // free the memory used by the linkedlist
    freeaddrinfo(result);

    return 0;


}

// this function checks whether the given IPV4 is in form 10.x.x.x
int isInTrap(char* ipv4) {


    int first_part_of_ipv4 = 0;

    // this loop extracts the number before reaching to the first '.',
    // which is the first part of ipv4
    for (int i = 0 ; i < INET_ADDRSTRLEN ; i++) {
        if (ipv4[i] != '.') {
            first_part_of_ipv4 *= 10;
            first_part_of_ipv4 += ipv4[i] - '0';
        }
        else {

            break;

        }

    }
    
    // 10.x.x.x ipv4 addresses are local
    return (first_part_of_ipv4 == 10);
}