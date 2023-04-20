#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <mpi.h>
#include <math.h>

#define DEBUG 0

/* Translation of the DNA bases
   A -> 0
   C -> 1
   G -> 2
   T -> 3
   N -> 4*/

#define M  1000000 // Number of sequences
#define N  200  // Number of bases per sequence

unsigned int g_seed = 0;

int fast_rand(void) {
    g_seed = (214013*g_seed+2531011);
    return (g_seed>>16) % 5;
}

// The distance between two bases
int base_distance(int base1, int base2){

  if((base1 == 4) || (base2 == 4)){
    return 3;
  }

  if(base1 == base2) {
    return 0;
  }

  if((base1 == 0) && (base2 == 3)) {
    return 1;
  }

  if((base2 == 0) && (base1 == 3)) {
    return 1;
  }

  if((base1 == 1) && (base2 == 2)) {
    return 1;
  }

  if((base2 == 2) && (base1 == 1)) {
    return 1;
  }

  return 2;
}

int main(int argc, char *argv[] ) {

  int numprocs, myrank;

  MPI_Init(&argc, &argv); //Inicializamos el entorno MPI

  MPI_Comm_size(MPI_COMM_WORLD, &numprocs); //Recogemos el numero de procesos total
  MPI_Comm_rank(MPI_COMM_WORLD, &myrank);     //Recogemos el numero de proceso concreto

  int i, j;
  int *data1, *data2;
  int *result;
  struct timeval  tv1, tv2;

  int filas_por_proceso = (int) ceil(1.0*M/numprocs);

  if(myrank == 0){
    data1 = (int *) malloc(filas_por_proceso*numprocs*N*sizeof(int));
    data2 = (int *) malloc(filas_por_proceso*numprocs*N*sizeof(int));
    result = (int *) malloc(filas_por_proceso*numprocs*sizeof(int));
  }
  int* subdata1 = (int *) malloc(filas_por_proceso*N*sizeof(int));
  int* subdata2 = (int *) malloc(filas_por_proceso*N*sizeof(int));
  int* subresult = (int *) malloc(filas_por_proceso*sizeof(int));

  /*La inicialización de las matrices las hace únicamente el proceso raíz*/
  if(myrank == 0){
  /* Initialize Matrices */
    for(i=0;i<M;i++) {
        for(j=0;j<N;j++) {
        /* random with 20% gap proportion */
        data1[i*N+j] = fast_rand();
        data2[i*N+j] = fast_rand();
        }
    }
  }

  //Transmitimos las submatrices correspondientes a cada proceso:
  //Lo haremos mediante un MPI_Scatter()
  
  

  MPI_Scatter(data1, filas_por_proceso*N, MPI_INT, subdata1, filas_por_proceso*N, MPI_INT, 0, MPI_COMM_WORLD);
  MPI_Scatter(data2, filas_por_proceso*N, MPI_INT, subdata2, filas_por_proceso*N, MPI_INT, 0, MPI_COMM_WORLD);




  gettimeofday(&tv1, NULL);

  for(i=0;i<filas_por_proceso;i++) {
    subresult[i]=0;
    for(j=0;j<N;j++) {
      subresult[i] += base_distance(subdata1[i*N+j], subdata2[i*N+j]);
    }
  }

  gettimeofday(&tv2, NULL);
    
  int microseconds = (tv2.tv_usec - tv1.tv_usec)+ 1000000 * (tv2.tv_sec - tv1.tv_sec);


  //Tras la computación de cada subresultado, hay que agregar cada uno a un unico array resultado
  //Mediante un MPI_Gather

  MPI_Gather(subresult, filas_por_proceso, MPI_INT, result, filas_por_proceso, MPI_INT, 0, MPI_COMM_WORLD);

  /* Display result */
  if (DEBUG == 1) {
    int checksum = 0;
    for(i=0;i<M;i++) {
      checksum += result[i];
    }
    printf("Checksum: %d\n ", checksum);
  } else if (DEBUG == 2) {
    for(i=0;i<M;i++) {
      printf(" %d \t ",result[i]);
    }
  } else {
    printf ("Time (seconds) = %lf\n", (double) microseconds/1E6);
  }    

  if(myrank == 0)
    free(data1); free(data2); free(result);

  free(subdata1); free(subdata2); free(subresult);  

  return 0;
}


