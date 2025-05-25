#include "header.h"
#include <conio.h>

int main(int argc, char **argv){

    WSADATA d;
    char *message = "Hello world\0";
    if(WSAStartup(MAKEWORD(2,2), &d)){
        fprintf(stderr, "An error has ocurred (%d)", GETSOCKETERRNO());
    }


    struct addrinfo hint;
    memset(&hint, 0, sizeof(hint));
    hint.ai_socktype = SOCK_STREAM;

    struct addrinfo *sock_peer;
    getaddrinfo("127.0.0.1","9999", &hint, &sock_peer);

    SOCKET socket_peer = socket(sock_peer->ai_family,
                                sock_peer->ai_socktype,
                                sock_peer->ai_protocol);

    if(socket_peer == INVALID_SOCKET){
        fprintf(stderr, "An error has ocurred (%d)", WSAGetLastError());
    }
    if(connect(socket_peer, sock_peer ->ai_addr, sock_peer -> ai_addrlen)){
        fprintf(stderr, "An error has ocurred (%d)", WSAGetLastError());
    }

    freeaddrinfo(sock_peer);


    printf("Connected.\n");
    printf("To send data, enter text followed by enter.\n");

    while(1){

        fd_set reads;
        FD_ZERO(&reads);
        FD_SET(socket_peer, &reads);


        struct timeval timeout;
        timeout.tv_sec = 0;
        timeout.tv_usec = 100000;
        if(select(socket_peer+1, &reads,0,0,&timeout) < 0){
            fprintf(stderr, "select() failed. (%d)\n", WSAGetLastError());
            return -1;
        }

        if(FD_ISSET(socket_peer, &reads)){
            char read[4096];
            int bytes_received = recv(socket_peer, read, 4096,0);
            if(bytes_received < 1 ){
                printf("Connection closed by peer");
                break;
            }
            printf("Received (%d bytes):%.*s", bytes_received, bytes_received, read);
        }


        if(_kbhit()){
            char read[4096];
            if(!fgets(read, 4096, stdin)) break;
            printf("Sending: %s", read);
            int bytes_sent = send(socket_peer, read, 4096, 0);
            printf("Sent %d bytes", bytes_sent);
        }

    }

    closesocket(socket_peer);

    WSACleanup();
    return 0;
}