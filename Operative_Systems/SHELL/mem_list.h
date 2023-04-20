/*--------------------------------PRÁCTICA_2-----------------------------------

Santiago Alfredo Castro Rampesad (s.a.castro.rampersad) (54940335M)
Xian Priego Martín (xian.priego) (39457432N)

GRUPO 4.3
-----------------------------------------------------------------------------*/
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <sys/types.h>
#include <sys/time.h>
#include <time.h>



typedef enum {MALLOC, SHARED, MAPPED, ALL} alloc_t;

typedef struct {
    void* memDir;
    size_t size;
    time_t allocTime;
    alloc_t allocType;
    key_t key;
    char fileName[1024];
    int fileDes;
}tItemM;

typedef struct tNodoM* tPosM;

typedef struct tHNodeM* tHeadPosM;

struct tHNodeM{
    tPosM next;
    tPosM last;
};

struct tNodoM { 
    tItemM item;
    tPosM next;
};

typedef tHeadPosM tListM; 


void createEmptyListM(tListM* L);
bool insertItemM(tItemM d, tListM* L);


void updateItemM(tItemM d, tPosM p);


void deleteAtPositionM(tPosM p, tListM *L);
void deleteListM(tListM* L);
void clearListM(tListM* L);

tPosM findItemSizeM(size_t d, tListM L);
tPosM findItemDirM(void* , tListM L);
tPosM findItemKeyM(key_t, tListM L);
tPosM findItemFileM(char*, tListM L);
bool isEmptyListM(tListM L);
tItemM getItemM(tPosM p);
tPosM firstM(tListM L);
tPosM lastM(tListM L);
tPosM previousM(tPosM p, tListM L);
tPosM nextM(tPosM p);
void printItem(tItemM item);