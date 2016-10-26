#include <stdio.h>
#include <device_launch_parameters.h>
#include <cuda_runtime.h>
#include <cuda.h>
#include <Windows.h>
#include <iostream>
using namespace std;
#define BLOCK 8
__global__
void matrizXescalarK(float* A, float B, float* C, int f,int c,int e){

	int fila = blockIdx.y*blockDim.y + threadIdx.y;
	int columna = blockIdx.x*blockDim.x + threadIdx.x;
	int profundidad = blockIdx.z*blockDim.z + threadIdx.z;

	if ((fila < f) && (columna < c) && (profundidad < e)) {
		C[fila*c + columna + profundidad*c*f] = B * A[fila*c + columna+profundidad*c*f];
	}
}
void matrizXescalar(float* A, float B, float* C, int f,int c,int e) {
	int size = f * c * e *  sizeof(float);
	float *d_A, *d_C;
	cudaMalloc((void **)&d_A, size);
	cudaMemcpy(d_A, A, size, cudaMemcpyHostToDevice);
	cudaMalloc((void **)&d_C, size);
	///dividir las filas, columnas y tercera dimension en bloques
	int NumBlocks1 = c / BLOCK;
	int NumBlocks2 = f / BLOCK;
	int NumBlocks3 = e / BLOCK;

	///cubrir todos los elementos
	if (c % BLOCK) NumBlocks1++;
	if (f % BLOCK) NumBlocks2++;
	if (e % BLOCK) NumBlocks3++;

	dim3 dime1(NumBlocks1, NumBlocks2, NumBlocks3);
	dim3 dime2(BLOCK, BLOCK, BLOCK);
	matrizXescalarK << < dime1, dime2 >> > (d_A, B, d_C, f, c,e);

	/// Copiar de Device a Host
	cudaMemcpy(C, d_C, size, cudaMemcpyDeviceToHost);
	/// Liberar Memoria
	cudaFree(d_A); cudaFree(d_C);
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
	int fila, columna, profundidad;
	cout<<"ingrese dimensiones"<<endl;
	cin>>fila;
	columna=profundidad=fila;
	float* A = (float*)malloc(fila*columna*profundidad*sizeof(float));
	float B;
	cout<<"ingrese escalar"<<endl;
	cin>>B;
	float* C = (float*)malloc(fila*columna*profundidad*sizeof(float));
	for (int i = 0; i < fila*columna*profundidad; i++)
	{
		A[i] = i;
	}
	cout<<"Matriz"<<endl;
	mostrarM(A, fila, columna*profundidad);
	cout<<" * "<<B<<" = "<<endl;
	matrizXescalar(A, B, C, fila,columna,profundidad);
	mostrarM(C, fila, columna*profundidad);

	system("PAUSE");
	exit(0);
}
