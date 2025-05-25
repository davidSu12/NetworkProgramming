
#if defined(_WIN32)
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0600
#endif
#include <winsock2.h>
#include <ws2tcpip.h>
#define ISVALIDSOCKET(S) ((S) != INVALID_SOCKET)
#define CLOSESOCKET(S) closesocket(S)
#define GETSOCKETERRNO() (WSAGetLastError())
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

int main() {
    time_t timer;
    time(&timer);
    WSADATA d;
    if(WSAStartup(MAKEWORD(2,2), &d)){
        fprintf(stderr, "Failted to intialize");
        return -1;

    }

    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;  //ipv4
    hints.ai_socktype = SOCK_STREAM; //TCP
    hints.ai_flags = AI_PASSIVE; //we are searching for an IP to do bind

    struct addrinfo *bind_address;
    getaddrinfo(0, "9999",&hints, &bind_address);

    SOCKET socket_listen;
    socket_listen = socket(bind_address -> ai_family,
                           bind_address ->ai_socktype,
                           bind_address -> ai_protocol);

    if(!ISVALIDSOCKET(socket_listen)){
        fprintf(stderr, "Socket() has failed",GETSOCKETERRNO());
        return -1;
    }

    //bind
    if(bind(socket_listen, bind_address -> ai_addr, bind_address -> ai_addrlen)){
        fprintf(stderr, "bind() failed", GETSOCKETERRNO());
        return -1;
    }
    freeaddrinfo(bind_address);
    printf("Listening...\n");

    if(listen(socket_listen, 10) < 0){
        fprintf(stderr, "listen has failed", GETSOCKETERRNO());
        return -1;
    }

    printf("Waiting for connections\n");
    struct sockaddr_storage client_address;
    socklen_t client_len = sizeof(client_address);
    SOCKET socket_client = accept(socket_listen, (struct sockaddr *) &client_address, &client_len);
    if(!ISVALIDSOCKET(socket_client)){
        fprintf(stderr, "the socket for the client has failed", GETSOCKETERRNO());
        return -1;
    }

    printf("Client is connected...");
    char address_buffer[100];
    getnameinfo((struct sockaddr *) &client_address,
            client_len, address_buffer, sizeof(address_buffer), 0, 0,
            NI_NUMERICHOST);
    printf("%s\n", address_buffer);


    printf("Reading request...\n");
    char request[1024];
    int bytes_received = recv(socket_client, request, 1024, 0);
    printf("Received %d bytes.\n", bytes_received);
    printf("%.*s", bytes_received, request);


    printf("Sending response...\n");
    const char *response=
            "HTTP/1.1 200 OK\r\n"
            "Connection: close\r\n"
            "Content-Type: text/plain\r\n\r\n"
            "Local time is: ";
    int bytes_sent = send(socket_client, response, strlen(response), 0);
    printf("Sent %d of %d bytes.\n", bytes_sent, (int)strlen(response));

    char *time_msg = ctime(&timer);
    bytes_sent = send(socket_client, time_msg, strlen(time_msg),0);
    printf("Sent %d of %d bytes.\n", bytes_sent, (int) strlen(time_msg));

    printf("Closing connection...\n");
    CLOSESOCKET(socket_client);
    printf("Closiing listening socket...\n");
    CLOSESOCKET(socket_listen);

    WSACleanup();
    return 0;
}
