#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
void llenarM(double *matrix,int fila, int columna)
{
    for (int i=0;i<(fila*columna);i++) 
    {
		*(matrix + i)=1 + (double)(10*random()/(RAND_MAX+1.0));
    }

}
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

void Mat_vect_mult(double local_A[],double local_x[], double local_y[], int local_m,int n,int local_n)
{
    double * x;
    int local_i,j;
    int local_ok=1;
    x=(double*)malloc(n*sizeof(double));
    MPI_Allgather(local_x,local_n,MPI_DOUBLE,x,local_n,MPI_DOUBLE,MPI_COMM_WORLD);
    for(local_i=0;local_i<local_m;local_i++)
    {
    	local_y[local_i]=0.0;
    	for(j=0;j<n;j++)
    	{
    		local_y[local_i]+=local_A[local_i*n+j]*x[j];
    	}
    }
    free(x);
}

int main(int argc, char **argv)
{
	double start_time, end_time,runtime,serial,end_time_secuencial, start_time_secuencial;
        double speedup,effi;
    int my_rank, comm_sz;
    double * Matriz;
    double * Vector;
    double * Resultado;
    double * Resultados;
    double * comprueba;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
    //comm_sz=2;
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    int filas=16384;
    int columnas=filas;
    Matriz=(double*)malloc(filas*columnas*sizeof(double));
    Vector=(double*)malloc(columnas*sizeof(double));
    Resultados=(double*)malloc(filas*sizeof(double));
    comprueba=(double*)malloc(filas*sizeof(double));
    if(my_rank==0)
    {
	    llenarM(Matriz,filas,columnas);
	    llenarM(Vector,1,columnas);
            printf("Matriz \n");
           // mostrarM(Matriz,filas,columnas);
	    printf("Vector \n");
	   // mostrarM(Vector,1,columnas);
	    MPI_Bcast(Matriz,filas*columnas,MPI_DOUBLE, 0, MPI_COMM_WORLD);
		MPI_Bcast(Vector,columnas,MPI_DOUBLE, 0, MPI_COMM_WORLD);
    }
    else
    {
    	MPI_Bcast(Matriz,filas*columnas,MPI_DOUBLE, 0, MPI_COMM_WORLD);
		MPI_Bcast(Vector,columnas,MPI_DOUBLE, 0, MPI_COMM_WORLD);
    }
    
	double* MiMat;
    double* MiVec;
    int inicioM,finM,saltoM,inicioV,finV,saltoV;
	saltoM = (int)(filas/comm_sz);
	inicioM = saltoM*(my_rank);
	finM = inicioM+saltoM-1;
	if (my_rank == comm_sz-1) 
	{
	    finM =finM+(filas%comm_sz);
	}
	saltoV = (int)(columnas/comm_sz);
	inicioV = saltoV*(my_rank);
	finV = inicioV+saltoV-1;
	if (my_rank == comm_sz-1) 
	{
	    finV =finV+(columnas%comm_sz);
	}
	MiMat=(double*)malloc((finM-inicioM+1)*columnas*sizeof(double));
	MiVec=(double*)malloc((finV-inicioV+1)*sizeof(double));
	Resultado=(double*)malloc((finM-inicioM+1)*sizeof(double));
	for (int i=inicioM,k=0; i <=finM; i++,k++)
	{
		for (int j=0; j<columnas;j++)
		{
			MiMat[k*columnas+j]=Matriz[i*columnas+j];
		}
	}
	for (int i = inicioV,j=0; i <=finV; i++,j++)
	{
		MiVec[j]=Vector[i];
	}
	free(Matriz);
	free(Vector);
	MPI_Barrier(MPI_COMM_WORLD);
	start_time = MPI_Wtime();
	Mat_vect_mult(MiMat,MiVec,Resultado,finM-inicioM+1,columnas,finV-inicioV+1);
	MPI_Allgather(Resultado,finM-inicioM+1,MPI_DOUBLE,Resultados,finM-inicioM+1,MPI_DOUBLE,MPI_COMM_WORLD);
	MPI_Barrier(MPI_COMM_WORLD); 
	end_time= MPI_Wtime();
         /////SECUEENCIAL
        start_time_secuencial = MPI_Wtime();
        Mat_vect_mult(MiMat,MiVec,comprueba,0,columnas*filas,0);
	/* for (int x = 0; x < filas; x++) 
	    {
			for (int y = 0; y < columnas; y++) 
			{
		    	comprueba[x]=MiMat[x*columnas+y]*MiVec[y];
			}
			printf("doing");
	    } 	
        */
	/*for (int i = 0; i < n; i++) {
	comprueba[i] = 0;
	for (int j = 0; j < n; j++) {
	    comprueba[i] += MiMat[i][j] * Mivect[j];
	}
   	}*/
        end_time_secuencial= MPI_Wtime(); 
        serial=end_time_secuencial - start_time_secuencial;
        runtime=end_time - start_time;
	speedup=serial/runtime;
        effi=speedup/comm_sz;
	printf("Codigo se ejecuto en %f segundos\n",runtime);
	printf("Speedup %f \n",speedup);
 	printf("Efficiency %f \n",effi);
        printf("Resultado Paralelo\n");
	//mostrarM(Resultado,1,columnas);
 	printf("Resultado Secuencial\n");
        //mostrarM(comprueba,1,columnas);
    free(Resultados);
    MPI_Finalize();
    return 0;
}
