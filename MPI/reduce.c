#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
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
void my_reduce(void* data, int count, MPI_Datatype datatype, int root,
              MPI_Comm communicator) {
  int world_rank;	// identificador de tareas
  MPI_Comm_rank(communicator, &world_rank);
  int world_size;	// numero de tareas en la particion
  MPI_Comm_size(communicator, &world_size);

  if (world_rank != root) {
    // PROPAGAR Si somos el proceso raiz, enviar nuestra data a todos
    int i;
    for (i = 0; i < world_size; i++) {
      if (i != world_rank) {
        MPI_Send(data, count, datatype, i, 0, communicator);
      }
    }
  } else {
    
    // Si somos el proceso que recepciona, recibir la data de la raiz
    MPI_Recv(data, count, datatype, root, 0, communicator, MPI_STATUS_IGNORE);
  }
}

int main(int argc, char** argv) {
 /*if (argc != 2) {
    fprintf(stderr, "Usage: avg num_elements_per_proc\n");
    exit(1);
  }*/

  int num_elements_per_proc = 10;
  MPI_Init(NULL, NULL);

  int world_rank;		 // identificador de tareas
  MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
  int world_size;        	 // numero de tareas en la particion
  MPI_Comm_size(MPI_COMM_WORLD, &world_size);

  //Crear un array random de elementos en todos los procesos.
  srand(time(NULL)*world_rank);  // Semilla para random
  float *rand_nums = NULL;

  rand_nums = create_rand_nums(num_elements_per_proc);

  // Suma los numeros localmente
  float local_sum = 0;
  int i;
  for (i = 0; i < num_elements_per_proc; i++) {
    local_sum += rand_nums[i];
  }

  printf("Suma local para el proceso %d es  %f, promedio = %f\n",
         world_rank, local_sum, local_sum / num_elements_per_proc);

  // Reduce todas las sumas locales en una suma global
  float global_sum;
  MPI_Reduce(&local_sum, &global_sum, 1, MPI_FLOAT, MPI_SUM, 0,
             MPI_COMM_WORLD);

  // Imprime el resultado
  if (world_rank == 0) {
    printf("Suma total = %f, promedio = %f\n", global_sum,
           global_sum / (world_size * num_elements_per_proc));
  }

  // Limpiar
  free(rand_nums);

  MPI_Barrier(MPI_COMM_WORLD);	 //Bloquea al proceso hasta que todos los procesos pertenecientes al comunicador especificado lo ejecuten.
  MPI_Finalize();
}
