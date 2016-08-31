#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <assert.h>

void my_bcast(void* data, int count, MPI_Datatype datatype, int root,
              MPI_Comm communicator) {
  int world_rank;	// identificador de tareas
  MPI_Comm_rank(communicator, &world_rank);
  int world_size;	// numero de tareas en la particion
  MPI_Comm_size(communicator, &world_size);

  if (world_rank == root) {
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
  /*if (argc != 3) {
    fprintf(stderr, "Usage: compare_bcast num_elements num_trials\n");
    exit(1);
  }*/

  int num_elements = 10;
  int num_trials = 4;

  MPI_Init(NULL, NULL);

  int world_rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

  double total_my_bcast_time = 0.0;
  double total_mpi_bcast_time = 0.0;
  int i;
  int* data = (int*)malloc(sizeof(int) * num_elements);
  assert(data != NULL);

  for (i = 0; i < num_trials; i++) {
    // Tiempo de my_bcast

    // Sincroniza antes de comenzar a contar el tiempo
    MPI_Barrier(MPI_COMM_WORLD);			 //Bloquea al proceso hasta que todos los procesos pertenecientes al comunicador especificado lo ejecuten.

    total_my_bcast_time -= MPI_Wtime(); 		//Devuelve el tiempo trascurrido hasta el momento de la llamda.
    my_bcast(data, num_elements, MPI_INT, 0, MPI_COMM_WORLD);

    // Synchronize again before obtaining final time
    MPI_Barrier(MPI_COMM_WORLD);
    total_my_bcast_time += MPI_Wtime();

    // Tiempo de MPI_Bcast
    MPI_Barrier(MPI_COMM_WORLD);
    total_mpi_bcast_time -= MPI_Wtime();

    MPI_Bcast(data, num_elements, MPI_INT, 0, MPI_COMM_WORLD);

    MPI_Barrier(MPI_COMM_WORLD);
    total_mpi_bcast_time += MPI_Wtime();
  }

  // Imprimir la informacion de tiempos
  if (world_rank == 0) {
    printf("Elementos= %d, Tamaño de datos = %d, Trials = %d\n", num_elements ,num_elements * (int)sizeof(int),
           num_trials);
    printf("my_bcast tiempo promedio = %lf\n", total_my_bcast_time / num_trials);
    printf("MPI_Bcast tiempo promedio= %lf\n", total_mpi_bcast_time / num_trials);
  }

  MPI_Finalize();
}
