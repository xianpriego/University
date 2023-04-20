 /*--------------------------------PRÁCTICA_2-----------------------------------

Santiago Alfredo Castro Rampesad (s.a.castro.rampersad) (54940335M)
Xian Priego Martín (xian.priego) (39457432N)

GRUPO 4.3
-----------------------------------------------------------------------------*/
#include "mem_list.h"


void createEmptyListM(tListM *L){
    *L = malloc(sizeof(struct tHNodeM));
    (*L)->next = NULL;
    (*L)->last = NULL;

}

bool isEmptyListM(tListM L){
    return L->next == NULL;
}

tItemM getItemM(tPosM p){
    return p->item;
}

    void updateItemM(tItemM d, tPosM p){
        p->item = d;
    }

tPosM findItemSizeM(size_t d, tListM L){
    
    tPosM aux = L->next;

    while(aux!=NULL){
        if(aux->item.size == d && aux->item.allocType == MALLOC) return aux;
        else aux = aux->next;
    }

    return NULL;

}

tPosM findItemDirM(void* d, tListM L){
    
    tPosM aux = L->next;

    while(aux!=NULL){
        if(aux->item.memDir == d) return aux;
        else aux = aux->next;
    }
    return NULL;
}

tPosM findItemKeyM(key_t key, tListM L){
    tPosM aux = L->next;

    while(aux!=NULL){
        if(aux->item.key == key) return aux;
        else aux = aux->next;
    }
    return NULL;
}

tPosM findItemFileM(char* file, tListM L){
      tPosM aux = L->next;

    while(aux!=NULL){
        if(strcmp(aux->item.fileName, file)== 0) return aux;
        else aux = aux->next;
    }
    return NULL;
}

tPosM firstM(tListM L){
    return L->next;
}

tPosM lastM(tListM L){
    
    return L->last;
    
    /*tPosM aux = L->next;

    while(aux->next != NULL){
        aux = aux->next;
    }
    return aux;*/
}

tPosM previousM(tPosM p, tListM L){
    tPosM aux = L->next;

    if(aux==p) return NULL;

    while(aux->next != p){
        aux = aux->next;
    }
    return aux;
}

tPosM nextM(tPosM p){
    return p->next;
}

void deleteAtPositionM(tPosM p, tListM *L){
    tPosM auxPrevious, auxNext = p->next;
   

    if(p==(*L)->next){ //Caso en que el elemento a borrar sea el primero de la lista
        (*L)->next = auxNext;
    }
    else if (auxNext == NULL){ // Caso que sea el ultimo elemento de la lista
        auxPrevious = previousM(p,*L);
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

void deleteListM(tListM* L){
    tPosM auxPos;
    while((*L)->next != NULL){
        auxPos = (*L)->next->next;
        free((*L)->next->item.memDir);
        free((*L)->next);
        (*L)->next = auxPos;
    }
    free(*L);
}

void clearListM(tListM* L){
    tPosM auxPos;
    while((*L)->next != NULL){
        auxPos = (*L)->next->next;
        free((*L)->next->item.memDir);
        free((*L)->next);
        (*L)->next = auxPos;
    }
    (*L)->next = NULL;
    (*L)->last = NULL;

}

bool createNodeM(tPosM *p){
    *p = malloc(sizeof (struct tNodoM));
    if (*p == NULL) return false;
    else return true;
}


bool insertItemM(tItemM d, tListM *L){
    tPosM newNode;

    if (!(createNodeM(&newNode))) return false; //Verificamos que se pudo crear el nodo
    newNode->item = d; //HAY QUE LIBERARLO UWU
    newNode->next = NULL;

    if(isEmptyListM(*L)){
        (*L)->last = newNode;
        (*L)->next = newNode;
    }
    else{
        (*L)->last->next = newNode;
        (*L)->last = newNode;
    }
    return true;

}