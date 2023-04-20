

#include "p2Code.h"

tListM memList;
float vglobal1 = 9.5, vglobal2 = 0.0, vglobal3 = -4.5;

void printItem(tItemM item);
void printList(alloc_t atype);
void* cadtop(char* cad);

/*-----------------------------------------ALLOCATE-----------------------------------------------*/
tItemM createItem(void* memDir, size_t size, alloc_t atype, key_t key, char* fileName, int fileD){
    tItemM item;
    item.memDir = memDir;
    item.size = size;
    item.allocTime = time(NULL);
    item.allocType = atype;
    item.key = key;
    if(fileName != NULL) strcpy(item.fileName, fileName);
    item.fileDes = fileD;

    return item;
}

void doAllocateMalloc(char** trozos){
    tItemM newItem;
    size_t size; 
    void* memDir;
    if(trozos[0]==NULL)
        printList(MALLOC);//Flag malloc
    else if((size = (size_t)strtoul(trozos[0],NULL,10)) == 0)  //Si lo introducido no es un int 
        printf("No se asignan bloques de %ld bytes\n",size);
    else{
        if((memDir = malloc(size)) == NULL){
            perror("malloc: ");
            return;
        }    
        newItem = createItem(memDir, size, MALLOC, 0, NULL, -1);
        insertItemM(newItem, &memList);
        printf("Asignados %ld bytes en %p\n", size, memDir);
    }

}

void * ObtenerMemoriaShmget (key_t clave, size_t tam)
{
    void * p;
    int aux,id,flags=0777;
    struct shmid_ds s;
    tItemM newItem;

    if (tam)     /*tam distito de 0 indica crear */
        flags=flags | IPC_CREAT | IPC_EXCL;
    if (clave==IPC_PRIVATE)  /*no nos vale*/
        {errno=EINVAL; return NULL;}
    if ((id=shmget(clave, tam, flags))==-1)
        return (NULL);
    if ((p=shmat(id,NULL,0))==(void*) -1){
        aux=errno;
        if (tam)
             shmctl(id,IPC_RMID,NULL);
        errno=aux;
        return (NULL);
    }
    shmctl (id,IPC_STAT,&s);
 /* Guardar en la lista   InsertarNodoShared (&L, p, s.shm_segsz, clave); */
    newItem = createItem(p, s.shm_segsz, SHARED, clave, NULL, -1);
    insertItemM(newItem, &memList); 
    return (p); 
}

void doAllocateCreateshared (char *tr[]){
   key_t cl;
   size_t tam;
   void *p;

   if (tr[0]==NULL || tr[1]==NULL) {
		printList(SHARED); //FLAG SHARED
		return;
   }
  
   cl=(key_t)  strtoul(tr[0],NULL,10);
   tam=(size_t) strtoul(tr[1],NULL,10);
   if (tam==0) {
	printf ("No se asignan bloques de 0 bytes\n");
	return;
   }
   if ((p=ObtenerMemoriaShmget(cl,tam))!=NULL)
		printf ("Asignados %lu bytes en %p\n",(unsigned long) tam, p);
   else
		printf ("Imposible asignar memoria compartida clave %lu:%s\n",(unsigned long) cl,strerror(errno));
}


void doShared(char** trozos){
    key_t cl;
    void *p;

    if(trozos[0] == NULL){
        printList(SHARED);
        return;
    }
    cl = (key_t) strtoul(trozos[0], NULL, 10);
        
    if ((p = ObtenerMemoriaShmget(cl,0)) != NULL)
        printf("Memoria compartida de clave %lu en %p\n",(unsigned long) cl, p);
    else
        printf("Imposible asignar memoria compartida clave %lu:%s\n", (unsigned long) cl, strerror(errno));      
    
}

void * MapearFichero (char * fichero, int protection)
{
    int df, map=MAP_PRIVATE,modo=O_RDONLY;
    struct stat s;
    void *p;
    tItemM newItem;

    if (protection&PROT_WRITE)
          modo=O_RDWR;
    if (stat(fichero,&s)==-1 || (df=open(fichero, modo))==-1)
          return NULL;
    if ((p=mmap (NULL,s.st_size, protection,map,df,0))==MAP_FAILED)
           return NULL;
/* Guardar en la lista    InsertarNodoMmap (&L,p, s.st_size,df,fichero); */
    newItem = createItem(p, s.st_size, MAPPED, 0, fichero, df);
    insertItemM(newItem, &memList);
    return p;
}

