/*Write a Pthreads program that uses two condition variables and a mutex to
implement a read-write lock. Download the online linked list program that
uses Pthreads read-write locks, and modify it to use your read-write locks.
Now compare the performance of the program when readers are given pref-
erence with the program when writers are given preference. Can you make any
generalizations?*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <sys/time.h>
//////////////////////////
typedef struct {
    int Escritores;
    int EscritoresEsp;
    int Lectores;
    pthread_mutex_t mutex;
    pthread_cond_t AccesoEs;
    pthread_cond_t AccesoLec;
}mutex_rw;

int mutex_create( mutex_rw* x){
    x->Lectores=0;
    x->Escritores=0;
    x->EscritoresEsp=0;
    pthread_cond_init(&(x->AccesoLec),NULL);
    pthread_cond_init(&(x->AccesoEs),NULL);
    pthread_mutex_init(&(x->mutex),NULL);
    return 1;
}      
void mutex_rdlock( mutex_rw* x){//lector bloquea
    pthread_mutex_lock(&(x->mutex));
    while(x->Escritores > 0)///Mientras haya escritores
        pthread_cond_wait(&(x->AccesoLec), &(x->mutex));///desbloquea el mutex y espera por la conficion de acceso lec
    x->Lectores++;
    pthread_mutex_unlock(&(x->mutex));   
   
}
void mutex_rdunlock( mutex_rw* x){//lector desbloquea
    pthread_mutex_lock(&(x->mutex));
        x->Lectores--;
        if(x->Lectores==0 && x->EscritoresEsp>0)
            pthread_cond_signal(&(x->AccesoEs));///recomiensa ine de los hilos que esperan las condicion , si no hay lista de espera no hace nada 
        pthread_mutex_unlock(&(x->mutex));
}
void mutex_wrlock( mutex_rw* x) /// escritor bloquea
{
        pthread_mutex_lock(&(x->mutex));
        x->EscritoresEsp++;
        while(x->Lectores>0 || x->Escritores>0)
        {
            pthread_cond_wait(&(x->AccesoEs), &(x->mutex));
        }
        x->EscritoresEsp--; x->Escritores++;
        pthread_mutex_unlock(&(x->mutex));
}
void mutex_wrunlock( mutex_rw* x)///escritor desbloquea
{
        pthread_mutex_lock(&(x->mutex));
        x->Escritores--;
        if(x->EscritoresEsp>0)
            pthread_cond_signal(&(x->AccesoEs));
        pthread_cond_broadcast(&(x->AccesoLec));
        pthread_mutex_unlock(&(x->mutex));
}
//////////////////////////
double timeval_diff(struct timeval *a, struct timeval *b)
{
  return
    (double)(a->tv_sec + (double)a->tv_usec/1000000) -
    (double)(b->tv_sec + (double)b->tv_usec/1000000);
}
struct list_node_s
{
    int data;
    struct list_node_s* next;
    pthread_mutex_t mutex;
};


int thread_count;
struct list_node_s** head_pp;
pthread_mutex_t mutex;
pthread_mutex_t head_p_mutex;
pthread_rwlock_t  rwlock;



int MemberP(int value)
{
    struct list_node_s* temp_p;
    pthread_mutex_lock(&head_p_mutex);
    temp_p=*head_pp;
    while(temp_p != NULL && temp_p->data<value)
    {
        if (temp_p->next != NULL)
        {
            pthread_mutex_lock(&(temp_p->next->mutex));
        }
        if (temp_p == *head_pp)
        {
            pthread_mutex_unlock(&head_p_mutex);
        }
        pthread_mutex_unlock(&(temp_p->mutex));
        temp_p=temp_p->next;
    }
    if (temp_p == NULL || temp_p->data >value)
    {
        if (temp_p == *head_pp)
        {
            pthread_mutex_unlock(&head_p_mutex);
        }
        if (temp_p != NULL)
        {
            pthread_mutex_unlock(&(temp_p->mutex));
        }
        return 0;
    }
    else
    {
        if (temp_p == *head_pp)
        {
            pthread_mutex_unlock(&head_p_mutex);
        }
        pthread_mutex_unlock(&(temp_p->mutex));
        return 1;
    }
}
int Member(int value)
{
    struct list_node_s* curr_p=*head_pp;
    while(curr_p!=NULL && curr_p->data < value)
        curr_p=curr_p->next;
    if(curr_p == NULL || curr_p->data >value)
        return 0;
    else
        return 1;
}
int Insert(int value)
{
    struct list_node_s* curr_p= *head_pp;
    struct list_node_s* pred_p= NULL;
    struct list_node_s* temp_p;
    while(curr_p != NULL && curr_p->data<value)
    {
        pred_p=curr_p;
        curr_p=curr_p->next;
    }
    if(curr_p == NULL || curr_p->data > value)
    {
        temp_p=malloc(sizeof(struct list_node_s));
        temp_p->data=value;
        temp_p->next=curr_p;
        if (pred_p == NULL)
            *head_pp=temp_p;
        else
            pred_p->next=temp_p;
        return 1;
    }
    else
        return 0;
}
int Delete(int value)
{
    struct list_node_s* curr_p=*head_pp;
    struct list_node_s* pred_p= NULL;
    while(curr_p != NULL && curr_p->data < value)
    {
        pred_p=curr_p;
        curr_p=curr_p->next;
    }
    if(curr_p != NULL && curr_p->data == value)
    {
        if(pred_p == NULL)
        {
            *head_pp=curr_p->next;
            free(curr_p);
        }
        else
        {
            pred_p->next=curr_p->next;
            free(curr_p);
        }
        return 1;
    }
    else
        return 0;
}
void* LLamarFunc(void* rango)
{   //Mi Estructura mutex_rw
	//mutex_rw n;
    mutex_rw n_rwlock;
    mutex_create(&n_rwlock);
	//////////////////
    long mi_rango=(long) rango;
    printf("Thread numero %ld\n",mi_rango);

    mutex_wrlock(&n_rwlock);
    Insert((int)mi_rango);
    mutex_wrunlock(&n_rwlock);

    mutex_rdlock(&n_rwlock);
    int numero=Member((int)mi_rango);
    mutex_rdunlock(&n_rwlock);

    printf("Presente %d\n",numero);
    return NULL;
}
int main(int argc,char* argv[])
{

    long thread;
    pthread_t* thread_handles;
    struct list_node_s* head;
    head=malloc(sizeof(struct list_node_s));
    head->data=0;
    head->next=NULL;
    head_pp=&head;
    thread_count=strtol(argv[1],NULL,10);
    thread_handles=malloc (thread_count*sizeof(pthread_t));
    struct timeval t_ini, t_fin;
    double secs;

    gettimeofday(&t_ini, NULL);
    for(thread=0;thread<thread_count;thread++)
	   pthread_create(&thread_handles[thread],NULL,LLamarFunc,(void *)thread);
	   
    for(thread=0;thread<thread_count;thread++)
	   pthread_join(thread_handles[thread],NULL);
    gettimeofday(&t_fin, NULL);

    secs = timeval_diff(&t_fin, &t_ini);
    printf("%.16g millisegundos\n", secs * 1000.0);
    free(thread_handles);
    return 0;
}
