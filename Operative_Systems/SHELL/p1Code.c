/*--------------------------------PRÁCTICA_1-----------------------------------

Santiago Alfredo Castro Rampesad (s.a.castro.rampersad) (54940335M)
Xian Priego Martín (xian.priego) (39457432N)

GRUPO 4.3
-----------------------------------------------------------------------------*/
#include "p1Code.h"

char LetraTF (mode_t m);
char * ConvierteModo (mode_t m, char *permisos);
void delTreeRec(char* path);
void printRuta();
void printFStat(char* trozos, bool Long, bool acc, bool link);
void listDir(char* path, bool Long, bool acc, bool link, bool hid);
void listDirRec(char* path, bool Long, bool acc, bool link, bool hid, bool reca, bool recb);

bool fCrear(char** trozos){
  if(trozos[0]==NULL)
    return false;
  if(!strcmp(trozos[0], "-f") && trozos[1]!=NULL){
    if(creat(trozos[1], 0777) == -1){
      perror("creat");
      return false;
    }
  }
  else{
    if(mkdir(trozos[0], 0777) == -1){
      perror("mkdir");
      return false;
    }
  }
  return true;
}

bool fStat(char** trozos){
  bool Long = false;
  bool link = false;
  bool acc = false;
  int i;

  for(i = 0; (trozos[i]!=NULL && trozos[i][0] == '-'); i++){
      if(!strcmp(trozos[i], "-long"))
          Long = true;
      else if(!strcmp(trozos[i], "-link"))
          link = true;
      else if(!strcmp(trozos[i], "-acc"))
          acc = true;
  }
  
  if(trozos[i] == NULL){
    printRuta();  
  }   
  else{
    for(;trozos[i] != NULL; i++)
      printFStat(trozos[i], Long, acc, link);
  } 
  return true;
}

bool fBorrar(char** trozos){
  int i=0; 
  bool borro = false;

  while(trozos[i]!=NULL){
    if(remove(trozos[i])) 
      perror(trozos[i]);
    else
      borro = true;
    i++;  
  }
  return borro;
}

bool fList(char** trozos){
  bool Long = false, link = false, acc = false, hid = false, reca = false, recb = false;
  int i;

  for(i = 0; (trozos[i] != NULL && trozos[i][0] == '-'); i++){
        if(!strcmp(trozos[i], "-long"))
            Long = true;
        else if(!strcmp(trozos[i], "-link"))
            link = true;
        else if(!strcmp(trozos[i], "-acc"))
            acc = true;
        else if(!strcmp(trozos[i], "-hid"))
            hid = true;
        else if(!strcmp(trozos[i], "-reca"))
            reca = true;
        else if(!strcmp(trozos[i], "-recb"))
            recb = true;
  }

  if(trozos[i] == NULL){
    printRuta();
  }else
    for(;trozos[i] != NULL; i++){       
      if(reca){ 
        listDirRec(trozos[i], Long, acc, link, hid, reca, recb);
      }
      else if(recb){
        listDirRec(trozos[i], Long, acc, link, hid, reca, recb);
      }
      else{
        listDir(trozos[i], Long, acc, link, hid);
      }
    }
  return true;
  
}

bool fDelTree(char** trozos){
  if(trozos[0] == NULL){
     printRuta();
  }else
    for(int i = 0; trozos[i] != NULL; i++){
      delTreeRec(trozos[i]);
    }
  return true;
}

void delTreeRec(char* path){
  DIR* dir;
  char* copy = NULL;
  char* saltoAtras;
  struct dirent* file;

  if(!strcmp(path, ".")){
    saltoAtras = malloc(512 * sizeof(char));
    if(getcwd(saltoAtras, sizeof(char)*512) == NULL){
      perror("getcwd");
    }else{
      chdir("..");
      delTreeRec(saltoAtras);
    }  
    free(saltoAtras);
  }
  else if(!strcmp(path, "..")){
    chdir("..");
    saltoAtras = malloc(512 * sizeof(char));
    if(getcwd(saltoAtras, sizeof(char)*512) == NULL){
      perror("getcwd");
    }else{
      chdir("..");
      delTreeRec(saltoAtras);
    }  
    free(saltoAtras);
  }
  else{
    dir = opendir(path);
    
    if(dir == NULL){
      if (remove(path))
        perror("remove"); 
      return;
    }
    while((file = readdir(dir)) != NULL){
      if ((strcmp(file->d_name, "..") != 0) && (strcmp(file->d_name, ".") != 0)){
        copy = calloc(strlen(path) + strlen(file->d_name) + 2, sizeof(char));
        strcpy(copy, path);
        strcat(copy, "/");
        strcat(copy, file->d_name);
        delTreeRec(copy);
        free(copy);
      }
    }

    if(remove(path))
      perror("remove");
    closedir(dir);
  }
} 

int TrocearPath(char * cadena, char * trozos[]){
    int i=1;
    if ((trozos[0]=strtok(cadena,"/"))==NULL)
        return 0;
    while ((trozos[i]=strtok(NULL,"/"))!=NULL)
        i++;
    return i;
}