void doAllocateMmap(char *arg[])
{ 
     char *perm;
     void *p;
     int protection=0;
     
     if (arg[0]==NULL)
            {printList(MAPPED); return;}
     if ((perm=arg[1])!=NULL && strlen(perm)<4) {
            if (strchr(perm,'r')!=NULL) protection|=PROT_READ;
            if (strchr(perm,'w')!=NULL) protection|=PROT_WRITE;
            if (strchr(perm,'x')!=NULL) protection|=PROT_EXEC;
     }
     if ((p=MapearFichero(arg[0],protection))==NULL)
             perror ("Imposible mapear fichero");
     else
             printf ("fichero %s mapeado en %p\n", arg[0], p);
}


bool fAllocate(char** trozos){
    if(trozos[0] == NULL)
        printList(ALL);
    else if(!strcmp(trozos[0], "-malloc"))
        doAllocateMalloc(trozos+1);
    else if(!strcmp(trozos[0], "-createshared"))
        doAllocateCreateshared(trozos+1);
    else if(!strcmp(trozos[0], "-shared"))
        doShared(trozos+1);
    else if(!strcmp(trozos[0], "-mmap"))
        doAllocateMmap(trozos+1);
    else
        printf("uso: allocate [-malloc|-shared|-createshared|-mmap] ....\n");

    return true;    

}
/*---------------------------------------------------------------------------------------*/
/*-----------------------------------DEALLOCATE------------------------------------------*/

void doDeallocateMalloc(char** trozos){
    tPosM pos;
    size_t size; 
    if(trozos[0]==NULL)
        printList(MALLOC);//Flag malloc
    
    else if((size = (size_t)strtoul(trozos[0],NULL,10)) == 0)  //Si lo introducido no es un int 
        printf("No se asignan bloques de %ld bytes\n",size);
    else{
      pos = findItemSizeM(size, memList);
      if(pos == NULL) printf("No hay bloque de ese tamano asignado con malloc");
      else{
        free(pos->item.memDir);
        deleteAtPositionM(pos, &memList); 
      }

    }

}

void doDeallocateShared(char** trozos){
    tPosM pos;
    key_t cl;

    if(trozos[0] == NULL){
        printList(SHARED);//Flag SHARED
        return;
    }
    else if((cl = (key_t) strtoul(trozos[0], NULL, 10)) == 0)
        printf("No hay bloque de esa clave mapeado en el proceso");
    else{
        pos = findItemKeyM(cl, memList);
        if(pos == NULL)
            printf("No hay bloque de esa clave mapeado en el proceso");
        else{
            if(shmdt(pos->item.memDir) == -1) 
                perror("shmdt: ");
            else deleteAtPositionM(pos, &memList);
        }    
    }
}

void doDeallocateDelKey (char** trozos){
   key_t clave;
   int id;
   char *key=trozos[0];

   if (key==NULL || (clave=(key_t) strtoul(key,NULL,10))==IPC_PRIVATE){
        printf ("      delkey necesita clave_valida\n");
        return;
   }
   if ((id=shmget(clave,0,0666))==-1){
        perror ("shmget: imposible obtener memoria compartida");
        return;
   }
   if (shmctl(id,IPC_RMID,NULL)==-1)
        perror ("shmctl: imposible eliminar memoria compartida\n"); 
}

void doDeallocateMmap(char** trozos){
    tPosM pos;
     
     if (trozos[0] == NULL)
            printList(MAPPED);
     else{
        if ((pos = findItemFileM(trozos[0], memList)) == NULL)
            printf("fichero %s no mapeado", trozos[0]);
        else
            if(close(pos->item.fileDes) == -1)
                perror("close: ");
            else{    
                if(munmap(pos->item.memDir, pos->item.size) == -1)
                    perror("munmap: ");
                else deleteAtPositionM(pos, &memList);
            }    
     }

}

