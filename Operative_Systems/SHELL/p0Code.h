/*--------------------------------PRÁCTICA_1-----------------------------------

Santiago Alfredo Castro Rampesad (s.a.castro.rampersad) (54940335M)
Xian Priego Martín (xian.priego) (39457432N)

GRUPO 4.3
-----------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/utsname.h>
#include <errno.h>
#include "types.h"
#include "head_list.h"
#include "p1Code.h"
#include "p2Code.h"
#include "p3Code.h"

#define N 512
#define M 32

void iniciarLista();
void imprimirPrompt();
void leerEntrada(char* cadena);
bool procesarEntrada(char* cadena);
bool fAutores(char** trozos);
bool fPid(char** trozos);
bool fCarpeta(char** trozos);
bool fFecha(char** trozos);
bool fHist(char** trozos);
bool fComando(char** trozos);
bool fInfosis(char** trozos);
bool fAyuda(char** trozos);
bool fFin(char** trozos);

