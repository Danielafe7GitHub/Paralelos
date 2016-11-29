///PARC
// Karelia Vilca Salinas
//Odd-Even Sort Source: http://homepages.ihug.co.nz/~aurora76/Malc/Sorting_Array.htm#Exchanging

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define N 64
int ADJMATRIX[N][N];           // matriz A  
int DEGREE[N];                // matriz DEGREE
int COLOR[N];
int SORT[N];
int D2MATRIX[N][N];
int FORBIDDEN[N][2*N-1];
int lambda=0;
int ci=0;


int compare(const void *, const void *);
void mergeArrays(int *, int *, int *, int, int);
int computeNeighbor(int, int, int);

int main(int argc, char ** argv)
{ 
  int i, j, n, rank, size;
 double start_time, end_time,runtime;
  MPI_Status status;
  start_time = MPI_Wtime();
  MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int numElements = N/size;
    int * arr = (int *) malloc(sizeof(int)*numElements);
    int * temp = (int *) malloc(sizeof(int)*numElements*2);
    int * recvArr = (int *) malloc(sizeof(int)*numElements);
    int * fullArr = NULL;

    if(rank == 0)
      fullArr = (int *) malloc(sizeof(int)*N);

    //Fill array in descending order
    int start = rank*numElements;
    int end = start+numElements;
    for (i=0; i<N; i++)
     {    for (j=0; j<N; j++)
            {
		if(i==j)	ADJMATRIX[i][j]=0;	
		else {ADJMATRIX[i][j]=(1+j)%2;
		     //printf("!!");
		     }
	    }
      }
    for (i=0; i<N; i++)
         for (j=0; j<2*N-1; j++)
            {
		FORBIDDEN[i][j]=0;
	    }

    //Begin iterations
///PASO 1
for(i=0;i<N;++i){
   DEGREE[i]=0;
   COLOR[i]=0;
   for(int l =0; l<N;++l)
   {
	if(ADJMATRIX[i][l]==1)
		{DEGREE[i]=DEGREE[i]+1;
		//printf("GRADO AUMENTA");		
		}
   }
}
///SORT
  for(i = 0, j = start; i < numElements; i++, j++)
      arr[i] = DEGREE[i];

    //Sort array with odd-even sort

    //Sort local values
    qsort(arr, numElements, sizeof(int), compare);
    for(n = 0; n < size; n++) {
      
      MPI_Barrier(MPI_COMM_WORLD);

      int neighbor = computeNeighbor(n, rank, size);

      if(neighbor >= 0 && neighbor < size)
      {
        //Send my values to my neighbor and receive values from my neighbor
        MPI_Sendrecv(arr, numElements, MPI_INT, neighbor, n, 
               recvArr, numElements, MPI_INT, neighbor, n, 
               MPI_COMM_WORLD, &status);

        //If my rank < my neighbor's rank, keep the smaller values
        if(rank < neighbor)
	{
          mergeArrays(arr, recvArr, temp, numElements, 1);
        //Else keep the larger values
        } 
	else {
          mergeArrays(arr, recvArr, temp, numElements, 0);
        }
      }
    }

    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Gather(arr, numElements, MPI_INT, fullArr, numElements, MPI_INT, 0, MPI_COMM_WORLD);
//SORT
    if(rank == 0)
    {
      for(i = 0; i < N; i++)
        {SORT[N-i]=fullArr[i];
	//printf("%d ", SORT[i]);
	}
    }
    free((void *) arr);
    free((void *) recvArr);
    free((void *) temp);

  MPI_Finalize();

for(int j=0;j<N;++j)///parbegin
{
	for(int i=0;i<N;i++)
	{
		if(ADJMATRIX[i][j]==1)
		{
			for(int m =0; m<N;++m)
			{
				if(ADJMATRIX[j][m]==1 && m!=i)
					{
					//printf("forbi4");
					D2MATRIX[i][m]=1;				
					}
			}		
		}
	}
 

}
//lambda=0;
int c=lambda;
for(int j=0;j<N;j++)
{
	for(int i =0; i<N;i++)
	{
		if(FORBIDDEN[SORT[j]][lambda]==0)	
		{ lambda ++;
                  c=lambda;
                  break;
		} 
	}
	COLOR[SORT[j]]=c;
        
	
	for(int i =0; i<N;i++)
	{
		if(ADJMATRIX[SORT[j]][i]==1 && c!=0)
		  {	//printf("la 1");
			FORBIDDEN[i][c-1]=1;
		  }
		if(ADJMATRIX[SORT[j]][i]==1)
		  {	//printf("la 2");
			FORBIDDEN[i][c]=1;
		  }
		if(ADJMATRIX[SORT[j]][i]==1 && c!=2*N-2)
		  {	//printf("la 3");			
			FORBIDDEN[i][c+1]=1;
		  }
		if(D2MATRIX[SORT[j]][i]==1 )
                  {     
			FORBIDDEN[i][c]=1;
                         //printf("forbi %d",SORT[j]);	
	          }
	}
}
    /*if(rank == 0)
    {
      for(i = 0; i < N; i++)
        printf("cuki%d ", fullArr[i]);
      printf("\n"); 
    }*/
  printf("lamdaxdx %d ",lambda);
end_time= MPI_Wtime();
runtime=end_time - start_time;
printf("Codigo se ejecuto en %f segundos\n",runtime);

  return 0;
}

int compare(const void * a, const void * b)
{
  if( *((int *)a) <  *((int *)b) ) return -1;
  else if( *((int *)a) == *((int *)b) ) return 0;
  else return 1;
}

int computeNeighbor(int phase, int rank, int size)
{
  int neighbor;
  if(phase % 2 != 0) {  //odd phase
    if(rank % 2 != 0) {  //odd rank
      neighbor = rank + 1;
    } else {  //even rank
      neighbor = rank - 1;
    }
  } else {  //even phase
    if(rank % 2 != 0) {  //odd rank
      neighbor = rank - 1;
    } else {  //even rank
      neighbor = rank + 1;
    }    
  }

  if(neighbor < 0 || neighbor >= size)
    neighbor = -1;
  return neighbor;
}

void mergeArrays(int * arr, int * neighborArr, int * temp, int size, int low_or_high)
{
  int i, j, k;
  i = j = k = 0;

  //Assume arrays are already sorted
  for(i = 0, j = 0, k = 0; i < size*2; i++)
  {
    if(j < size && k < size)
    {
      if(arr[j] < neighborArr[k])
      {
        temp[i] = arr[j];
        j++;
      } else {
        temp[i] = neighborArr[k];
        k++;
      }
    } else if(j < size) {
      temp[i] = arr[j];
      j++;
    } else {
      temp[i] = neighborArr[k];
      k++;
    }
  }

  if(low_or_high % 2 != 0)
    for(i = 0; i < size; i++)
      arr[i] = temp[i];
  else
    for(i = size, j=0; j < size; i++,j++)
      arr[j]= temp[i];
}
