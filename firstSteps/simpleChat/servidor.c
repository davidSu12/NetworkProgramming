#include "header.h"
#include "dynamic_list.h"


int main(int argc, char **argv){

    WSADATA dd;
    tList lista_de_usuarios;


    createEmptyList(&lista_de_usuarios); // inicializamos la lista;
    if(WSAStartup(MAKEWORD(2,2), &dd)){
        fprintf(stderr, "Startup could not be intialized (%d)", WSAGetLastError());
        return -1;
    }

    struct addrinfo hint;
    memset(&hint, 0, sizeof(hint));

    hint.ai_family = AF_INET;
    hint.ai_socktype = SOCK_STREAM;
    hint.ai_flags = AI_PASSIVE;


    struct addrinfo *res;
    if(getaddrinfo(0, "8888", &hint, &res)){
        fprintf(stderr, "Info could not be resolved (%d)", WSAGetLastError());
        return -1;
    }

    SOCKET sock;

    sock = socket(res -> ai_family,
                    res -> ai_socktype,
                    res -> ai_flags);
    if(sock == INVALID_SOCKET){
        fprintf(stderr, "Invalid socket (%d)", WSAGetLastError());
        return -1;

    }
    if(bind(sock, res -> ai_addr, res -> ai_addrlen)){
        fprintf(stderr, "Could not complete the bind (%d)", WSAGetLastError());
        return -1;
    }

    freeaddrinfo(res);

    //we can start to listen

    if(listen(sock, 10) < 0){
        fprintf(stderr, "Listen could not be fullfiled (%d)", WSAGetLastError());
    }

    fd_set master;
    FD_ZERO(&master);
    FD_SET(sock, &master);
    SOCKET max_socket = sock;
    printf("Waiting for connections...\n");

    while(1){
        fd_set reads;
        reads = master;

        if(select(max_socket + 1, &reads, 0, 0, 0) < 0){
            fprintf(stderr,"Select could not be fullfiled (%d)", WSAGetLastError());
        }


        SOCKET i;
        for(i = 0; i < max_socket; i++){
            if(FD_ISSET(i, &reads)){

                if(i == sock){ //this we just simply accept more connections
                    struct tItemL * data = malloc(sizeof(struct tItemL));
                    struct sockaddr_storage client;
                    int client_size = sizeof(client);
                    SOCKET client_socket = accept(sock,
                                                  (struct sockaddr *)&client,
                                                          &client_size);
                    if(client_socket == INVALID_SOCKET){
                        fprintf(stderr, "An error has ocurred while accepting connections");
                    }

                    if(client_socket > max_socket){
                        max_socket = client_socket;
                    }
                    FD_SET(client_socket, &master);
                    char data_client[1024];

                    getnameinfo((struct sockaddr *) &client, client_size,
                            data_client, 1024, 0,0, NI_NUMERICHOST);

                    printf("New connection from %s\n", data_client);


                    strcpy(data->userName, data_client);
                    data ->num_sock = client_socket;
                    insertItem(*data, &lista_de_usuarios);



                }else{ //this we read from user

                    char read[1024];
                    int bytes_received = recv(i, read, 1024, 0);
                    if(bytes_received < 1){
                        FD_CLR(i, &master);
                        CLOSESOCKET(i);
                        continue;
                    }

                    for(tPosL pos = lista_de_usuarios; pos != NULL; pos = pos -> next){
                        send(pos -> data.num_sock, read, 1024, 0);
                        printf("sending to %llu\n",pos->data.num_sock);
                    }

                }
            }
        }

    }


    WSACleanup();
    return 0;
}