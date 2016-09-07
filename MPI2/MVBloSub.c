#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

void llenarM(int *matrix,int fila, int columna)
{
    for (int i=0;i<(fila*columna);i++) 
    {
		*(matrix + i)=i*4 + (int)(10*random()/(RAND_MAX+1.0));

    }

}
int mostrarM(int *matriz, int fila, int columna)
{
    for (int x = 0; x < fila; x++) 
    {
		for (int y = 0; y < columna; y++) 
		{
	    	printf("%7d ",matriz[x*columna+y]);
		}
		printf("\n");
    }
    return 0;
}
void copearMC(int* matriz1,int* matriz2,int* vector1,int* vector2,int inicio,int fin,int filas,int columna)
{
	for(int i=inicio,k=0;i<=fin;i++,k++)
	{
		for(int j=0;j<filas;j++)
		{
			matriz2[k*filas+j]=matriz1[j*columna+i];
		}
		vector2[i]=vector1[i];
	}
}
int main(int argc, char **argv)
{
    int my_rank, comm_sz, fila, columna;
    int proceso,inicio,fin,salto;
    int *matriz=NULL;
    int *vector=NULL;
    int *resultado=NULL;
    int * l_resultado=NULL;
    fila=6;
    columna=6;
    l_resultado= (int*)malloc(fila*sizeof(int));
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    if (my_rank == 0) 
    {
		matriz = (int*)malloc(fila*columna*sizeof(int));
		vector =(int*) malloc(columna*sizeof(int));
		resultado = (int*)malloc(fila*sizeof(int));
		for(int i=0;i<fila;i++)
			l_resultado[i]=0;
		llenarM(matriz, fila, columna);
		llenarM(vector, 1, columna);
		mostrarM(matriz,fila,columna);
		printf("\n");
		mostrarM(vector,1,columna);
		proceso = sqrt(comm_sz);
		printf("proceso %d\n",proceso);
		salto = (int)(columna/proceso);
		int* matrizs=NULL;
		int* vectors=NULL;
		matrizs=(int*)malloc(salto*salto*sizeof(int));
		vectors=(int*)malloc(salto*sizeof(int));
		for(int i=1,m=0;i<=proceso;i++)
		{
			inicio = salto*(i-1);
			fin = inicio+salto-1;
			printf("inicio %d\n",inicio);
			printf("fin %d\n",fin);
			for(int l=1;l<=proceso;l++)
			{
				int inicio2=salto*(l-1);
				int fin2=inicio2+salto-1;
				for(int r=inicio,k=0;r<=fin;r++,k++)
				{
					for(int j=inicio2,p=0;j<=fin2;j++,p++)
					{
						matrizs[k*salto+p]=matriz[j*columna+r];
					}
					vectors[k]=vector[r];
				}
 				printf("HERE\n");
				MPI_Send(matrizs,salto*salto,MPI_INT,m, 0, MPI_COMM_WORLD);
				MPI_Send(vectors,salto,MPI_INT,m, 0, MPI_COMM_WORLD);
				m++;
			}
		}
    }
    else
    {
    	for (int i = 0; i < fila; i++)
    	{
    		l_resultado[i]=0;
    	}
    	proceso = sqrt(comm_sz-1);
		salto = (int)(fila/proceso);
		matriz=(int*)malloc(salto*salto*sizeof(int));
		vector=(int*)malloc(salto*sizeof(int));
    	MPI_Recv(matriz,salto*salto,MPI_INT,0,0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    	MPI_Recv(vector,salto,MPI_INT,0,0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    	for(int i=((my_rank-1)%proceso)*salto,k=0;i<salto+(((my_rank-1)%proceso)*salto);i++,k++)
	    {
	    	l_resultado[i]=0;
	    	for(int j=0;j<salto;j++)
	    	{
	    		l_resultado[i]+=matriz[k+j*salto]*vector[j];
	    	}
	    }
    }
    MPI_Reduce(l_resultado, resultado, fila, MPI_INT, MPI_SUM, 0,MPI_COMM_WORLD);
    if(my_rank==0)
    {
    	printf("\n");
		mostrarM(resultado,fila,1);
    }
    
    MPI_Finalize();
    return 0;
}
