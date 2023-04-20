

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>

void inicializaCadena(char *cadena, int n){
  int i;
  for(i=0; i<n/2; i++){
    cadena[i] = 'A';
  }
  for(i=n/2; i<3*n/4; i++){
    cadena[i] = 'C';
  }
  for(i=3*n/4; i<9*n/10; i++){
    cadena[i] = 'G';
  }
  for(i=9*n/10; i<n; i++){
    cadena[i] = 'T';
  }
}

int main(int argc, char *argv[])
{
  if(argc != 3){
    printf("Numero incorrecto de parametros\nLa sintaxis debe ser: program n L\n  program es el nombre del ejecutable\n  n es el tamaño de la cadena a generar\n  L es la letra de la que se quiere contar apariciones (A, C, G o T)\n");
    exit(1); 
  }


  int numprocs, rank;

  MPI_Init(&argc, &argv); //Inicializamos el entorno MPI

  MPI_Comm_size(MPI_COMM_WORLD, &numprocs); //Recogemos el numero de procesos total
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);     //Recogemos el numero de proceso concreto

    
  int i, n, count=0;
  char *cadena;
  char L;

  //Esto lo hace únicamente el proceso 0
  if(rank == 0){
    n = atoi(argv[1]);
    L = *argv[2];
  }
  //
  /*
  Exceptuando el proceso 0, todos los demás no conocen el valor de
  n y L, por lo que hay que pasarselos desde el proceso 0 mediante un SEND()
  y el resto deben esperar el envío mediante un RECEIVE()
  */
  if (rank == 0){
    for(int proceso_destino = 1; proceso_destino<numprocs; proceso_destino++){
      MPI_Send(&n, 1, MPI_INT, proceso_destino, 0, MPI_COMM_WORLD);
      MPI_Send(&L, 1, MPI_CHAR, proceso_destino, 0, MPI_COMM_WORLD);
    }
  }
  else{
    MPI_Recv(&n, 1, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    MPI_Recv(&L, 1, MPI_CHAR, 0, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
  }
  
  //Esto lo deben hacer todos los procesos
  cadena = (char *) malloc(n*sizeof(char));
  inicializaCadena(cadena, n);
  //
  
  //Fase de computación, también la hacen todos los procesos
  //Pero cada proceso debe acceder a unas posiciones determinadas del array
  //Para esto usamos rank y numprocs en la lógica del bucle
  for(i=rank; i<n; i = i+numprocs){
    if(cadena[i] == L){
      count++;
    }
  }
  //

  //Una vez terminada la fase de computación hay que agregar los
  //subresultados de cada proceso en uno solo.
  //Para ello utilizamos MPI_REDUCE, teniendo como root el proceso 0 
  //y la operación a realizar sería una suma de todos los resultados count.
  int total_count;
  MPI_Reduce(&count, &total_count, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);


  //Ya no es necesaria más colaboración entre procesos así que llamamos a Finalize()

  MPI_Finalize();

  if(rank == 0)
    printf("El numero de apariciones de la letra %c es %d\n", &L, total_count);
  
  
  //Ahora imprimimos el resultado total guardado en total_count
  printf("El numero de apariciones de la letra %c es %d\n", &L, total_count);
  free(cadena);
  exit(0);
}