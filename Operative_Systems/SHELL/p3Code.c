#include "p3Code.h"

extern char** arg3;
extern char** environ;
extern void* arg3p;

tListP proList;

int BuscarVariable (char * var, char *e[]);
int CambiarVariable(char * var, char * valor, char *e[]);
int OurExecvpe(const char *file, char *const argv[], char *const envp[]);
char** twoDimArray(int row, int col);
int envVariableArr(char** trozos, char** ourEnv);
int argsArr(char** trozos, char** args);
int TrocearPath2(char * cadena, char * trozos[]);
char * Ejecutable (char *s);
void freeTwoDimArray(char** arr, int row);
void printEnv(char** env,char* str);
void printItemPro(tItemP i);
char *NombreSenal(int sen);
void updateStatus2(tPosP pos, int options);

bool fPriority(char** trozos){
    int pid, prio;

    if (trozos[0] == NULL)
        pid = getpid();
    else pid = (int)strtoul(trozos[0], NULL, 10);

    if(trozos[1] == NULL)
        printf("Prioridad del proceso %d es %d\n", pid, getpriority(PRIO_PROCESS,pid));
    else{
        prio = (int)strtoul(trozos[1], NULL, 10);
        errno = 0;
        setpriority(PRIO_PROCESS, pid, prio);
        if(errno != 0){
            perror("imposible cambiar la prioridad del proceso"); //VER SI HAY QUE PONER EL PID
            return false;
        }    
    }

    return true;
                
}

bool fShowvar(char** trozos){
    int i = 0;
    char* genv;
    if(trozos[0]==NULL){
       printEnv(arg3, "main arg3");
    }
    else{
        if((i = BuscarVariable(trozos[0], arg3)) != -1)
            printf("Con arg3 main %s(%p) @%p\n",arg3[i], arg3[i], arg3+i);
        if((i = BuscarVariable(trozos[0], environ)) != -1)
            printf("\tCon environ %s(%p) @%p\n",environ[i], environ[i], environ+i);
        if((genv = getenv(trozos[0])) != NULL)
            printf("\t\tCon getenv %s(%p)\n", genv, genv);
        
    }
    return true;
}

bool fChangevar(char** trozos){
    int i = 0;
    if(trozos[0] == NULL || trozos[1] == NULL || trozos[2] == NULL)
        printf("Uso: changevar [-a|-e|-p] var valor\n");
    else{
        if(!strcmp(trozos[0], "-a")){
            if((i = CambiarVariable(trozos[1], trozos[2], arg3)) == -1){
                perror("Imposible cambiar variable");
                return false;
            }    
        }
        else if(!strcmp(trozos[0], "-e")){
            if((i = CambiarVariable(trozos[1], trozos[2], environ)) == -1){
                perror("Imposible cambiar variable");
                return false;
            }    
        }
        else if(!strcmp(trozos[0], "-p")){
            if(setenv(trozos[1], trozos[2], 1) == -1){
                perror("setenv");
                return false;
            }    
        }
        else{
            printf("Uso: changevar [-a|-e|-p] var valor\n");
        }
    }    
    return true;

}

bool fShowenv(char** trozos){
    if(trozos[0]==NULL)
        printEnv(arg3, "main arg3");
    else if(!strcmp(trozos[0], "-environ"))
        printEnv(environ, "environ");
    else if(!strcmp(trozos[0], "-addr")){
        printf("environ:\t%p (almacenada en %p)\n", environ, &environ);
        printf("main arg3:\t%p (almacenada en %p)\n", arg3, arg3p);
    }
    else{
        printf("Uso: showenv [-environ|-addr]\n");
    } 

    return true;
}

bool fFork (char** trozos){
	pid_t pid;
	
	if ((pid=fork()) == 0){
		clearListP(&proList);
		printf ("ejecutando proceso %d\n", getpid());
	}
	else if (pid!=-1)
		waitpid (pid,NULL,0);
    else{
        perror("fork");
        return false;  
    }
    return true;
}

