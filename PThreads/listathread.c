#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <sys/time.h>

struct list_node_s
{
	int data;
	struct list_node_s* next;
        ///SOLUCION 2 lock individual nodes
	pthread_mutex_t mutex;
};

double timeval_diff(struct timeval *a, struct timeval *b)
{
  return
    (double)(a->tv_sec + (double)a->tv_usec/1000000) -
    (double)(b->tv_sec + (double)b->tv_usec/1000000);
}
int thread_count;
struct list_node_s** head_pp;
pthread_mutex_t mutex;
pthread_mutex_t mutex2;
pthread_rwlock_t rwlock;
pthread_mutex_t head_p_mutex;
long thread;
long operaciones=100000;
double member=80;//99.9;
double insert=10;//0.05;
double del=10;//0.05;
struct timeval t_ini, t_fin;
double secs;

int MemberP(int value)
{
	struct list_node_s* temp_p;
	pthread_mutex_lock(&head_p_mutex);
	temp_p=*head_pp;
	while(temp_p != NULL && temp_p->data<value)
	{
		if (temp_p->next != NULL)
			pthread_mutex_lock(&(temp_p->next->mutex));
		if (temp_p == *head_pp)
			pthread_mutex_unlock(&head_p_mutex);
		pthread_mutex_unlock(&(temp_p->mutex));
		temp_p=temp_p->next;
	}
	if (temp_p == NULL || temp_p->data >value)
	{
		if (temp_p == *head_pp)
			pthread_mutex_unlock(&head_p_mutex);
		if (temp_p != NULL)
			pthread_mutex_unlock(&(temp_p->mutex));
		return 0;
	}
	else
	{
		if (temp_p == *head_pp)
			pthread_mutex_unlock(&head_p_mutex);
		pthread_mutex_unlock(&(temp_p->mutex));
		return 1;
	}
}
int Member(int value)//struc list_node_s* head_p
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
	while(curr_p != NULL && curr_p->data<value)//anterior inmediato
	{
		pred_p=curr_p;
		curr_p=curr_p->next;
	}
	if(curr_p == NULL || curr_p->data > value)//proximo
	{
		temp_p=malloc(sizeof(struct list_node_s));
		temp_p->data=value;
		temp_p->next=curr_p;
		if (pred_p == NULL)
			*head_pp=temp_p;//unico elemento
		else
			pred_p->next=temp_p;//linkear
		return 1;
	}
	else
		return 0;
}
int Delete(int value)
{
	struct list_node_s* curr_p=*head_pp;
	struct list_node_s* pred_p= NULL;
	while(curr_p != NULL && curr_p->data < value)//busca
	{
		pred_p=curr_p;
		curr_p=curr_p->next;
	}
	if(curr_p != NULL && curr_p->data == value)//si
	{
		if(pred_p == NULL)//si era cabeza
		{
			*head_pp=curr_p->next;
			free(curr_p);
		}
		else            //linkea
		{
			pred_p->next=curr_p->next;
			free(curr_p);
		}
		return 1;
	}
	else					//no
		return 0;
}


void* Mutex(void* r)
{ long ops=(long) r;
	//printf("Operaciones %li \n",ops);
        
	for(int j=0;j<ops*member/100;j++)
	{	
		pthread_mutex_lock(&mutex);		
		Member(rand()%10000);
		pthread_mutex_unlock(&mutex);
	}	
	for(int j=0;j<ops*insert/100;j++)
	{	
		pthread_mutex_lock(&mutex);		
		Insert(rand()%10000);
		pthread_mutex_unlock(&mutex);
	}
 	for(int j=0;j<ops*del/100;j++)
	{	
		pthread_mutex_lock(&mutex);		
		Delete(rand()%10000);
		pthread_mutex_unlock(&mutex);
	}
}




void* MutexNode(void* r)
{ long ops=(long) r;
	printf("Operaciones %li \n",ops);

	for(int j=0;j<ops*member/100;j++)
		MemberP(rand()%10000);
	for(int j=0;j<ops*insert/100;j++)
		Insert(rand()%10000);
 	for(int j=0;j<ops*del/100;j++)
		Delete(rand()%10000);
}
void* RW(void* r)
{ long ops=(long) r;
	for(int j=0;j<ops*member/100;j++)
	{	
		pthread_rwlock_rdlock(&rwlock);		
		Member(rand()%10000);
		pthread_rwlock_unlock(&rwlock);
	}	
	for(int j=0;j<ops*insert/100;j++)
	{	
		pthread_rwlock_wrlock(&rwlock);		
		Insert(rand()%10000);
		pthread_rwlock_unlock(&rwlock);
	}
 	for(int j=0;j<ops*del/100;j++)
	{	
		pthread_rwlock_wrlock(&rwlock);		
		Delete(rand()%10000);
		pthread_rwlock_unlock(&rwlock);
	}
}
int main(int argc,char* argv[])
{

	
	pthread_t* thread_handles;
	struct list_node_s* head;
//cabeza
	head=malloc(sizeof(struct list_node_s));
	head->data=0;
	head->next=NULL;
	head_pp=&head;

	thread_count=strtol(argv[1],NULL,10);
	thread_handles=malloc (thread_count*sizeof(pthread_t));
	
	///1000initia keys	
        for(int i =0; i<1000;++i)
		Insert(i); 
        
        long op=operaciones/thread_count; 

///MUTEX
        gettimeofday(&t_ini, NULL);
	for(thread=0;thread<thread_count;thread++)
	{
          pthread_create(&thread_handles[thread],NULL,Mutex,(void *)op);
          //printf("Operadora %li \n",op);
	}
	for(thread=0;thread<thread_count;thread++)
		pthread_join(thread_handles[thread],NULL);
	gettimeofday(&t_fin, NULL);
        secs = timeval_diff(&t_fin, &t_ini);
	printf("%.16g Mutex segundos\n", secs );
	free(thread_handles);

///MUTEX NODE
	gettimeofday(&t_ini, NULL);
	for(thread=0;thread<thread_count;thread++)
	{
          pthread_create(&thread_handles[thread],NULL,MutexNode,(void *)op);
          //printf("Operadora %li \n",op);
	}
	for(thread=0;thread<thread_count;thread++)
		pthread_join(thread_handles[thread],NULL);
	gettimeofday(&t_fin, NULL);
	 secs = timeval_diff(&t_fin, &t_ini);
	printf("%.16g MutexNODE segundos\n", secs );
	free(thread_handles);
 /*
///Read-Write
	gettimeofday(&t_ini, NULL);
	for(thread=0;thread<thread_count;thread++)
	{//printf("Operadora %li \n",op);
          pthread_create(&thread_handles[thread],NULL,RW,(void *)op);
          //
	}
        for(thread=0;thread<thread_count;thread++)
		pthread_join(thread_handles[thread],NULL);
	gettimeofday(&t_fin, NULL);
	secs = timeval_diff(&t_fin, &t_ini);
	printf("%.16g Read-Write segundos\n", secs );
	free(thread_handles);
                
  */
	return 0;
}
