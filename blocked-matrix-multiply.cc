// A simple matrix multiply code to show affect of ordering and blocking
// to compile this use gcc -O2 mm.c -lrt or icc -O2 mm.c -lrt

#include <bits/stdc++.h>

#define MATSIZE 8000
#define BLOCKSIZE 8

void setmat(int n, int m, double **a)
{
   int i, j ;

   for (i=0;i<n; i++)
      for (j = 0 ; j<m; j++)
      {
         a[i][j] = 0.0 ;
      }
}

void fillmat(int n, int m, double **a)
{
   int i, j ;

   for (i = 0; i<n; i++)
      for (j = 0 ; j < m; j++)
      {
         a[i][j] = (double)i*j ;
      }
}

void abasicmm(int n, int m,double **a, double **b, double** c)
{
   int i, j, k ;

   for (i=0;i<n; i++)
      for (j = 0; j<n; j++)
         for (k=0;k<n; k++)
            a[i][j] += b[i][k]* c[k][j] ;
}

void abettermm(int n, int m, double **a, double **b, double** c)
{
   int i, j, k ;

   for (i=0;i<n; i++)
      for (k=0;k<n; k++)
         for (j = 0; j<n; j++)
            a[i][j] += b[i][k]* c[k][j] ;
}
void ablockmm(int n,int m,double **a, double **b, double** c, int blockSize)
{
   int i, j, k, iInner, jInner, kInner ;
   for (i = 0; i < n; i+=blockSize)
      for (k = 0 ; k < n; k+=blockSize)
         for (j=0; j<n ; j+= blockSize)
            for (iInner = i; iInner<i+blockSize; iInner++)
               for (kInner = k ; kInner<k+blockSize ; kInner++)
                  for (jInner = j ; jInner<j+blockSize; jInner++)
                        a[iInner][jInner] += b[iInner][kInner] * c[kInner][jInner] ;
}


int main(int argc, char *argv[])
{

   double **a, **b, **c, **aa ;
   unsigned int n ;
   unsigned i, j, k, iInner, jInner, kInner, blockSize ;
   struct timespec ts1, ts2, ts3, ts4, ts5, ts6, ts7 ;

  // n = MATSIZE ;
  // blockSize = BLOCKSIZE ;
   if (argc != 3)
   {
      printf("Ingresar la longitud\n") ;
     // exit(0);
   }

   n =4; //atoi(argv[1]) ;printf("Aqui\n") ;
   blockSize = n/2;//atoi(argv[2]) ;
   printf("longitud de la matriz %d tamaño de bloque %d\n", n,blockSize) ;   printf("hola") ;

   if (n%blockSize)
   {
      printf("longitud de la matriz debe ser multiplo del tamaño de bloque \n  Reinicie \n") ;
      exit(0);
   }
// allocate matrices

   a = (double **)calloc((n+blockSize)*(n+blockSize), sizeof(double)) ;
   b = (double **)calloc((n+blockSize)*(n+blockSize), sizeof(double)) ;
   c = (double **)calloc((n+blockSize)*(n+blockSize), sizeof(double)) ;
   aa = (double **)calloc((n+blockSize)*(n+blockSize), sizeof(double)) ;
   if (aa == NULL)
   {
     printf("memoria insuficiente \n") ;
     exit(0) ;
   }

// fill matrices

   setmat(n, n, a) ;
   setmat(n, n, aa) ;


   fillmat(n,n,b) ;
   fillmat(n,n,c) ;

   clock_t start, end;

   start = clock();
   abasicmm (n,n,a,b,c) ;
   end = clock();
   printf("El tiempo es: %f\n", (end - start) / 1000.0);

   setmat(n, n, a) ;

   start = clock();
   abettermm (n,n,a,b,c) ;
   end = clock();
   printf("El tiempo es: %f\n", (end - start) / 1000.0);

   start = clock();
   ablockmm (n, n, aa, b, c, blockSize) ;
   end = clock();
   printf("El tiempo es: %f\n", (end - start) / 1000.0);

   printf("Multiplicaciones terminadas \n") ;
}

