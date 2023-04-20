 /*--------------------------------PRÁCTICA_2-----------------------------------

Santiago Alfredo Castro Rampesad (s.a.castro.rampersad) (54940335M)
Xian Priego Martín (xian.priego) (39457432N)

GRUPO 4.3
-----------------------------------------------------------------------------*/

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <errno.h>
#include <sys/ipc.h>
#include <sys/mman.h>
#include <sys/shm.h>
#include <unistd.h>
#include <sys/wait.h>
#include "mem_list.h"

#define TAMANO 2048

void initializeMemList();
void deleteMemList();
bool fAllocate(char** trozos);
bool fDeallocate (char** trozos);
bool fI_0(char** trozos);
bool fMemDump(char** trozos);
bool fMemFill(char** trozos);
bool fMemory(char** trozos);
bool fRecurse(char** trozos);