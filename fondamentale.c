#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>


// retourne la fondamentale du spectre d'un signal en cherchant le maximum


float fondamentale(float *y, int N, float hf, int taille) 
{
    float *T = malloc(sizeof(float)*N/2);

    for (int k =0; k< N/2; k++) {

        T[k]= fabs(y[k]); //La transformée de fourier du signal contient des valeurs complexes. Pour déterminer le max, nous passons au module.

    }

// méthode du maximum : (fonctionnelle)

    float max =0;
    int Imax=0;

    for (int j=0;j<N/2;j++) {
        if (T[j]>max) {
            max=T[j];
            Imax= j;

        }

    }
// méthode du produit spectral ( non fonctionnelle )
      /*  int i=1;
        float prod = 1;
        
        while ((y[k*i+99]!=0) && (k*i<N-100)) {
            prod = prod*(fabs(y[i*k+99])*fabs(y[i*k+99]));
            i=i+1;
        }
        
        T[k]=log(prod); 

*/

    float fo = (Imax)*hf/taille;
//    printf("Imax = %d \n", Imax);
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
