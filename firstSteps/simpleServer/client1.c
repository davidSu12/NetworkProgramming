
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
#include <time.h>


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

    send(socket_peer, message, sizeof(message), 0);

    closesocket(socket_peer);


    WSACleanup();
    return 0;
}