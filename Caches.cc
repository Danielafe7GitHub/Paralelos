#include <bits/stdc++.h>
//const double CLK_TCK = 1000.0;
using namespace std;

int main()
{
    int i, j, maxi;
    maxi=300;
    clock_t start, end;
    double A[maxi][maxi];
    double X[maxi];
    double Y[maxi];
    for(i=0; i<maxi; ++i)
      for(j=0;j<maxi;++j)
        A[i][j]=i+j;
    for(j=0;j<maxi;++j)
        X[j]=j*j;
    cout<<"______________Primero__________________________________"<<endl;
    start = clock();
    for(i=0; i<maxi; ++i)
      for(j=0;j<maxi;++j)
        Y[i]+=A[i][j]*X[j];
    end = clock();
    printf("El tiempo es: %f\n", (end - start) / 1000.0);

     cout<<"______________Segundo__________________________________"<<endl;
    start = clock();
    for(j=0; j<maxi; ++j)
      for(i=0;i<maxi;++i)
        Y[i]+=A[i][j]*X[j];
    end = clock();
    printf("El tiempo es: %f\n", (end - start) / 1000.0);


    return 0;
}