void doDeallocateAddress(char** trozos){
    tPosM pos;
    void* dir = cadtop(trozos[0]);

    if ((pos = findItemDirM(dir, memList)) == NULL)
        printf("Direccion %p no asignada con malloc, shared o mmap\n", dir);
    else{
        if(pos->item.allocType == MALLOC)
            free(pos->item.memDir);
        else if(pos->item.allocType == SHARED){
            if(shmdt(pos->item.memDir) == -1){
                perror("shmdt: "); return;
            }
        }    
        else{
            if(close(pos->item.fileDes) == -1) {
                perror("close: "); return;  
            }
            else{    
                if(munmap(pos->item.memDir, pos->item.size) == -1){ 
                    perror("munmap: "); return;
                }
            }    
        }
        deleteAtPositionM(pos, &memList);
    }   
}

bool fDeallocate (char** trozos){
    if(trozos[0] == NULL)
        printList(ALL);
    else if(!strcmp(trozos[0], "-malloc"))
        doDeallocateMalloc(trozos+1);
    else if(!strcmp(trozos[0], "-delkey"))
        doDeallocateDelKey(trozos+1);
    else if(!strcmp(trozos[0], "-shared"))
        doDeallocateShared(trozos+1);
    else if(!strcmp(trozos[0], "-mmap"))
        doDeallocateMmap(trozos+1);
    else
        doDeallocateAddress(trozos);

    return true;    

}

/*------------------------------------------------------------------------------------*/
/*-------------------------------------I_O--------------------------------------------*/

ssize_t LeerFichero (char *f, void *p, size_t cont)
{
   struct stat s;
   ssize_t  n;  
   int df,aux;

   if (stat (f,&s)==-1 || (df=open(f,O_RDONLY))==-1)
	return -1;     
   if (cont==-1)   /* si pasamos -1 como bytes a leer lo leemos entero*/
	cont=s.st_size;
   if ((n=read(df,p,cont))==-1){
	aux=errno;
	close(df);
	errno=aux;
	return -1;
   }
   close (df);
   return n;
}

void doI_ORead (char** trozos){  
   void *p;
   size_t cont=-1;
   ssize_t n;
   if (trozos[0]==NULL || trozos[1]==NULL){
	printf ("faltan parametros\n");
	return;
   }
   p = cadtop(trozos[1]);  /*convertimos de cadena a puntero*/
   if (trozos[2]!=NULL)
	cont=(size_t) atoll(trozos[2]);

   if ((n=LeerFichero(trozos[0],p,cont))==-1)
	perror ("Imposible leer fichero");
   else
	printf ("leidos %lld bytes de %s en %p\n",(long long) n,trozos[0],p);
}

ssize_t EscribirFichero (char *f, void *p, size_t cont, int overwrite){
   ssize_t  n;
   int df,aux, flags=O_CREAT | O_EXCL | O_WRONLY;

   if (overwrite)
	flags=O_CREAT | O_WRONLY | O_TRUNC;

   if ((df=open(f,flags,0777))==-1)
	return -1;

   if ((n=write(df,p,cont))==-1){
	aux=errno;
	close(df);
	errno=aux;
	return -1;
   }
   close (df);
   return n;
}

void doI_OWrite(char** trozos){
    size_t cont;
    if(!strcmp(trozos[0], "-o")){
        if(trozos[1] == NULL|| trozos[2] == NULL || trozos[3] == NULL){
            printf ("faltan parametros\n");
            return;
        }
        else{
            cont = (size_t) atoll(trozos[3]);
            if(EscribirFichero(trozos[1], cadtop(trozos[2]), cont, 1) == -1)
                perror("EscribirFichero ");
            else
                printf("Escritos %ld bytes en %s desde %s", cont, trozos[1], trozos[2]);
        }    
            
    }
    else{
        if(trozos[0] == NULL|| trozos[1] == NULL || trozos[2] == NULL){
            printf("faltan parametros\n");
            return;
        }     
        else{
            cont = (size_t) atoll(trozos[2]);
            if(EscribirFichero(trozos[0], cadtop(trozos[1]), cont, 0) == -1)
                perror("EscribirFichero ");
            else
                printf("Escritos %ld bytes en %s desde %s\n", cont, trozos[0], trozos[1]);

        }

    }
}

