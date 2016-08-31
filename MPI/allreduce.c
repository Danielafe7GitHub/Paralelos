#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <math.h>
#include <assert.h>

// Crear un array de numeros random. Cada numero tiene el valor de 0 - 1
float *create_rand_nums(int num_elements) {
  float *rand_nums = (float *)malloc(sizeof(float) * num_elements);
  assert(rand_nums != NULL);
  int i;
  for (i = 0; i < num_elements; i++) {
    rand_nums[i] = (rand() / (float)RAND_MAX);
  }
  return rand_nums;
}

int main(int argc, char** argv) {

  int num_elements_per_proc = 10;

  MPI_Init(NULL, NULL);

  int world_rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
  int world_size;
  MPI_Comm_size(MPI_COMM_WORLD, &world_size);
 
  //Crear un array random de elementos en todos los procesos.
  srand(time(NULL)*world_rank); // Semilla
  float *rand_nums = NULL;
  rand_nums = create_rand_nums(num_elements_per_proc);

  // Sumar los numeros localmente 
  float local_sum = 0;
  int i;
  for (i = 0; i < num_elements_per_proc; i++) {
    local_sum += rand_nums[i];
  }

  // Reducir todas las sumas locales en una suma global para calcular la media
  float global_sum;
  MPI_Allreduce(&local_sum, &global_sum, 1, MPI_FLOAT, MPI_SUM,
                MPI_COMM_WORLD);
  float mean = global_sum / (num_elements_per_proc * world_size);

  
  float local_sq_diff = 0;
  for (i = 0; i < num_elements_per_proc; i++) {
    local_sq_diff += (rand_nums[i] - mean) * (rand_nums[i] - mean);
  }

  // Reducir la suma global de las diferencias cuadradas al proceso raiz e imprimir 
  float global_sq_diff;
  MPI_Reduce(&local_sq_diff, &global_sq_diff, 1, MPI_FLOAT, MPI_SUM, 0,
             MPI_COMM_WORLD);

  // La desviacion estandas es la raiz .
  if (world_rank == 0) {
    float stddev = sqrt(global_sq_diff /
                        (num_elements_per_proc * world_size));
    printf("Mean - %f, Standard deviation = %f\n", mean, stddev);
  }

  // Limpiar
  free(rand_nums);

  MPI_Barrier(MPI_COMM_WORLD);
  MPI_Finalize();
}
