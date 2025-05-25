
#if defined(_WIN32)
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0600
#endif
#include <winsock2.h>
#include <ws2tcpip.h>
#define ISVALIDSOCKET(S) ((S) != INVALID_SOCKET)
#define CLOSESOCKET(S) closesocket(S)
#define GETSOCKETERRNO() (WSAGetLastError())
#pragma comment(lib, "ws2_32.lib")
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>
#define SOCKET int
#define ISVALIDSOCKET(S) ((S) >= 0)
#define CLOSESOCKET(S) close(S)
#define GETSOCKETERRNO() (errno)

#endif

#include <stdio.h>
#include <stdlib.h>


int main(int argc, char **argv){

    WSADATA d;
    if(WSAStartup(MAKEWORD(2,2), &d)){
        fprintf(stderr, "An error has ocurred (%d)", GETSOCKETERRNO());
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


    sock = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if(!ISVALIDSOCKET(sock)){
        fprintf(stderr, "An error has ocurred", GETSOCKETERRNO());
    }


    if(bind(sock, res->ai_addr, res->ai_addrlen)){
        fprintf(stderr, "An error has ocurred (%d)", GETSOCKETERRNO());
        return -1;
    }


    freeaddrinfo(res);


    //listen

    if(listen(sock, 10) < 0){
        fprintf(stderr, "listen has failed", GETSOCKETERRNO());
        return -1;
    }


    struct sockaddr_storage client;
    int size_client = sizeof(client);
    SOCKET sock_client = accept(sock, (struct sockaddr *)&client, &size_client);
    if(!ISVALIDSOCKET(sock_client)){
        fprintf(stderr, "An error has ocurred", GETSOCKETERRNO());
        return -1;
    }



    char client_buffer[1024];
    getnameinfo((struct sockaddr *)&client, size_client,
            client_buffer,sizeof(client_buffer),
            0,0, NI_NUMERICHOST);
    printf("%s",client_buffer);


    char buff[1024];
    int bytes_received = recv(sock_client, buff, sizeof(buff), 0);
    printf("Bytes received %d\n", bytes_received);
    printf("The message received was: \n%*s", bytes_received, buff);


    CLOSESOCKET(sock_client);
    CLOSESOCKET(sock);


    WSACleanup();
    return 0;
}
