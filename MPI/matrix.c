#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>

#define NRA 10                 // numero de filas en la matriz A 
#define NCA 10                 // numero de columnas en la matriz A 
#define NCB 7                  // numero de columnas en la matriz B
#define MASTER 0               // Id de la primera tarea 
#define FROM_MASTER 1          // tipo de mensaje
#define FROM_WORKER 2          
int main (int argc, char *argv[])
{
int	numtasks,              // numero de tareas en la particion
	taskid,                // identificador de tareas 
	numworkers,            //numero de tareas esclavo
	source,                //origen
	dest,                  //destino
	mtype,                 //tipo
	rows,                  // filas de la matriz A enviadas a cada tarea 
	averow, extra, offset, 
	i, j, k, rc;           // misc 
double	a[NRA][NCA],           // matriz A  
	b[NCA][NCB],           // matriz B 
	c[NRA][NCB];           // resultado matriz C
MPI_Status status;

MPI_Init(&argc,&argv);
MPI_Comm_rank(MPI_COMM_WORLD,&taskid);
MPI_Comm_size(MPI_COMM_WORLD,&numtasks);
if (numtasks < 2 ) {
  printf("Se necesita al mneos dos tareas MPI. Cerrando...\n");
  MPI_Abort(MPI_COMM_WORLD, rc);
  exit(1);
  }
numworkers = numtasks-1;
///MAESTRO
   if (taskid == MASTER)
   {
      printf("Se ha comenzado con %d tareas.\n",numtasks);
      printf("Inicializando matrices...\n");
      for (i=0; i<NRA; i++)
         for (j=0; j<NCA; j++)
            a[i][j]= i+j;
      for (i=0; i<NCA; i++)
         for (j=0; j<NCB; j++)
            b[i][j]= i*j;

      //Enviar la data de las matrices a las tareas esclavas
      averow = NRA/numworkers; //de lo que se hace cargo cada tarea
      extra = NRA%numworkers;  //excedente
      offset = 0;
      mtype = FROM_MASTER;
      for (dest=1; dest<=numworkers; dest++)
      {
         rows = (dest <= extra) ? averow+1 : averow;  //if else 	
         printf("Enviando %d filas a la tarea %d offset=%d\n",rows,dest,offset);
         MPI_Send(&offset, 1, MPI_INT, dest, mtype, MPI_COMM_WORLD);
         MPI_Send(&rows, 1, MPI_INT, dest, mtype, MPI_COMM_WORLD);
         MPI_Send(&a[offset][0], rows*NCA, MPI_DOUBLE, dest, mtype,
                   MPI_COMM_WORLD);
         MPI_Send(&b, NCA*NCB, MPI_DOUBLE, dest, mtype, MPI_COMM_WORLD);
         offset = offset + rows;
      }

      // Recibir resultados de las tareas esclavas
      mtype = FROM_WORKER;
      for (i=1; i<=numworkers; i++)
      {
         source = i;
         MPI_Recv(&offset, 1, MPI_INT, source, mtype, MPI_COMM_WORLD, &status);
         MPI_Recv(&rows, 1, MPI_INT, source, mtype, MPI_COMM_WORLD, &status);
         MPI_Recv(&c[offset][0], rows*NCB, MPI_DOUBLE, source, mtype, 
                  MPI_COMM_WORLD, &status);
         printf("Resultados recibidos de la tarea %d\n",source);
      }

      /* Imprimir */
      printf("___________________________________________________________________\n");
      printf("Resultado de la matriz:\n");
      for (i=0; i<NRA; i++)
      {
         printf("\n"); 
         for (j=0; j<NCB; j++) 
            printf("%6.2f   ", c[i][j]);
      }
      printf("\n___________________________________________________________________\n");
      printf ("Echo.\n");
   }


///ESCLAVO
   if (taskid > MASTER)
   {  //Lo que recibe
      mtype = FROM_MASTER; 
      MPI_Recv(&offset, 1, MPI_INT, MASTER, mtype, MPI_COMM_WORLD, &status);
      MPI_Recv(&rows, 1, MPI_INT, MASTER, mtype, MPI_COMM_WORLD, &status);
      MPI_Recv(&a, rows*NCA, MPI_DOUBLE, MASTER, mtype, MPI_COMM_WORLD, &status);
      MPI_Recv(&b, NCA*NCB, MPI_DOUBLE, MASTER, mtype, MPI_COMM_WORLD, &status);
      //Lo opera
      for (k=0; k<NCB; k++)
         for (i=0; i<rows; i++)
         {
            c[i][k] = 0.0;
            for (j=0; j<NCA; j++)
               c[i][k] = c[i][k] + a[i][j] * b[j][k];
         }
      //Lo manda
      mtype = FROM_WORKER;
      MPI_Send(&offset, 1, MPI_INT, MASTER, mtype, MPI_COMM_WORLD);
      MPI_Send(&rows, 1, MPI_INT, MASTER, mtype, MPI_COMM_WORLD);
      MPI_Send(&c, rows*NCB, MPI_DOUBLE, MASTER, mtype, MPI_COMM_WORLD);
   }
   MPI_Finalize();
}
