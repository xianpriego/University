
#define  _GNU_SOURCE 700
#define _XOPEN_SOURCE 700
#define LENGTH 4096

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <time.h>
#include <sys/stat.h>
#include <fcntl.h> 
#include <errno.h>
#include <pwd.h>
#include <grp.h>
#include <ftw.h>
#include <dirent.h>

bool fDelTree(char** trozos);
bool fCrear(char** trozos);
bool fBorrar(char** trozos);
bool fStat(char** trozos);
bool fList(char** trozos);


