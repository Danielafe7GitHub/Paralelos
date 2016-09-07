#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

void llenarM(int *matrix,int fila, int columna)
{
    for (int i=0;i<(fila*columna);i++) 
    {
		*(matrix + i)=i + (int)(10*random()/(RAND_MAX+1.0));
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
                printf("comm_sz %d \n", comm_sz);
		proceso = comm_sz-1;
		salto = (int)(columna/proceso);
		int* matrizs=NULL;
		int* vectors=NULL;
		matrizs=(int*)malloc(salto*fila*sizeof(int));
		vectors=(int*)malloc(salto*sizeof(int));
		for(int i=1;i<comm_sz;i++)
		{
			inicio = salto*(i-1);
			fin = inicio+salto-1;
			printf("inicio %d\n",inicio);
			printf("fin %d\n",fin);
			for(int r=inicio,k=0;r<=fin;r++,k++)
			{
				for(int j=0;j<fila;j++)
				{
					matrizs[k*fila+j]=matriz[j*columna+r];
				}
				vectors[k]=vector[r];
			}
			mostrarM(matrizs,(fin-inicio+1),fila);
			printf("\n");
			mostrarM(vectors,1,(fin-inicio+1));
			printf("\n");
			MPI_Send(matrizs,(fin-inicio+1)*fila,MPI_INT, i, 0, MPI_COMM_WORLD);
			MPI_Send(vectors,(fin-inicio+1),MPI_INT, i, 0, MPI_COMM_WORLD);
		}
    }
    else
    {
    	proceso = comm_sz-1;
		salto = (int)(fila/proceso);
    	inicio = salto*(my_rank-1);
		fin = inicio+salto-1;
		matriz=(int*)malloc((fin-inicio+1)*fila*sizeof(int));
		vector=(int*)malloc((fin-inicio+1)*sizeof(int));
    	MPI_Recv(matriz,(fin-inicio+1)*fila,MPI_INT,0,0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    	MPI_Recv(vector,(fin-inicio+1),MPI_INT,0,0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    	for(int i=0;i<fila;i++)
	    {
	    	l_resultado[i]=0;
	    	for(int j=0;j<=fin-inicio;j++)
	    	{
	    		l_resultado[i]+=matriz[i+j*fila]*vector[j];
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
