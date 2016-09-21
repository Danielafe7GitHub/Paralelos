#include <bits/stdc++.h>
#include <pthread.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <sys/time.h>
////g++ -std=c++11 mat.cpp -lpthread -o foo

using namespace std;

int     thread_count;
long int     m, n;
struct timeval ini, fin;
struct timeval t_ini1, t_fin1;
struct timeval t_ini2, t_fin2;
int* A;
int* x;
int* y;
int* z;
double timeval_diff(struct timeval *a, struct timeval *b)
{
  return
    (double)(a->tv_sec + (double)a->tv_usec/1000000) -
    (double)(b->tv_sec + (double)b->tv_usec/1000000);
}
void *Pth_mat_vect(void* rank);

int rdtsc()
{
    __asm__ __volatile("rdtsc");
}

int main()
{
    srand(rdtsc());

    long thread;
    pthread_t* thread_handles;

    cout<<"Numero de threads : "<<endl;
    cin>>thread_count;

    thread_handles = new pthread_t[thread_count];

    cout<<"Ingresar m y n : "<<endl;
    cin>>m>>n;

    A=new int[m*n];
    x=new int[n];
    y=new int[m];
    z=new int[m];

    cout<<"MATRIZ : "<<endl;
    for(int i=0;i<m;++i)
    {
        for(int j=0;j<n;++j)
        {
            A[i*n +j]=rand()%9 +1;
            //cout<<A[i*n +j]<<" ";
        }
        //cout<<endl;
    }

    cout<<"VECTOR : "<<endl;
    for(int i=0;i<n;++i)
    {
        x[i]=rand()%9 +1;
        //cout<<x[i]<<endl;
    }
 ///PARALELO
    gettimeofday(&ini, NULL);
    for(thread = 0; thread < thread_count; thread++)
        pthread_create(&thread_handles[thread], NULL, Pth_mat_vect, (void*) thread);

    for (thread = 0; thread < thread_count; thread++)
        pthread_join(thread_handles[thread], NULL);
    gettimeofday(&fin, NULL);
 ///SECUENCIAL
    gettimeofday(&t_ini1, NULL);
    for (int i = 0; i <= m; i++)
    {
        z[i] = 0.0;
        for (int j = 0; j < n; j++)
            z[i] += A[i*n+j]*x[j];
    }
	
    gettimeofday(&t_fin1, NULL);

    double paralelo=timeval_diff(&fin, &ini);
    double secuencial=timeval_diff(&t_fin1, &t_ini1);
    double speedup= secuencial/paralelo;
    double eficiencia=speedup/thread_count;
    printf("Time: %.16g segundos\n", paralelo * 1.0);
    printf("Eff: %.16g \n", eficiencia * 1.0);

    /*cout<<"RESULTADO : "<<endl;
    for(int i=0;i<m;++i)
        cout<<y[i]<<endl;*/


    free(A);
    free(x);
    free(y);
    free(thread_handles);

    return 0;
}

void *Pth_mat_vect(void* rank)
{   
    long my_rank = (long) rank;
    int i, j;
    int local_m = m/thread_count;
    int my_first_row = my_rank*local_m;
    int my_last_row = my_first_row + local_m - 1;

    for (i = my_first_row; i <= my_last_row; i++)
    {
        y[i] = 0.0;
        for (j = 0; j < n; j++)
            y[i] += A[i*n+j]*x[j];
    }
    return NULL;
}
