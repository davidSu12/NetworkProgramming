#include "dynamic_list.h"

bool isEmptyList(tList l){
    return l == NULL;
}
void createEmptyList(tList *l){
    (*l) = NULL;
}

bool insertItem(tItemL data, tList *l){

    struct tNode *temp = malloc(sizeof(struct tNode));

    if(!temp){
        fprintf(stderr, "Couldn't allocate more space for list");
        return false;
    }
    temp -> data = data;
    temp -> next = (*l);
    (*l) = temp;
    return true;
}
void deleteItem(tPosL pos, tList *l){

    tPosL prev, curr;

    for(prev = NULL, curr = (*l);
    curr != pos && curr != NULL;
    prev = curr, curr = curr -> next);

    if(prev == NULL){
        (*l) = (*l) -> next;
    }else if(curr == NULL){
        return;
    }else{
        prev -> next = curr -> next;
    }
    free(curr);
}
tPosL findItem(SOCKET num_sock, tList l){

    tPosL temp;

    for(temp = l;
    temp -> data . num_sock != num_sock && temp != NULL;
    temp = temp -> next);

    return temp;

}
tItemL getItem(tPosL pos, tList l){
    return pos -> data;
}

tPosL first(tList l){
    return l;
}