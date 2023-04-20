


typedef struct {
  char* nombre;
  bool (*funcion_comando)(char**);
} tCmd;

typedef struct{
  char* nombre;
  char* login;
} tAutor;

typedef struct{
    char* cmd;
    char* ops;
    char* help;
}ayudaCmd;

