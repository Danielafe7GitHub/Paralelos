
#include <bits/stdc++.h>
#include    <iostream>
#include    <cstdlib>
#include    <ctime>
#include    <sys/timeb.h>

using namespace std;

int rdtsc()
{
    __asm__ __volatile__("rdtsc");
}


struct matrix
{
    int**m;
    int fil;
    int col;
    matrix(int fil1, int col1)
    {
        fil=fil1;
        col=col1;
        srand(rdtsc());
        m=new int*[fil1];
        for(int i=0;i<fil1;++i)
            m[i]=new int[col1];

        for(int i=0;i<fil1;++i)
            for(int j=0;j<col1;++j)
                m[i][j]=rand()%10;
    }
};

void show_matrix(matrix my_matrix)
{
    for(int i=0;i<my_matrix.fil;++i)
    {
        for(int j=0;j<my_matrix.col;++j)
            cout<<my_matrix.m[i][j]<<" ";
        cout<<endl;
    }
}

void mult_matrix(const matrix& m1, const matrix& m2, const matrix& m3)
{
    for(int i=0;i<m1.fil;++i)
    {
        for(int j=0;j<m2.col;++j)
        {
            m3.m[i][j]=0;
            for(int k=0;k<m1.col;++k)
            {
//                cout<<m1.m[i][k]<<"   "<<m2.m[k][j]<<endl;
                m3.m[i][j]+=m1.m[i][k]*m2.m[k][j];
            }
        }
    }

}

void block_mult(const matrix& m1, const matrix& m2, const matrix& m3, int tam)
{
    int sum=0;
    int tmp=m1.col*(tam/m1.col);
    for (int a = 0; a< tmp; a+= m1.col)
    {
        for (int b = 0; b< tmp; b+= m1.col)
        {
            for (int i = 0; i < tam; i++)
            {
                for (int j = b; j < b+ m1.col; j++)
                {
                    m3.m[i][j]=0;
                    sum = m3.m[i][j];
                    for (int k = a; k < a+ m1.col; k++)
                        sum += m1.m[i][k]*m2.m[k][j];
                    m3.m[i][j] = sum;
                }
            }
        }
    }
}

int main()
{
    clock_t startC, finishC;
    struct timeb startT, finishT;
    unsigned int seconds, milliseconds;
    cout<<"ingrese longitud"<<endl;
    int n;
    cin>>n;
    matrix m1(n,n);
    matrix m2(n,n);
    matrix m3(n,n);

    cout<<"MATRIZ A : "<<endl;
    show_matrix(m1);
    cout<<"MATRIZ B: "<<endl;
    show_matrix(m2);
    cout<<"RESULTADO : "<<endl;
    show_matrix(m3);

    cout<<"Triple for anidado "<<endl;
    startC = clock();
    ftime(&startT);
    mult_matrix(m1,m2,m3);
    finishC = clock();
    ftime(&finishT);

    //seconds = finishT.time - startT.time - 1;
    milliseconds = (1000 - startT.millitm) + finishT.millitm;

    cout << "clock: "    << (finishC - startC)/CLOCKS_PER_SEC << endl;
    cout << "difftime: " << difftime(finishC, startC) << endl;
    //cout << "ftime: " << (milliseconds + seconds * 1000) << endl;

    cout<<"Por bloques"<<endl;
    startC = clock();
    ftime(&startT);
    block_mult(m1,m2,m3,3);
    finishC = clock();
    ftime(&finishT);

    //seconds = finishT.time - startT.time - 1;
    milliseconds = (1000 - startT.millitm) + finishT.millitm;

    cout << "clock: "    << (finishC - startC)/CLOCKS_PER_SEC << endl;
    cout << "difftime: " << difftime(finishC, startC) << endl;
    //cout << "ftime: " << (milliseconds + seconds * 1000) << endl;

    return 0;
}
