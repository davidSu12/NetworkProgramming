#include "header.h"
#include <conio.h>


int main(void){

    WSADATA d;
    if(WSAStartup(MAKEWORD(2,2),&d)){
        fprintf(stderr, "An error has ocurred while initializating", WSAGetLastError());
        return -1;
    }

    struct addrinfo hint;
    memset(&hint, 0, sizeof(hint));

    hint.ai_socktype = SOCK_STREAM;

    struct addrinfo *res;
    getaddrinfo("127.0.0.1", "8888", &hint, &res);

    SOCKET sock = socket(res -> ai_family,
                         res->ai_socktype,
                         res->ai_protocol);
    if(!ISVALIDSOCKET(sock)){
        fprintf(stderr, "Invalid Socket %d",WSAGetLastError());
        return -1;
    }

    printf("Connecting...\n");

    if(connect(sock, res -> ai_addr, res->ai_addrlen)){
        fprintf(stderr, "Connect() failed (%d)",WSAGetLastError());
        return -1;
    }

    printf("Connected...\n");

    freeaddrinfo(res);

    while(1){
        fd_set set;
        FD_ZERO(&set);
        FD_SET(sock, &set);
        SOCKET max_sock = sock;

        struct timeval time;
        time.tv_sec = 0;
        time.tv_usec = 1000000;

        if(select(max_sock+1, &set, 0,0, &time) < 0){
            fprintf(stderr, "select() failed miserably (%d)",WSAGetLastError());
            return -1;
        }
        if(FD_ISSET(sock, &set)){ //ready to read
            char read[1024];
            int bytes_received = recv(sock, read, 1024, 0);
            if(bytes_received < 1){
                printf("Connection closed");
                break;
            }
            printf("%.*s\n", bytes_received, read);
        }

        if(_kbhit()){
            char write[1024];
            if(!fgets(write, 1024, stdin)) break;
            printf("Sending: %s\n", write);

            int bytes_sent = send(sock, write, 1024, 0);
        }
    }


    WSACleanup();
    return 0;
}