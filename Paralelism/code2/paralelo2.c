//Xian Priego Martin

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

int MPI_BinomialBcast(void* buffer, int count, MPI_Datatype datatype, int root, MPI_Comm comm){
  int numprocs, myrank, destino, origen;
  MPI_Comm_size(MPI_COMM_WORLD, &numprocs); //Recogemos el numero de procesos total
  MPI_Comm_rank(MPI_COMM_WORLD, &myrank); 

  int power = 1;
  while(power < numprocs){
    if (myrank < power){ //Debe enviar
      destino = myrank + power;
      if(destino < numprocs) //Verificamos que el destino no es un proceso que no existe
        MPI_Send(buffer, count, datatype, destino, 0, comm);
    }

    else if(myrank < power*2){ //Debe recibir el valor
      origen = myrank - power;
      MPI_Recv(buffer, count, datatype, origen, MPI_ANY_TAG, comm, MPI_STATUS_IGNORE);
    }
    power = power * 2;
  }

  return 0;

}

int MPI_FlatTree (const void* sendbuf, void* recvbuf, int count, MPI_Datatype datatype, MPI_Op op, int root, MPI_Comm comm){
  int numprocs, myrank;
  MPI_Comm_size(MPI_COMM_WORLD, &numprocs); //Recogemos el numero de procesos total
  MPI_Comm_rank(MPI_COMM_WORLD, &myrank); 

  int* rbuf = (int*) recvbuf;
  int* sbuf = (int*) sendbuf;

  if (myrank == root) {
    // Recibimos los resultados parciales de cada proceso
    for (int i = 1; i < numprocs; i++) {
      int count_i;
      MPI_Recv(&count_i, 1, datatype, i, MPI_ANY_TAG, comm, MPI_STATUS_IGNORE);
      *rbuf += count_i;
    }
    // Sumamos nuestro propio resultado parcial
    *rbuf += *sbuf;
  } else {
    // Enviamos nuestro resultado parcial al proceso root
    MPI_Send(sendbuf, 1, datatype, root, 0, comm);
  }

  return 0;
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

  //DISTRIBUCIÓN DE n Y L AL RESTO DE PROCESOS MEDIANTE UNA OPERACIÓN COLECTIVA
  //LA OP. COLECTIVA SE LLAMARÁ MPI_BINOMIAL_BCAST(), IMPLEMENTARÁ UN BROADCAST BINOMIAL
  //DE n y L
  MPI_BinomialBcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
  MPI_BinomialBcast(&L, 1, MPI_CHAR, 0, MPI_COMM_WORLD);
  //MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
  //MPI_Bcast(&L, 1, MPI_CHAR, 0, MPI_COMM_WORLD);
  

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
  //Para ello utilizamos MPI_FlatTree, teniendo como root el proceso 0 
  //y la operación a realizar sería una suma de todos los resultados count.
  int total_count = 0; 
  MPI_FlatTree(&count, &total_count, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
  //MPI_Reduce(&count, &total_count, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

  //Ya no es necesaria más colaboración entre procesos así que llamamos a Finalize()

  

  //Ahora imprimimos el resultado total guardado en total_count, pero unicamente lo hace el proceso root
  
  
  if(rank == 0)
    printf("El numero de apariciones de la letra %c es %d\n", L, total_count);
  
  
  
  free(cadena);
  MPI_Finalize();
  exit(0);
}
