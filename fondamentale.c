#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>


// retourne la fondamentale du spectre d'un signal en se basant sur la formule fo = argmaxlogprodmodcarréX(if)


float fondamentale(float *y, int N, float hf, int taille) 
{
    float *T = malloc(sizeof(float)*N/2);

    for (int k =0; k< N/2; k++) {

        T[k]= fabs(y[k]);

      /*  int i=1;
        float prod = 1;
        
        while ((y[k*i+99]!=0) && (k*i<N-100)) {
            prod = prod*(fabs(y[i*k+99])*fabs(y[i*k+99]));
            i=i+1;
        }
        
        T[k]=log(prod); 

*/
    }



    float max =0;
    int Imax=0;

    for (int j=0;j<N/2;j++) {
        if (T[j]>max) {
            max=T[j];
            Imax= j;

        }

    }

    float fo = (Imax)*hf/taille;
    printf("Imax = %d \n", Imax);
    free(T);
 //   Ind_max=&Imax;
    return fo;
}

int gamme (float f) {

    int n ;
    float f_ref = 27.5 ;// fondamentale d'un clavier de piano (en Hz)

    n =12*log2f(f/f_ref);

    return n;
}
