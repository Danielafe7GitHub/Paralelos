#include <stdio.h>
#include <device_launch_parameters.h>
#include <cuda_runtime.h>
#include <cuda.h>
#include <Windows.h>
#include <iostream>
using namespace std;

//B. Por cada elemento
__global__
void matrixAddKernel(float* A, float* B, float* C, int n)
{
	int i = threadIdx.x + blockDim.x * blockIdx.x;
	if (i < n*n) C[i] = A[i] + B[i];
}

//C. Por cada fila 
__global__
void matrixAddKernel2(float* A, float* B, float* C, int n)
{
	int i = threadIdx.x + blockDim.x * blockIdx.x;
	if (i < n)
	{
		for (int j = i * n; j < i * n + n; j++)
			C[j] = A[j] + B[j];
	}
}

//D. Por cada columna
__global__
void matrixAddKernel3(float* A, float* B, float* C, int n)
{
	int i = threadIdx.x + blockDim.x * blockIdx.x;
	if (i<n)
	for (int j = i; j < n*n; j += n)
		C[j] = A[j] + B[j];
}

void matrixAdd(float* A, float* B, float* C, int n) {
	int size = n * n * sizeof(float);
	float *d_A, *d_B, *d_C;

	///A. separar memoria, copiar Input data a Device
	cudaMalloc((void **)&d_A, size);
	cudaMemcpy(d_A, A, size, cudaMemcpyHostToDevice);
	cudaMalloc((void **)&d_B, size);
	cudaMemcpy(d_B, B, size, cudaMemcpyHostToDevice);
	cudaMalloc((void **)&d_C, size);

	///Llamada kernel
	dim3 DimGrid ((n-1)/256+1,1,1);
	dim3 DimBlock (256,1,1);
	matrixAddKernel1 <<<DimGrid,DimBlock  >>> (d_A, d_B, d_C, n);
	/// alternativo ceil((n*n) / 256.0), 256
	
	///tranferir output data a Host
	cudaMemcpy(C, d_C, size, cudaMemcpyDeviceToHost);

	///Liberar Memoria
	cudaFree(d_A); cudaFree(d_B); cudaFree(d_C);
}
void mostrarM(float* matriz, float fila, float columna)
{
	for (int x = 0; x < fila; x++)
	{
		for (int y = 0; y < columna; y++)
		{
			int puesto = x*columna + y;
			printf("%3.0f ", matriz[puesto]);
		}
		printf("\n");
	}
}
int main() {
	int fila;
	cout<<"ingrese dimensiones: ";
	cin>>fila;
	printf("	=  \n"); 

	float* A = (float*)malloc(fila*fila*sizeof(float));
	float* B = (float*)malloc(fila*fila*sizeof(float));
	float* C = (float*)malloc(fila*fila*sizeof(float));
	for (int i = 0; i < fila*fila; i++)
	{
		A[i] = i;
		B[i] = i*2;
	}
	
	///mostrar
	mostrarM(A, fila, fila);
	printf("	+ \n");
	mostrarM(B, fila, fila);
	printf("	=  \n");

	///operaciones
	matrixAdd(A, B, C, fila);
	mostrarM(C, fila, fila);
	system("PAUSE");
	exit(0);
}