bool fExecute(char** trozos){
    char** ourEnv = twoDimArray(50,1024),
           **args = twoDimArray(15, 64);       
    char exec[256];   
    int i = 0;

    i = envVariableArr(trozos, ourEnv);
    trozos += i;
    strcpy(exec, trozos[0]);
    
    trozos += argsArr(trozos, args);

    if(trozos[0] != NULL){
        errno = 0;
        setpriority(PRIO_PROCESS, getpid(), strtoul(trozos[0]+1, NULL, 10));
        if(errno != 0){
            perror("imposible cambiar la prioridad del proceso");
            freeTwoDimArray(ourEnv, 50);
            freeTwoDimArray(args, 15);
            return false;
        }
    }

    if(i == 0){
        if(execvp(exec, args) == -1)
            perror("Imposible ejecutar ");   
    }
    else{
        if (OurExecvpe(exec, args, ourEnv) == -1)
            perror("Imposible ejecutar ");
    }

    freeTwoDimArray(ourEnv, 50);
    freeTwoDimArray(args, 15);
    return false;
    
}

bool fAsterisk(char** trozos){
   char** ourEnv = twoDimArray(50,1024),
           **args = twoDimArray(15, 64);       
    char exec[256];
    char copyCommand[256] = {""};
    pid_t pid; 
    int i = 0, j = 0;
    int status = 0;
    bool priority = false;
    tItemP item;

    while(trozos[j] != NULL){
        strcat(copyCommand, trozos[j]);
        strcat(copyCommand, " ");
        j++;
    }
   
    i = envVariableArr(trozos, ourEnv);
    trozos += i;
    strcpy(exec, trozos[0]);
    trozos += argsArr(trozos, args);
    
    if(priority = trozos[0] != NULL && trozos[0][0] == '@')
        trozos++;
    if((pid = fork()) == 0){
        if(priority){
            errno = 0;
            setpriority(PRIO_PROCESS, getpid(), strtoul((trozos-1)[0]+1, NULL, 10));
            if(errno != 0){
                perror("imposible cambiar la prioridad del proceso");
                freeTwoDimArray(ourEnv, 50);
                freeTwoDimArray(args, 15);
                exit(EXIT_FAILURE);
            }
            trozos++;
        }
        
        if(i == 0){
            if(execvp(exec, args) == -1){
                perror("Imposible ejecutar ");
                freeTwoDimArray(ourEnv, 50);
                freeTwoDimArray(args, 15);
                exit(EXIT_FAILURE);
                return false;
            }    
        }
        else{
            if (OurExecvpe(exec, args, ourEnv) == -1){
                perror("Imposible ejecutar ");
                freeTwoDimArray(ourEnv, 50);
                freeTwoDimArray(args, 15);
                exit(EXIT_FAILURE);
                return false;
            }
        }
        
    }
    else if (pid != -1 && trozos[0] != NULL && trozos[0][0] == '&'){
        waitpid(pid, &status, WNOHANG|WUNTRACED);//Sí entra por aquí, da igual que se pueda ejecutar o no, se mete en la lista
        item.allocTime = time(NULL);
        item.pid = pid;
        item.status = ACTIVE;
        strcpy(item.commandline, copyCommand);
        item.value = status;
        insertItemP(item, &proList); 
    }
    else if (pid != -1){
        waitpid(pid, NULL,0);
    }
    else{
        perror("fork");
        freeTwoDimArray(ourEnv, 50);
        freeTwoDimArray(args, 15);
        return false;
    }

    freeTwoDimArray(ourEnv, 50);
    freeTwoDimArray(args, 15);
    return true;
}


bool fListjobs(char** trozos){
    tPosP auxPos = firstP(proList);
    tItemP auxItem;

    while(auxPos != NULL){
        updateStatus2(auxPos, WNOHANG|WUNTRACED);
        auxItem = getItemP(auxPos);
        printItemPro(auxItem);
        auxPos = nextP(auxPos);
    }
    return true;
}


