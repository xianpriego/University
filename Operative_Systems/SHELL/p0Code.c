/*--------------------------------PRÁCTICA_1-----------------------------------

Santiago Alfredo Castro Rampesad (s.a.castro.rampersad) (54940335M)
Xian Priego Martín (xian.priego) (39457432N)

GRUPO 4.3
-----------------------------------------------------------------------------*/
#include "p0Code.h"

tList cList;


void imprimirPrompt(){
    printf("🦧☭🦍 "); 
}

void leerEntrada(char* cadena){
    fgets(cadena, N, stdin);
}

void iniciarLista(){
  createEmptyList(&cList);
}

int TrocearCadena(char * cadena, char * trozos[]){
    int i=1;
    if ((trozos[0]=strtok(cadena," \n\t"))==NULL)
        return 0; 
    while ((trozos[i]=strtok(NULL," \n\t"))!=NULL)
        i++;
    return i;
}

bool procesarEntrada(char *cadena){
  char **trozos;
  char* copiaCadena;
  int i; /*nTrozos;*/
  bool seguir = true, existe = false;

  if(strcmp(cadena, "\n") != 0){

    copiaCadena = (char*)malloc(N);
    strcpy(copiaCadena, cadena);

    tCmd comandos[] = {{"autores", fAutores},
                      {"pid", fPid},
                      {"carpeta", fCarpeta},
                      {"fecha", fFecha},
                      {"infosis", fInfosis},
                      {"hist", fHist},
                      {"comando", fComando},
                      {"ayuda", fAyuda},
                      {"fin", fFin},
                      {"salir", fFin},
                      {"bye", fFin},
                      {"crear", fCrear},
                      {"borrar", fBorrar},
                      {"stat", fStat},
                      {"list", fList},
                      {"deltree", fDelTree},
                      {"allocate",fAllocate},
                      {"deallocate", fDeallocate},
                      {"i-o",fI_0},
                      {"memdump", fMemDump},
                      {"memfill", fMemFill},
                      {"memory", fMemory},
                      {"recurse", fRecurse},
                      {"priority", fPriority},
                      {"showvar", fShowvar},
                      {"changevar", fChangevar},
                      {"showenv", fShowenv},
                      {"fork", fFork},
                      {"execute", fExecute},
                      {"listjobs", fListjobs},
                      {"deljobs", fDeljobs},
                      {"job", fJob},
                      {NULL, NULL}
                      };
    trozos = (char**)malloc(M * sizeof(char*)); 
    /*nTrozos = */ TrocearCadena(cadena, trozos);

    for (i = 0; comandos[i].nombre != NULL; i++) { 
      if(!strcmp(trozos[0], comandos[i].nombre)){
        if (comandos[i].funcion_comando(trozos+1)){
          seguir = !(strcmp("fin", comandos[i].nombre) == 0 ||
                  strcmp("salir", comandos[i].nombre) == 0 || 
                  strcmp("bye", comandos[i].nombre) == 0);
          if(seguir)    
            insertItem(strtok(copiaCadena,"\n"), LNULL, &cList);
        } 
        existe = true;
      }
    }
    if(!existe){ 
      if(fAsterisk(trozos))
        insertItem(strtok(copiaCadena,"\n"), LNULL, &cList);
    }
    free(trozos);
    free(copiaCadena);
  }
  return seguir;
}


bool fAutores(char **trozos){
  tAutor autor1 = {"Xian Priego Martin", "xian.priego@udc.es"},
         autor2 = {"Santiago Alfredo Castro Rampersad", "s.a.castro.rampersad@udc.es"};
  
  if (trozos[0]==NULL){
    printf("Autor 1: %s\nLogin 1: %s\nAutor 2: %s\nLogin 2: %s\n", 
            autor1.nombre, autor1.login, autor2.nombre, autor2.login);
  } 
  else if (!strcmp(trozos[0], "-l")){
    printf("Login 1: %s\nLogin 2: %s\n", autor1.login, autor2.login);
  } //Printeamos logins
  else if (!strcmp(trozos[0], "-n")){ 
    printf("Autor 1: %s\nAutor 2: %s\n", 
            autor1.nombre, autor2.nombre);
  }
  else{
    return false;
  }
  return true;
}

bool fPid(char **trozos){
  if(trozos[0] != NULL && !strcmp(trozos[0], "-p")){
    printf("Parent pid: %d\n", getppid());
  }
  else{
    printf("pid: %d\n", getpid());
  }
  return true;
}

bool fCarpeta(char **trozos){
  char* ruta;
  ruta = malloc(512 * sizeof(char));
  
  if(trozos[0]== NULL){
    if(getcwd(ruta, sizeof(char)*512) == NULL){;
      perror("getcwd");
      free(ruta);
      return false;
    }else 
      printf("Directorio Actual: %s\n", ruta);
  }
  else{
    if (chdir(trozos[0]) < 0){
      perror("chdir");
      free(ruta);
      return false;
    }
  }
  free(ruta);
  return true;
}

bool fFecha(char** trozos){
  char* str;
  time_t t;
  struct tm* lt;
  time(&t);
  lt = localtime(&t);
  str = malloc(32 * sizeof(char));
  
	if(trozos[0] == NULL){
    strftime(str, 32, "|%d\\%m\\%y - %X|", lt);
		printf("Fecha y hora actual: %s\n", str);
	}
	else if(!strcmp(trozos[0], "-d")){
    strftime(str, 32, "|%d\\%m\\%y|", lt);
    printf("Fecha actual: %s\n", str);
	}
	else if(!strcmp(trozos[0], "-h")){
    strftime(str, 32, "|%X|", lt);
    printf("Hora actual: %s\n", str);

	}
  else{
	free(str);
  return false;
  }
  free(str);
  return true;
}

