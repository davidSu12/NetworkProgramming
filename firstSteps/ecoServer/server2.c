#include <conio.h>
#include "header.h"



int main(void){


    WSADATA d;
    if(WSAStartup(MAKEWORD(2,2), &d)){

        fprintf(stderr, "There was an error (%d)", WSAGetLastError());
        return -1;
    }


    struct addrinfo hint;
    memset(&hint, 0, sizeof(hint));
    hint.ai_family = AF_INET;
    hint.ai_socktype = SOCK_STREAM;
    hint.ai_flags = AI_PASSIVE;
    struct addrinfo *res;


    getaddrinfo(0, "9999", &hint, &res);

    SOCKET sock;
    sock = socket(res -> ai_family,
                  res->ai_socktype,
                  res->ai_protocol);

    if(!ISVALIDSOCKET(sock)){
        fprintf(stderr, "There was an error (%d)", GETSOCKETERRNO());
        return -1;
    }
    if(bind(sock, res->ai_addr, res->ai_addrlen)){
        fprintf(stderr, "There was an error (%d)", GETSOCKETERRNO());
        return -1;
    }
    freeaddrinfo(res);

    if(listen(sock, 10) < 0){
        fprintf(stderr, "An error has ocurred (%d)", GETSOCKETERRNO());
        return -1;
    }

    fd_set master;
    FD_ZERO(&master);
    FD_SET(sock, &master);
    SOCKET max_socket  = sock;


    printf("Waiting for connections...\n");

    while(1){

        fd_set reads;
        reads = master;
        if(select(max_socket+1,&reads, 0,0,0) < 0){
            fprintf(stderr, "An error has ocurred (%d)", GETSOCKETERRNO());
            return -1;
        }

        SOCKET i;
        for(i = 1; i <= max_socket; i++){
            if(FD_ISSET(i, &reads)) {

                if (i == sock) {
                    struct sockaddr_storage client;
                    int size_client = sizeof(client);
                    SOCKET socket_client = accept(sock, (struct sockaddr *) &client, &size_client);

                    if (!ISVALIDSOCKET(socket_client)) {
                        fprintf(stderr, "An error has ocurred", GETSOCKETERRNO());
                        return -1;
                    }
                    FD_SET(socket_client, &master);
                    if (socket_client > max_socket) {
                        max_socket = socket_client;
                    }
                    char address_buffer[1024];
                    getnameinfo((struct sockaddr *) &client, size_client, address_buffer,
                                1024, 0, 0, NI_NUMERICHOST);
                    printf("New connection from %s\n", address_buffer);
                } else {
                    char read[1024];
                    int bytes_received = recv(i, read, 1024, 0);
                    if (bytes_received < 1) {
                        FD_CLR(i, &master);
                        CLOSESOCKET(i);
                        continue;
                    }
                    int j;
                    send(i, read, bytes_received, 0);
                }
            }
        }
    }

    printf("Closing listening socket...");
    closesocket(sock);

    WSACleanup();

    printf("Finished");



    return 0;
}