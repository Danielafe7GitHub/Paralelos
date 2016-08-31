#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv) {
  const int PING_PONG_LIMIT = 10;

  MPI_Init(NULL, NULL);

  int world_rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);//identificador de procesos
  int world_size;
  MPI_Comm_size(MPI_COMM_WORLD, &world_size);//procesos 

  // Al menos 2 procesos para esta tarea
  if (world_size != 2) {
    fprintf(stderr, "World size debe ser 2 para %s\n", argv[0]);
    MPI_Abort(MPI_COMM_WORLD, 1);
  }

  int ping_pong_count = 0;
  int partner_rank = (world_rank + 1) % 2;
  while (ping_pong_count < PING_PONG_LIMIT) {
    if (world_rank == ping_pong_count % 2) {
      
      // Incrementar el contador antes de enviar 
      ping_pong_count++;
      MPI_Send(&ping_pong_count, 1, MPI_INT, partner_rank, 0, MPI_COMM_WORLD);
      printf("ENVIAR	Rank actual:%d 	contador de ping pong:%d 	destino:%d\n",
             world_rank, ping_pong_count, partner_rank);
    } else {
      MPI_Recv(&ping_pong_count, 1, MPI_INT, partner_rank, 0, MPI_COMM_WORLD,
               MPI_STATUS_IGNORE);
      printf("RECIVIR	Rank acual:%d 	 contador de ping pong:%d 	origen :%d\n",
             world_rank, ping_pong_count, partner_rank);
    }
  }
  MPI_Finalize();
}
