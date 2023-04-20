/*--------------------------------PRÁCTICA_1-----------------------------------

Santiago Alfredo Castro Rampesad (s.a.castro.rampersad) (54940335M)
Xian Priego Martín (xian.priego) (39457432N)

GRUPO 4.3
-----------------------------------------------------------------------------*/


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

