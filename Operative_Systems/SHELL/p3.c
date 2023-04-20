
 
#include "p0Code.h"
char** arg3;
void* arg3p;

int main(int argc, char *argv[], char *envp[]){
    char* cadena;
    cadena = (char*)malloc(N); 
    bool seguir = true;
    arg3 = envp;
    arg3p = &envp;
    iniciarLista();
    initializeMemList();
    initializeProList();

    while(seguir){
        imprimirPrompt();
        leerEntrada(cadena);
        seguir = procesarEntrada(cadena);
    }
    free(cadena);
    return 0;
}
 
 