bool fHist(char** trozos){
	tPosL i;
  int indice = 0, opcionN;
  char* hist;
  

  if (trozos[0] == NULL){
    insertItem("hist", LNULL, &cList);
    for(i = first(cList); i != LNULL; i = next(i)){
        indice++;
        printf("%d -> %s\n", indice, getItem(i));
    }
    
  } 
  else if (!strcmp(trozos[0], "-c")){
    clearList(&cList);
  }
  else{
    
    opcionN = -atoi(trozos[0]);

    if(!opcionN){
      return false;
    }
    else{
      hist = (char*)(malloc(32));
      strcpy(hist,"hist ");
      strcat(hist, trozos[0]);
      insertItem(hist, LNULL, &cList);
      for(i = first(cList); i != LNULL && indice < opcionN ; i = next(i)){
          indice++;
          printf("%d -> %s\n", indice, getItem(i));
          
      }
      free(hist);
    }
  }
  
  return false;
  
}

bool fComando(char** trozos){
	int nCmd, i, indice = 0;
	tPosL aux = first(cList);

  if (trozos[0] == NULL){
    for(; aux != LNULL; aux = next(aux)){
        indice++;
        printf("%d -> %s\n", indice, getItem(aux));
    }
  }
  else{
    nCmd = atoi(trozos[0]);
  
    for(i = 1;aux != LNULL && i < nCmd; i++)
      aux = next(aux);

    if (aux != LNULL && nCmd != 0)
      procesarEntrada(getItem(aux));
    else
      printf("No hay elemento %s\n", trozos[0]);
  }  

	return false;
}

bool fInfosis(char** trozos){
  struct utsname sisinfo;
  if (trozos[0] != NULL){
    printf("Comando no valido\n");
    return false;
  }
  uname(&sisinfo);
  printf("System name: %s\nNode name: %s\nRelease: %s\nVersion: %s\nMachine: %s\n",
          sisinfo.sysname, sisinfo.nodename, sisinfo.release, sisinfo.version, sisinfo.machine);
  return true;

}

bool fFin(char** trozos){
  deleteList(&cList);
  deleteMemList();
  deleteProList();
  return true;
}


bool fAyuda(char** trozos){
    ayudaCmd cmdAyuda[] = {{"autores", "[-l|-n]", "Imprime los nombres y los logins de los autores.\n"
                                "-l sólo imprime los logins.\n-n sólo imprime los nombres."},
                               {"pid","[-p]", "Imprime el pid del proceso que se está ejecutando actualmente" 
                                 "en el shell.\n-p imprime el pid del proceso padre del shell."},
                               {"carpeta", "[direct]", "Cambia el directorio actual del shell a [direct].\n"
                                 "Si se invoca sin argumentos este muestra el directorio actual de trabajo."},
                               {"fecha", "[-d|-h]", "Sin argumentos imprime la fecha y la hora actual.\n"
                                 "-d muestra únicamente la fecha.\n-h imprime únicamente la hora."},
                               {"hist","[-c|-N]", "Sin argumento muestra el histórico de comandos válidos ejecutados.\n"
                                "-c limpia el histórico de comandos.\n-N muestra los primeros N comandos."},
                               {"Comando", "[N]", "Repite el comando N (del historico)"},
                               {"infosis", "", "Imprime la información de la máquina."},
                               {"ayuda", "[cmd]", "Sin argumentos muestra la lista de comandos disponibles.\n"
                               "ayuda [cmd] muestra una ayuda resumida del uso del comando [cmd]."},
                               {"fin", "", "Salir del shell"},
                               {"salir","", "Salir del shell"},
                               {"bye", "", "Salir del shell"},
                               {"crear","[-f][name]","Crea un directorio o un fichero (-f)"},
                               {"borrar","[name1 name2 ..]","Borra ficheros o directorios vacios"},
                               {"stat", "[-long][-link][-acc][name1 name2 ..]","lista ficheros\n-long: listado largo\n"
                                "-acc: acesstime\n-link: si es enlace simbolico, el path contenido" },
                               {"list","[-recb][-reca][-hid][-long][-link][-acc][name1 name2 ..]","lista contenidos de directorios"
                                "-hid: incluye los ficheros ocultos\n-recb: recursivo (antes)\n-reca: recursivo (despues)\n"
                                "-long: listado largo\n-acc: acesstime\n-link: si es enlace simbolico, el path contenido"},
                               {"deltree", "[name1 name2 ..]","Borra ficheros o directorios no vacios recursivamente"},
                               {NULL, NULL, NULL}};
                               
    if(trozos[0] == NULL){
        puts("LISTA DE COMANDOS DISPONIBLES:");
        for(int i=0; cmdAyuda[i].cmd != NULL; i++){
            printf("- %s %s\n", cmdAyuda[i].cmd, cmdAyuda[i].ops);
        }
        return true;

    }
    else {
        for(int j = 0; cmdAyuda[j].cmd != NULL; j++){
            if(!strcmp(cmdAyuda[j].cmd, trozos[0])){
                printf("%s %s\n%s\n", cmdAyuda[j].cmd, cmdAyuda[j].ops, cmdAyuda[j].help);
                return true;
            }
        }
        
    } 
    return false; 
}