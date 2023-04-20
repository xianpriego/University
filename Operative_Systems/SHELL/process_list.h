#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <sys/types.h>
#include <sys/time.h>
#include <time.h>




typedef enum {FINISHED, STOPPED, SIGNALED, ACTIVE} status_t;

typedef struct {
    pid_t pid;
    time_t allocTime;
    status_t status;
    int value;
    char commandline[256];
}tItemP;

typedef struct tNodoP* tPosP;

struct tNodoP{
    tItemP item;
    tPosP next;
};

typedef struct tHNodeP* tHeadPosP;
struct tHNodeP{
    tPosP next;
    tPosP last;
};



typedef tHeadPosP tListP;

void createEmptyListP(tListP* L);
bool insertItemP(tItemP d, tListP* L);

void updateItemP(tItemP d, tPosP p);

void deleteAtPositionP(tPosP p, tListP *L);
void deleteListP(tListP* L);
void clearListP(tListP* L);

bool isEmptyListP(tListP L);
tItemP getItemP(tPosP p);
tPosP findItemP(pid_t pid, tListP L);
tPosP firstP(tListP L);
tPosP lastP(tListP L);
tPosP previousP(tPosP p, tListP L);
tPosP nextP(tPosP p);
