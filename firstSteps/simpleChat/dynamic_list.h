//
// Created by sam_s on 26/05/2025.
//

#ifndef UNTITLED6_DYNAMIC_LIST_H
#define UNTITLED6_DYNAMIC_LIST_H


#include <stdlib.h>
#include "header.h"
#include <string.h>
#include <stdbool.h>

#define MAX_LEN 512
typedef struct tItemL{
    SOCKET num_sock;
    char userName[MAX_LEN];
}tItemL;


typedef struct tNode * tPosL;
struct tNode{
    tItemL data;
    tPosL next;
};

typedef struct tNode *tList;

void createEmptyList(tList*);
bool insertItem(tItemL, tList*);
void deleteItem(tPosL, tList*);
tPosL findItem(SOCKET num_sock, tList);
tItemL getItem(tPosL, tList);
bool isEmptyList(tList);
tPosL first(tList);

#endif //UNTITLED6_DYNAMIC_LIST_H
