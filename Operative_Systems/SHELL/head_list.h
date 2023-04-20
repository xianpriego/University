

#include <stdlib.h>
#include <stdbool.h>
#include <strings.h>
#define LNULL NULL

typedef char* tItemL;

typedef struct tNodo* tPosL;

typedef struct tHNode* tHeadPos;

struct tHNode{
    tPosL next;
    tPosL last;
};

struct tNodo { 
    tItemL item;
    tPosL next;
};

typedef tHeadPos tList; 

//GENERADORAS
void createEmptyList(tList* L);
bool insertItem(tItemL d, tPosL p, tList* L);

//MODIFICADORAS
void updateItem(tItemL d, tPosL p);

//DESTRUCTORAS
void deleteAtPosition(tPosL p, tList *L);
void deleteList(tList* L);
void clearList(tList* L);

//OBSERVADORAS
tPosL findItem(tItemL d, tList L);
bool isEmptyList(tList L);
tItemL getItem(tPosL p);
tPosL first(tList L);
tPosL last(tList L);
tPosL previous(tPosL p, tList L);
tPosL next(tPosL p);