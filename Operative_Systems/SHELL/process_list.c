#include "process_list.h"

void createEmptyListP(tListP *L){
    *L = malloc(sizeof(struct tHNodeP));
    (*L)->next = NULL;
    (*L)->last = NULL;

}

bool isEmptyListP(tListP L){
    return L->next == NULL;
}

tItemP getItemP(tPosP p){
    return p->item;
}

void updateItemP(tItemP d, tPosP p){
    p->item = d;
}

void deleteAtPositionP(tPosP p, tListP *L){
    tPosP auxPrevious, auxNext = p->next;
   

    if(p==(*L)->next){ //Caso en que el elemento a borrar sea el primero de la lista
        (*L)->next = auxNext;
    }
    else if (auxNext == NULL){ // Caso que sea el ultimo elemento de la lista
        auxPrevious = previousP(p,*L);
        auxPrevious->next = NULL;
        (*L)->last = auxPrevious;
    }
    else{ //Caso de un elemento en medio de la lista
        p->item = auxNext->item;    //Para que sea mas eficiente, pasamos el contenido del nodo p->next a p y borramos
        p->next = auxNext->next;    //p->next
        p = auxNext;
    }

    free(p);
}
tPosP findItemP(pid_t pid, tListP L){
    
    tPosP aux = L->next;

    while(aux!=NULL){
        if(aux->item.pid == pid) return aux;
        else aux = aux->next;
    }

    return NULL;

}

tPosP firstP(tListP L){
    return L->next;
}

tPosP lastP(tListP L){
    return L->last;
}

tPosP previousP(tPosP p, tListP L){
    tPosP aux = L->next;

    if(aux==p) return NULL;

    while(aux->next != p){
        aux = aux->next;
    }
    return aux;
}

tPosP nextP(tPosP p){
    return p->next;
}

void deleteListP(tListP* L){
    tPosP auxPos;
    while((*L)->next != NULL){
        auxPos = (*L)->next->next;
        free((*L)->next);
        (*L)->next = auxPos;
    }
    free(*L);
}

void clearListP(tListP* L){
    tPosP auxPos;
    while((*L)->next != NULL){
        auxPos = (*L)->next->next;
        free((*L)->next);
        (*L)->next = auxPos;
    }
    (*L)->next = NULL;
    (*L)->last = NULL;

}

bool createNodeP(tPosP *p){
    *p = malloc(sizeof (struct tNodoP));
    if (*p == NULL) return false;
    else return true;
}


bool insertItemP(tItemP d, tListP *L){
    tPosP newNode;

    if (!(createNodeP(&newNode))) return false; //Verificamos que se pudo crear el nodo
    newNode->item = d; 
    newNode->next = NULL;

    if(isEmptyListP(*L)){
        (*L)->last = newNode;
        (*L)->next = newNode;
    }
    else{
        (*L)->last->next = newNode;
        (*L)->last = newNode;
    }
    return true;

}