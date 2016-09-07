
///mpic++ OddEven.c -o odd
///mpirun  ./odd

#include <mpi.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
int mostrarM(double *matriz, int fila, int columna)
{
    for (int x = 0; x < fila; x++) 
    {
	    for (int y = 0; y < columna; y++) 
	    {
		printf("%7g ",matriz[x*columna+y]);
	    }
    printf("\n");
    }
    return 0;
}
void OddEven_Sort(double *vector, int tam)
{
  int acabo = 0, i;
  double tmp;
  while (!acabo)
  {
    acabo = 1;
    for (i = 1; i < tam-2; i+=2)
    {
      if (vector[i] > vector[i+1])
      {
        tmp = vector[i];
        vector[i] = vector[i+1];
        vector[i+1] = tmp;
        acabo = 0;
      }
    }
    for (i = 0; i < tam-1; i+=2)
    {
      if (vector[i] > vector[i+1])
      {
        tmp = vector[i];
        vector[i] = vector[i+1];
        vector[i+1] = tmp;
        acabo = 0;
      }
    }
  }
}
void llenarM(double *matrix,int fila, int columna)
{
    for (int i=0;i<(fila*columna);i++) 
    {
    *(matrix + i)=1 + (double)(1000*random()/(RAND_MAX+1.0));
    }

}
void Menores(double* My_Vector, double* temp1, double* temp2,int tamB) 
{
   
   for(int i=0,j=0,k=0;i<tamB;i++) 
   {
      if (My_Vector[j] <= temp1[k]) 
      {
        temp2[i] = My_Vector[j];
        j++;
      } 
      else 
      {
        temp2[i] = temp1[k];
        k++;
      }
   }
   memcpy(My_Vector, temp2, tamB*sizeof(double));
}
void Mayores(double* My_Vector,double* temp1,double* temp2,int tamB) 
{
   for(int i=tamB-1,j=tamB-1,k=tamB-1;i >= 0;i--) 
   {
      if (My_Vector[j] >= temp1[k]) 
      {
        temp2[i] = My_Vector[j];
        j--;
      } 
      else 
      {
        temp2[i] = temp1[k];
        k--;
      }
   }
   memcpy(My_Vector,temp2,tamB*sizeof(double));
}
int main(int argc, char **argv)
{
  double start_time, end_time;
  int my_rank, comm_sz;
  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
  double* My_Vector;
  double* Ordenado;
  int tamB=6;
  My_Vector=(double*)malloc(tamB*sizeof(double));
  if(my_rank==0)
    Ordenado=(double*)malloc(tamB*comm_sz*sizeof(double));
  llenarM(My_Vector,1,tamB);
  printf("Desordenado\n");
  mostrarM(My_Vector,1,tamB);
  printf("\n");
  OddEven_Sort(My_Vector,tamB);
  double *temp1, *temp2;
  int Com_Par;  
  int Com_Impar;
  temp1 =(double*)malloc(tamB*sizeof(double));
  temp2 =(double*)malloc(tamB*sizeof(double));
  if (my_rank % 2 != 0) 
  {
    Com_Par=my_rank - 1;
    Com_Impar=my_rank + 1;
    if (Com_Impar==comm_sz) 
      Com_Impar= -1;
  } 
  else 
  {
    Com_Par=my_rank + 1;
    if (Com_Par==comm_sz) 
      Com_Par= -1;
    Com_Impar=my_rank - 1;  
  }
  for (int fase = 0; fase < comm_sz; fase++)
  {
    if (fase % 2 == 0) 
    {
      if (Com_Par >= 0) 
      {
         MPI_Sendrecv(My_Vector, tamB, MPI_DOUBLE, Com_Par, 0, temp1, tamB, MPI_DOUBLE, Com_Par, 0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
         if (my_rank % 2 != 0)
            Mayores(My_Vector, temp1, temp2, tamB);
         else
            Menores(My_Vector, temp1, temp2, tamB);
      }
    } 
    else 
    {
      if (Com_Impar >= 0) 
      {
         MPI_Sendrecv(My_Vector, tamB, MPI_DOUBLE, Com_Impar, 0,temp1, tamB, MPI_DOUBLE, Com_Impar, 0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
         if (my_rank % 2 != 0)
            Menores(My_Vector, temp1, temp2, tamB);
         else
            Mayores(My_Vector, temp1, temp2, tamB);
      }
    }
  }
  free(temp1);
  free(temp2);
  MPI_Gather(My_Vector,tamB,MPI_DOUBLE,Ordenado,tamB,MPI_DOUBLE,0,MPI_COMM_WORLD);
  
 if(my_rank==0)
    {
    printf("Ordenado\n");
    mostrarM(Ordenado,1,tamB*comm_sz);
    }
  MPI_Finalize();  
	return 0;
}