bool fDeljobs(char** trozos){
    tPosP auxPos, auxPos2;
    tItemP auxItem;

    if(trozos[0] != NULL && !strcmp(trozos[0], "-term")){
        auxPos = firstP(proList);
        while(auxPos != NULL){
            updateStatus2(auxPos, WNOHANG|WUNTRACED);
            auxItem = getItemP(auxPos);
            auxPos2 = nextP(auxPos);
            if(auxItem.status == FINISHED)
                deleteAtPositionP(auxPos, &proList);
            
            auxPos = auxPos2;
        }
    
    }
    else if(trozos[0] != NULL && !strcmp(trozos[0], "-sig")){
        auxPos = firstP(proList);
        while(auxPos != NULL){
            updateStatus2(auxPos, WNOHANG|WUNTRACED);
            auxItem = getItemP(auxPos);
            auxPos2 = nextP(auxPos);
            if(auxItem.status == SIGNALED)
                deleteAtPositionP(auxPos, &proList);
            
            auxPos = auxPos2;
        }
    
    }
    
    fListjobs(trozos);
    return true;
}

bool fJob(char** trozos){
    tPosP auxPos;
    tItemP auxItem;

    
    if(trozos[1] != NULL && !strcmp(trozos[0], "-fg")){ //Traemos el proceso a primer plano
        pid_t pid = strtoul(trozos[1], NULL, 10);
        auxPos = findItemP(pid, proList);
        if (auxPos != NULL){ 
            auxItem = getItemP(auxPos);
            if(auxItem.status == ACTIVE || auxItem.status == STOPPED){
                updateStatus2(auxPos, WUNTRACED);
                auxItem = getItemP(auxPos);
                printItemPro(auxItem);
                if(auxItem.status == FINISHED)
                    printf("proceso %d terminado normalmente. Valor devuelto %d\n",
                     auxItem.pid, auxItem.value);
                else 
                    printf("Proceso %d terminado por la senal %s\n", auxItem.pid, NombreSenal(auxItem.value));
       
                deleteAtPositionP(auxPos, &proList);
            }
        } 
    }
    else if(trozos[0] != NULL){ //Printeamos información sobre ese proceso concreto, usamos finditem
        auxPos = findItemP(strtoul(trozos[0], NULL, 10), proList);
        if (auxPos != NULL){
            auxItem = getItemP(auxPos);
            printItemPro(auxItem);
        }
    }

}


/*-------------------------------CODIGO-AUXILIAR--------------------------*/
int CambiarVariable(char * var, char * valor, char *e[]) /*cambia una variable en el entorno que se le pasa como parámetro*/
{                                                        /*lo hace directamente, no usa putenv*/
  int pos;
  char *aux;
   
  if ((pos=BuscarVariable(var,e))==-1)
    return(-1);
 
  if ((aux=(char *)malloc(strlen(var)+strlen(valor)+2))==NULL)
	return -1;
  strcpy(aux,var);
  strcat(aux,"=");
  strcat(aux,valor);
  e[pos]=aux;
  return (pos);
}

int BuscarVariable (char * var, char *e[])  /*busca una variable en el entorno que se le pasa como parámetro*/
{
  int pos=0;
  char aux[MAXVAR];
  
  strcpy (aux,var);
  strcat (aux,"=");
  
  while (e[pos]!=NULL)
    if (!strncmp(e[pos],aux,strlen(aux)))
      return (pos);
    else 
      pos++;
  errno=ENOENT;   /*no hay tal variable*/
  return(-1);
}

void printEnv(char** env, char* str){
    int i = 0;
    char** aux = env;
     while(*aux != NULL){
            printf("%p -> %s[%d]=(%p)  %s\n",aux, str, i, *aux, *aux);
            aux++;
            i++;
    } 
}

void initializeProList(){
    createEmptyListP(&proList);
}

char * Ejecutable (char *s)
{
	char path[MAXNAME];
	static char aux2[MAXNAME];
	struct stat st;
	char *p;
	if (s==NULL || (p=getenv("PATH"))==NULL)
		return s;
	if (s[0]=='/' || !strncmp (s,"./",2) || !strncmp (s,"../",3))
        return s;       /*is an absolute pathname*/
	strncpy (path, p, MAXNAME);
	for (p=strtok(path,":"); p!=NULL; p=strtok(NULL,":")){
       sprintf (aux2,"%s/%s",p,s);
	   if (lstat(aux2,&st)!=-1)
		return aux2;
	}
	return s;
}

