/*Write a Pthreads program that finds the average time required by your system to create and terminate a thread. Does the number of threads affect the averagetime? If so, how?*/
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <sys/time.h>
double timeval_diff(struct timeval *a, struct timeval *b)
{
  return
    (double)(a->tv_sec + (double)a->tv_usec/1000000) -
    (double)(b->tv_sec + (double)b->tv_usec/1000000);
}

int filas=80;
int columnas=80;
int thread_count;
struct timeval ini, fin;
struct timeval t_ini1, t_fin1;
struct timeval t_ini2, t_fin2;
double media1=0;
double media2=0;
void* tiempo_H(void* rango)
{
	gettimeofday(&t_fin1, NULL);
	media1 += timeval_diff(&t_fin1, &t_ini1);
	gettimeofday(&t_ini2, NULL);
	return NULL;
}
int main(int argc,char* argv[])
{

	long thread;
	pthread_t* thread_handles;
	thread_count=strtol(argv[1],NULL,10);
	thread_handles=malloc (thread_count*sizeof(pthread_t));
        gettimeofday(&t_ini1, NULL);
	for(thread=0;thread<thread_count;thread++)
	{
	    gettimeofday(&ini, NULL);
		pthread_create(&thread_handles[thread],NULL,tiempo_H,(void *)thread);
		pthread_join(thread_handles[thread],NULL);
		pthread_detach(thread_handles[thread]);
	    gettimeofday(&t_fin2, NULL);
		media2 += timeval_diff(&t_fin2, &t_ini2);
	}
        gettimeofday(&fin, NULL);
	double secs=timeval_diff(&fin, &ini);
	double secs1=media1/thread_count;
	double secs2=media2/thread_count;
  	printf("Creacion %.16g milisegundos\n", secs1 * 1000.0);
  	printf("Destruccion %.16g milisegundos\n", secs2 * 1000.0);
        printf("PROMEDIO: %.16g milisegundos\n", secs * 1000.0/thread_count);
	free(thread_handles);
	return 0;
}