void listDir(char* path, bool Long, bool acc, bool link, bool hid){
  DIR* dir;
  char copy[LENGTH];
  dir = opendir(path);
  struct dirent* file;

  if(dir == NULL)  
    perror("opendir");
  else{  
    file = readdir(dir);
    printf("************%s\n", path);
    while(file != NULL){
      if ((file->d_name[0] == '.' && hid) || file->d_name[0] != '.'){
        strcpy(copy, path);
        strcat(copy, "/");
        strcat(copy, file->d_name);
        printFStat(copy, Long, acc, link);
      }
      file = readdir(dir);
    }
    closedir(dir);
  }
  
}

void listDirRec(char* path, bool Long, bool acc, bool link, bool hid, bool reca, bool recb){
  DIR* dir;
  char* copy;
  struct dirent* file;
  struct stat stats;

  if(reca){
    listDir(path, Long, acc, link, hid);
  }

  dir = opendir(path);
  if(dir == NULL) 
    perror("opendir");
  else{
    file = readdir(dir);
    while(file != NULL){
      
      if ((strcmp(file->d_name, "..") != 0) && (strcmp(file->d_name, ".") != 0)
           && ((file->d_name[0] == '.' && hid) || file->d_name[0] != '.')){
        copy = calloc(strlen(path) + strlen(file->d_name) + 2, sizeof(char));
        strcpy(copy, path);
        strcat(copy, "/");
        strcat(copy, file->d_name);
        lstat(copy, &stats);
        if(LetraTF(stats.st_mode) == 'd'){
          listDirRec(copy, Long, acc, link, hid, reca , recb);
        }
        free(copy);
      }

      file = readdir(dir);
    }
    if(recb){
      listDir(path, Long, acc, link, hid);
    }
    closedir(dir);
  }
}

void printFStat(char * trozos, bool Long, bool acc, bool link){
  char* buffer; //LIBERAR
  ssize_t lenStrLink;
  struct stat stats;
  struct passwd* pw;
  struct group* gr;
  struct tm* dateTime;
  char* permisos;
  char* strTime;
  char copyTrozos[LENGTH];
  char** tokens;
  int numTokens;

  tokens = (char**)malloc(128 * sizeof(char*));
  permisos = malloc(12 * sizeof(char));
  strTime = malloc(32 * sizeof(char));
  buffer = malloc(32 * sizeof(char));
  
  strcpy(copyTrozos, trozos);
  numTokens = TrocearPath(copyTrozos, tokens);

  if(lstat(trozos, &stats) == 0){
    if (!Long){
      printf("%8ld  %s\n", stats.st_size, tokens[numTokens-1]);
    }
    else{
    gr = getgrgid(stats.st_gid);
    pw = getpwuid(stats.st_uid);
    ConvierteModo(stats.st_mode,permisos);

    if(acc) dateTime = gmtime(&stats.st_atime);
    else dateTime = gmtime(&stats.st_mtime);
    strftime(strTime, 32, "%Y\\%m\\%d-%H:%M", dateTime);

    printf("%16s  %ld (  %6ld)    %s  %s  %s  %8ld  %s",
            strTime,stats.st_nlink, stats.st_ino, pw->pw_name,
            gr->gr_name, permisos, stats.st_size, tokens[numTokens-1]);
    
    if(link && permisos[0] == 'l'){
    lenStrLink = readlink(trozos, buffer, sizeof(buffer));

    if(lenStrLink == -1) perror("readlink");
    else buffer[lenStrLink] = '\0';
    printf("-> %s", buffer);
    }
    puts("");   
    }     
  }
  else perror(trozos);


free(permisos); free(strTime); free(buffer); free(tokens);
}  

void printRuta(){
  char* ruta = malloc(512 * sizeof(char));
  if(getcwd(ruta, sizeof(char)*512) == NULL)
    perror("getcwd");
  else
    printf("%s\n", ruta);
  free(ruta);
}

char LetraTF (mode_t m){
     switch (m&S_IFMT) { /*and bit a bit con los bits de formato,0170000 */
        case S_IFSOCK: return 's'; /*socket */
        case S_IFLNK: return 'l'; /*symbolic link*/
        case S_IFREG: return '-'; /* fichero normal*/
        case S_IFBLK: return 'b'; /*block device*/
        case S_IFDIR: return 'd'; /*directorio */ 
        case S_IFCHR: return 'c'; /*char device*/
        case S_IFIFO: return 'p'; /*pipe*/
        default: return '?'; /*desconocido, no deberia aparecer*/
     }
}

char * ConvierteModo (mode_t m, char *permisos){
    strcpy (permisos,"---------- ");
    
    permisos[0]=LetraTF(m);
    if (m&S_IRUSR) permisos[1]='r';    /*propietario*/
    if (m&S_IWUSR) permisos[2]='w';
    if (m&S_IXUSR) permisos[3]='x';
    if (m&S_IRGRP) permisos[4]='r';    /*grupo*/
    if (m&S_IWGRP) permisos[5]='w';
    if (m&S_IXGRP) permisos[6]='x';
    if (m&S_IROTH) permisos[7]='r';    /*resto*/
    if (m&S_IWOTH) permisos[8]='w';
    if (m&S_IXOTH) permisos[9]='x';
    if (m&S_ISUID) permisos[3]='s';    /*setuid, setgid y stickybit*/
    if (m&S_ISGID) permisos[6]='s';
    if (m&S_ISVTX) permisos[9]='t';
    
    return permisos;
}