int OurExecvpe(const char *file, char *const argv[], char *const envp[])
{
   return (execve(Ejecutable(file),argv, envp));
}

char** twoDimArray(int row, int col){
    char** arr = (char**)malloc(row*sizeof(char*));
    for(int i = 0; i<row; i++)
        arr[i] = (char*)malloc(col*sizeof(char));
    return arr;    
}

void freeTwoDimArray(char** arr, int row){

    for(int i = 0; i < row; i++)
        free(arr[i]);
    free(arr);    
}

int TrocearPath2(char * cadena, char * trozos[]){
    int i=1;
    if ((trozos[0]=strtok(cadena,"/"))==NULL)
        return 0;
    while ((trozos[i]=strtok(NULL,"/"))!=NULL)
        i++;
    return i;
}

int envVariableArr(char** trozos, char** ourEnv){
    int var, i = 0;

    while((var = BuscarVariable(trozos[0], environ)) != -1){
        strcpy(ourEnv[i], environ[var]);
        trozos++;
        i++;
    }
    free(ourEnv[i]);
    ourEnv[i] = NULL;
    return i;
}

int argsArr(char** trozos, char** args){
    char **path;
    int tokens, i = 1;
    strcpy(args[0], trozos[0]);
    if(args[0][0] == '/' || !strncmp(args[0], "./", 2) || !strncmp(args[0], "../", 3)){
        path = malloc(20*sizeof(char*));
        tokens = TrocearPath2(args[0], path);
        strcpy(args[0], path[tokens-1]);
        free(path);
    }
    trozos++;
    while(trozos[0] != NULL && trozos[0][0] != '@' && trozos[0][0] != '&'){
        strcpy(args[i], trozos[0]);
        trozos++;
        i++;
    }
    free(args[i]);
    args[i] = NULL;
    return i;
}

void printItemPro(tItemP i){

    struct tm* lt;
    lt = localtime(&i.allocTime);
    char sTime[32]; 
    strftime(sTime, 32, "%x %X", lt);

    struct passwd* pw;
    pw = getpwuid(getuid());
    if(i.status != SIGNALED){
        printf("%d      %s p=%d %s %s(%03d) %s\n",
         i.pid, pw->pw_name, getpriority(PRIO_PROCESS, i.pid), sTime, 
         (i.status == FINISHED)? "FINISHED": (i.status == STOPPED? "STOPPED" : "ACTIVE"),
         i.value, i.commandline);

    }
    else{
        // HAY QUE LLAMAR LO DEL CODIGO DE AYUDA PARA IMPRIMIR BIEN EL VALUE
        printf("%d      %s p=%d %s SIGNALED (%s) %s\n",
         i.pid, pw->pw_name, getpriority(PRIO_PROCESS, i.pid), sTime, NombreSenal(i.value), i.commandline);
    } 
}

void updateStatus2(tPosP pos, int options){
    
    tItemP auxItem;
    int status = 0;

    auxItem = getItemP(pos);
    if (waitpid(auxItem.pid, &status, options) == auxItem.pid){ 
        
        if(WIFCONTINUED(status)){
            auxItem.status = ACTIVE;
            auxItem.value = status;
        }    
        else if(WIFEXITED(status)){
            auxItem.status = FINISHED;
            auxItem.value = WEXITSTATUS(status);
        }   
        else if(WIFSTOPPED(status)){
            auxItem.status = STOPPED;
            auxItem.value = WSTOPSIG(status);
        }    
        else if(WIFSIGNALED(status)){
            auxItem.status = SIGNALED;
            auxItem.value = WTERMSIG(status);
        }       
    }
    updateItemP(auxItem, pos);
      
}

void deleteProList(){
    deleteListP(&proList);
}

char *NombreSenal(int sen)
{			
 int i;
  for (i=0; sigstrnum[i].nombre!=NULL; i++)
  	if (sen==sigstrnum[i].senal)
		return sigstrnum[i].nombre;
 return ("SIGUNKNOWN");
}