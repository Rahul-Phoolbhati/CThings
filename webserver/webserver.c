#include<stdio.h>
#include <sys/socket.h>
#include "proxy_parse.h"
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>

typedef struct ParsedRequest ParsedRequest;


void print_hostent(struct hostent *host) {
    if (host == NULL) {
        printf("Host information not found.\n");
        return;
    }

    // Print Official Name
    printf("Official Name: %s\n", host->h_name);

    // Print Aliases
    printf("Aliases:\n");
    if (host->h_aliases != NULL) {
        for (char **alias = host->h_aliases; *alias != NULL; alias++) {
            printf("  - %s\n", *alias);
        }
    }

    // Print Address Type and Length
    printf("Address Type: %s\n", (host->h_addrtype == AF_INET) ? "AF_INET (IPv4)" : "AF_INET6 (IPv6)");
    printf("Address Length: %d bytes\n", host->h_length);

    // Print IP Addresses
    printf("IP Addresses:\n");
    if (host->h_addr_list != NULL) {
        for (char **addr = host->h_addr_list; *addr != NULL; addr++) {
            char ip_str[INET6_ADDRSTRLEN];
            inet_ntop(host->h_addrtype, *addr, ip_str, sizeof(ip_str));
            printf("  - %s\n", ip_str);
        }
    }
}

char* getDummyRequest(){
    char *request = "GET http://example.com/ HTTP/1.1\r\n"
                "Host: example.com\r\n"
                "Connection: close\r\n\r\n";
    return request;
}

int main(){ 
    
    int socket_fd = socket(AF_INET, SOCK_STREAM, 6);
    if (socket_fd == -1) {
        printf("Could not create socket");
        return 1;
    }

    char* request = getDummyRequest();
    printf("%s\n", request);
    int len = strlen(request);
    ParsedRequest* req = ParsedRequest_create();
    if(ParsedRequest_parse(req, request, len)<0){
        printf("parse failed\n");
        exit(EXIT_FAILURE);
    }
    printf("%s\n, ", req->host);
    struct hostent* hp = gethostbyname(req->host);


    print_hostent(hp);
    struct sockaddr_in server_address;

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(80);
    memcpy(&server_address.sin_addr.s_addr, hp->h_addr_list[0], hp->h_length);
    if(connect(socket_fd, (struct sockaddr *)&server_address, sizeof(server_address))<0){
        printf("Error connecting server \n");
        close(socket_fd);
        exit(EXIT_FAILURE);
    }
    char response[4096];
    if(send(socket_fd, request, strlen(request) ,0)<0){
        printf("error sending http request \n");
        close(socket_fd);
        exit(EXIT_FAILURE);
    }
    int byte_recvd;
    while((byte_recvd = recv(socket_fd, response, strlen(response), 0))>0){
        response[byte_recvd]= '\0';
        printf("%s", response);
    }


    
    return 0;
}