bool fI_0(char** trozos){

    if(!strcmp(trozos[0], "read"))
        doI_ORead(trozos+1);
    else if(!strcmp(trozos[0], "write"))
        doI_OWrite(trozos+1);
    else
        printf("uso: e-s [read|write] ......\n");
    return true;

}

/*------------------------------------------------------------------------------------*/
/*-------------------------------------MEMDUMP--------------------------------------------*/
void memDump25OrLess(char* p, size_t cont){    
    for (int i = 0; i < 2; i++){
        for (int j = 0; j < cont; j++){
            
            if (i)
                printf("%02x ", (int)p[j]);
            else{
                if((int)p[j] == 0xA)
                    printf("\\n ");
                else if((int)p[j] == 0x9)
                    printf("\\t ");
                else
                    printf(" %c ", p[j]);
            }
        }
        puts("");
    }
}


bool fMemDump(char** trozos){
    size_t cont = 25;
    char* p;
    int i;
    if(trozos[0] == NULL)
        return false;
    else{
        if (trozos[1] != NULL){
            if((cont = atoi(trozos[1])) < 0)
                cont = 25;  
        }
        p = (char*)cadtop(trozos[0]);
        printf("Volcando %ld bytes desde la direccion %p\n", cont, p);
        for (i = 0; i < cont / 25; i++)
        {
            memDump25OrLess(p, 25);
            puts("");
            p += 25;
        }
        memDump25OrLess(p, cont % 25);
        puts("");      
       
        return true;
    }    
}


/*---------------------------------------------------------------------------------------------*/
/*-------------------------------------MEMFILL-------------------------------------------------*/

void LlenarMemoria (void *p, size_t cont, unsigned char byte){
  unsigned char *arr=(unsigned char *) p;
  size_t i;

  for (i=0; i<cont;i++)
		arr[i]=byte;
}

bool fMemFill(char** trozos){
    unsigned char byte = 'A';
    size_t cont = 128;

    if(trozos[0] == NULL)
        return false;
    else{
        if(trozos[1] != NULL){
            if((cont = strtoul(trozos[1], NULL, 10)) == 0){
                printf("Faltan argumetos\n");
                return false;
            }
        }

        if(trozos[2] != NULL)
            byte = (unsigned char)trozos[2][0];    
        
        LlenarMemoria(cadtop(trozos[0]), cont, byte);
        printf("Llenando %lu bytes de memoria con el byte %c(%x)"
         " a partir de la direccion %s\n", cont, byte, byte, trozos[0]);
        
        return true;
    }
}


/*---------------------------------------------------------------------------------------------*/
/*-------------------------------------MEMORY-------------------------------------------------*/

void Do_pmap (void) /*sin argumentos*/
 { pid_t pid;       /*hace el pmap (o equivalente) del proceso actual*/
   char elpid[32];
   char *argv[4]={"pmap",elpid,NULL};
   
   sprintf (elpid,"%d", (int) getpid());
   if ((pid=fork())==-1){
      perror ("Imposible crear proceso");
      return;
      }
   if (pid==0){
      if (execvp(argv[0],argv)==-1)
         perror("cannot execute pmap (linux, solaris)");
         
      argv[0]="procstat"; argv[1]="vm"; argv[2]=elpid; argv[3]=NULL;   
      if (execvp(argv[0],argv)==-1)/*No hay pmap, probamos procstat FreeBSD */
         perror("cannot execute procstat (FreeBSD)");
         
      argv[0]="procmap",argv[1]=elpid;argv[2]=NULL;    
            if (execvp(argv[0],argv)==-1)  /*probamos procmap OpenBSD*/
         perror("cannot execute procmap (OpenBSD)");
         
      argv[0]="vmmap"; argv[1]="-interleave"; argv[2]=elpid;argv[3]=NULL;
      if (execvp(argv[0],argv)==-1) /*probamos vmmap Mac-OS*/
         perror("cannot execute vmmap (Mac-OS)");      
      exit(1);
  }
  waitpid (pid,NULL,0);
}

