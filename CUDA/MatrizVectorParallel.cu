##include <stdio.h>
#include <device_launch_parameters.h>
#include <cuda_runtime.h>
#include <cuda.h>
#include <Windows.h>
#include <iostream>
#define TILE_WIDTH 16
using namespace std;
__global__
void vecMultKernel(float* A, float* B, float* C, int n){
	int i = threadIdx.x + blockDim.x * blockIdx.x;
	if (i<n){
		C[i] = 0;
		for (int j = 0; j<n; j++)
			C[i] += A[j*n + i] * B[j];
	}
}
__global__ void MatrixMultKernel(float* d_M, float* d_N, float* d_P, int width){
	__shared__ float Mds[TILE_WIDTH][TILE_WIDTH];
	__shared__ float Nds[TILE_WIDTH][TILE_WIDTH];
	int bx = blockIdx.x; int by = blockIdx.y;
	int tx = threadIdx.x; int ty = threadIdx.y;
	// Identify the row and column of the d_P element to work on
	int Row = by * TILE_WIDTH + ty;
	int Col = bx * TILE_WIDTH + tx;
	float Pvalue = 0;
	// Loop over the d_M and d_N tiles required to compute d_P element
	for (int m = 0; m < Width/TILE_WIDTH; ++m) {
	// Coolaborative loading of d_M and d_N tiles into shared memory
		Mds[ty][tx] = d_M[Row*Width + m*TILE_WIDTH + tx];
		Nds[ty][tx] = d_N[(m*TILE_WIDTH + ty)*Width + Col];
		__syncthreads();
		for (int k = 0; k < TILE_WIDTH; ++k) {
			Pvalue += Mds[ty][k] * Nds[k][tx];
		}
		__syncthreads();
	}
	d_P[Row*Width + Col] = Pvalue; 
}
void matrizXvector(float* A, float* B, float* C, int n) {
	int size = n * n * sizeof(float);
	int sizevect = n * sizeof(float);
	float *d_A, *d_B, *d_C;
	///Redimensionar y copiar de Host a Device
	cudaMalloc((void **)&d_A, size);
	cudaMemcpy(d_A, A, size, cudaMemcpyHostToDevice);
	cudaMalloc((void **)&d_B, sizevect);
	cudaMemcpy(d_B, B, sizevect, cudaMemcpyHostToDevice);
	cudaMalloc((void **)&d_C, sizevect);

	//Llamada Kernel
	vecMultKernel <<< ceil((n*n) / 256.0), 256 >>> (d_A, d_B, d_C, n);
	//ceil se asegura de que tener suficientes hilos para cubrir los elementos

	//copiar de Device a Host
	cudaMemcpy(C, d_C, sizevect, cudaMemcpyDeviceToHost);

	//liberar memoria
	cudaFree(d_A); cudaFree(d_B); cudaFree(d_C);
}
void matrizXmatriz(float* A, float* B, float* C, int n) {
	int size = n * n * sizeof(float);
	int sizevect = n * n * sizeof(float);
	float *d_A, *d_B, *d_C;
	///Redimensionar y copiar de Host a Device
	cudaMalloc((void **)&d_A, size);
	cudaMemcpy(d_A, A, size, cudaMemcpyHostToDevice);
	cudaMalloc((void **)&d_B, sizevect);
	cudaMemcpy(d_B, B, sizevect, cudaMemcpyHostToDevice);
	cudaMalloc((void **)&d_C, sizevect);

	//Llamada Kernel
	MatrixMultKernel <<< ceil((n*n) / 256.0), 256 >>> (d_A, d_B, d_C, n);
	//ceil se asegura de que tener suficientes hilos para cubrir los elementos

	//copiar de Device a Host
	cudaMemcpy(C, d_C, sizevect, cudaMemcpyDeviceToHost);

	//liberar memoria
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
	int fila , columna ;
	cout<<"ingrese dimensiones"<< endl;
	cin>>fila;
	columna=fila;
	float* A = (float*)malloc(fila*columna*sizeof(float));
	float* B = (float*)malloc(fila*sizeof(float));
	float* C = (float*)malloc(fila*sizeof(float));
	for (int i = 0; i < fila*columna; i++)
	{
		A[i] = i;
	}
	for (int i = 0; i < fila*columna; i++)
	{
		B[i] = i;
	}

	cout<<" vector "<<endl;
	mostrarM(B, 1, columna);
	cout<<" * matriz "<<endl;
	mostrarM(A, fila, columna);
	
	cout <<"Resultado"<<endl;
	//matrizXvector(A, B, C, fila);
	matrizXmatriz(A, B, C, fila);
	mostrarM(C, 1, columna);

	system("PAUSE");
	exit(0);
}
