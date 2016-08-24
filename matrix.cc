
#include <bits/stdc++.h>
using namespace std;

int main(int argc, char** args){
  clock_t start, end;
  int af, ac, bf, bc, i, j, k;
  printf("Filas     A-->");
  scanf("%d", &af);
  printf("Columnas  A-->");
  scanf("%d", &ac);
  printf("Filas     B-->");
  scanf("%d", &bf);
  printf("Columnas  B-->");
  scanf("%d", &bc);
  if(ac!=bf){
             printf("no es posible hacer la multiplicación\n");
             return 0;
  }
  int A[af][ac], B[bf][bc], C[af][bc]; /*Rutina para cargar los valores*/
    int var;
  printf("Autollenado (1/0)\n");
  scanf("%d", &var);
  if(var==1)
  {
        for(i=0;i<af;i++){
          for(j=0;j<ac;j++){
              A[i][j]=i;
          }
      }

      printf("\n\n");
      for(i=0;i<bf;i++){
          for(j=0;j<bc;j++){
              B[i][j]=j;
          }
      }
  }
  else
  {
      printf("Ingrese valores\n");

      for(i=0;i<af;i++){
          for(j=0;j<ac;j++){
              printf("Valor de A (%d, %d)-->",i+1, j+1);
              scanf("%d", &A[i][j]);
          }
      }

      printf("\n\n");
      for(i=0;i<bf;i++){
          for(j=0;j<bc;j++){
              printf("Valor de B (%d, %d)-->", i+1, j+1);
              scanf("%d", &B[i][j]);
          }
      }
  }
/*Triple for*/
  start= clock();
  for(i=0;i<af;i++){
      for(j=0;j<bc;j++){
          C[i][j]=0;
          for(k=0;k<ac;k++){
              C[i][j]=(C[i][j]+(A[i][k]*B[k][j]));
          }
      }
  }
  end = clock();

/*Imprimir*/
  printf("\n\n\t\t\t Matriz A");
  printf("\n");
  for(i=0;i<af;i++){
      printf("\n\t\t");
      for(j=0;j<ac;j++){
          printf("  %6d  ", A[i][j]);
      }
  }

  printf("*");

  printf("\n\n\t\t\t Matriz B");
  printf("\n");
  for(i=0;i<bf;i++){
      printf("\n\t\t");
      for(j=0;j<bc;j++){
          printf("  %6d  ", B[i][j]);
      }
  }
  printf("\n--------------------------------------------------------------------------------------\n");
  printf("\n\n\t\t\t Resultado");
  printf("\n");
  for(i=0;i<af;i++){
      printf("\n\t");
      for(j=0;j<bc;j++){
          printf("  %6d  ", C[i][j]);
      }
  }
  printf("\n");
  printf("El tiempo es: %f\n", (end - start) / 1000.0);
  ///TIEMPOS OBTENIDOS
  ///10*10   ------>0.021000 ms
  ///100*100 ------>18.115000 ms
  ///300*300 ------>174.628000 ms
}