bool fMemory(char** trozos){
    int vlocal1 = 69, vlocal2 = 73, vlocal3 = 6;
    static char vstatic1 = 'U',vstatic2 = 'w', vstatic3 = 'U';
    bool all = false;

    if(trozos[0] == NULL || !strcmp(trozos[0], "-all"))
        all =true;
     
    if( all || !strcmp(trozos[0], "-vars")){
        printf("Variables locales: %19p, %17p, %17p\n", &vlocal1, &vlocal2, &vlocal3);
        printf("Variables globales: %18p, %17p, %17p\n", &vglobal1, &vglobal2, &vglobal3);
        printf("Variables estaticas: %17p, %17p, %17p\n", &vstatic1, &vstatic2, &vstatic3);
    }
    if(all || !strcmp(trozos[0], "-funcs")){
        printf("Funciones programa:  %17p, %17p, %17p\n", fMemory, fMemDump ,fMemFill);
        printf("Funciones libreira:  %17p, %17p, %17p\n", free, open, printf);
    }
    if(all || !strcmp(trozos[0], "-blocks")){
        printList(ALL);
    }
    if(!all && !strcmp(trozos[0], "-pmap")){
        Do_pmap();
    }
    return true;
}

/*---------------------------------------------------------------------------------------------*/
/*-------------------------------------RECURSE-------------------------------------------------*/

void Recursiva(int n)
{
  char automatico[TAMANO];
  static char estatico[TAMANO];

  printf ("parametro:%3d(%p) array %p, arr estatico %p\n",n,&n,automatico, estatico);

  if (n>0)
    Recursiva(n-1);
}

bool fRecurse(char** trozos){
    if(trozos[0] != NULL){
        Recursiva(atoi(trozos[0]));
        return true;
    }
    return false;    
}

/*---------------------------------------------------------------------------------------------*/
/*-------------------------------------AUXILIARES-------------------------------------------------*/

void printItem(tItemM item){
    struct tm* lt;
    lt = localtime(&item.allocTime);
    char* sTime = malloc(32 * sizeof(char));
    strftime(sTime, 32, "%b %d %H:%M", lt);
    
    printf("%p\t\t\t\t%lu %s ", item.memDir, item.size, sTime);

    if(item.allocType == MALLOC) printf("malloc\n");
    else if(item.allocType == SHARED) printf("shared (%u)\n", item.key);
    else printf("%s (descriptor %d)\n", item.fileName, item.fileDes);

    free(sTime);
}

void printList(alloc_t atype){
    tPosM pos = firstM(memList);
    pid_t pid = getpid();

    if(atype == MALLOC) printf ("******Lista de bloques asignados malloc para el proceso %d\n", pid);
    else if(atype == SHARED) printf ("******Lista de bloques asignados shared para el proceso %d\n", pid);
    else if(atype == MAPPED) printf ("******Lista de bloques asignados mmap para el proceso %d\n", pid);
    else printf ("******Lista de bloques asignados para el proceso %d\n", pid);
    
    if (atype != ALL){
        for(; pos != NULL; pos = nextM(pos))
            if(pos->item.allocType == atype)
                printItem(pos->item); 
    }                
    else
        for(; pos != NULL; pos = nextM(pos))
            printItem(pos->item);  

}

void initializeMemList(){
    createEmptyListM(&memList);
}

void deleteMemList(){
    tPosM pos;
    while(memList->next != NULL){
        pos = memList->next->next;
        if(memList->next->item.allocType == MALLOC)
            free(memList->next->item.memDir);
        else if(memList->next->item.allocType == SHARED){
            if(shmdt(memList->next->item.memDir) == -1){
                perror("shmdt: "); return;
            }
        }    
        else{
            if(close(memList->next->item.fileDes) == -1) {
                perror("close: "); return;  
            }
            else{    
                if(munmap(memList->next->item.memDir, memList->next->item.size) == -1){ 
                    perror("munmap: "); return;
                }
            }    
        }
        free(memList->next);
        memList->next = pos;
    }
    free(memList);
}

void* cadtop(char* cad){
    int* pointer;
    sscanf(cad, "%p", &pointer);
    return (void*)pointer;
}