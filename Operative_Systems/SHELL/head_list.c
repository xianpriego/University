 /*--------------------------------PRÁCTICA_1-----------------------------------

Santiago Alfredo Castro Rampesad (s.a.castro.rampersad) (54940335M)
Xian Priego Martín (xian.priego) (39457432N)

GRUPO 4.3
-----------------------------------------------------------------------------*/
 
 #include "head_list.h"
 #include <string.h>


void createEmptyList(tList *L){
    *L = malloc(sizeof(struct tHNode));
    (*L)->next = LNULL;
    (*L)->last = LNULL;

}

bool isEmptyList(tList L){
    return L->next == LNULL;
}

tItemL getItem(tPosL p){
    return p->item;
}

void updateItem(tItemL d, tPosL p){
    p->item = d;
}

/*tPosL findItem(tItemL d, tList L){
    
    tPosL aux = L->next;

    while(aux!=LNULL){
        if(strcmp(aux->item,d) == 0) return aux;
        else aux = aux->next;
    }

    return LNULL;

}*/

tPosL first(tList L){
    return L->next;
}

tPosL last(tList L){
    
    return L->last;
    
    /*tPosL aux = L->next;

    while(aux->next != LNULL){
        aux = aux->next;
    }
    return aux;*/
}

tPosL previous(tPosL p, tList L){
    tPosL aux = L->next;

    if(aux==p) return LNULL;

    while(aux->next != p){
        aux = aux->next;
    }
    return aux;
}

tPosL next(tPosL p){
    return p->next;
}

void deleteAtPosition(tPosL p, tList *L){
    tPosL auxPrevious, auxNext = next(p);
    
    free(p->item);

    if(p==(*L)->next){ //Caso en que el elemento a borrar sea el primero de la lista
        (*L)->next = auxNext;
    }
    else if (auxNext == LNULL){ // Caso que sea el ultimo elemento de la lista
        auxPrevious = previous(p,*L);
        auxPrevious->next = LNULL;
        (*L)->last = auxPrevious;
    }
    else{ //Caso de un elemento en medio de la lista
        p->item = auxNext->item;    //Para que sea mas eficiente, pasamos el contenido del nodo p->next a p y borramos
        p->next = auxNext->next;    //p->next
        p = auxNext;
    }

    free(p);
}

void deleteList(tList* L){
    tPosL auxPos;
    while((*L)->next != LNULL){
        auxPos = (*L)->next->next;
        free((*L)->next->item);
        free((*L)->next);
        (*L)->next = auxPos;
    }
    free(*L);
}

void clearList(tList* L){
    tPosL auxPos;
    while((*L)->next != LNULL){
        auxPos = (*L)->next->next;
        free((*L)->next->item);
        free((*L)->next);
        (*L)->next = auxPos;
    }
    (*L)->next = LNULL;
    (*L)->last = LNULL;

}

bool createNode(tPosL *p){
    *p = malloc(sizeof (struct tNodo));
    if (*p == LNULL) return false;
    else return true;
}


bool insertItem(tItemL d, tPosL p, tList *L){
    tPosL auxPosition, newNode;
    tItemL auxItem;

    if (!(createNode(&newNode))) return false; //Verificamos que se pudo crear el nodo
    newNode->item = (char*)(malloc(256)); //HAY QUE LIBERARLO UWU
    strcpy(newNode->item, d);
    newNode->next = LNULL;

    if((*L)->next == LNULL){ //Caso para introducir el cuando la lista está vacía
        (*L)->next = newNode;
        (*L)->last = newNode;
    }

    else if(p == LNULL){ //Caso para introducir un elemento al final de la lista
        auxPosition = last(*L);
        auxPosition->next = newNode;
        (*L)->last = newNode;
    }

    else if(p == (*L)->next){ //Caso para introducir en la primera posicion de la lista
        auxPosition = (*L)->next;
        (*L)->next = newNode;
        (*L)->next->next = auxPosition;
    }
    else{ //Caso para introducir en medio de la lista
        /* Para no recorrer la lista, introducimos un nodo despues de p,
         * e intercambiamos el item de p (p->item) con el nuevo item (d).*/
        auxPosition = next(p);
        p->next = newNode;
        newNode->next = auxPosition;
        auxItem = p->item;
        p->item = newNode->item;
        newNode->item = auxItem;

    }

    return true;

}




