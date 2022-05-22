#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>

float * filtre(float *echantillon,int filtre_freq, int filtre_large, int size) {

double *filtre=NULL; //tableau calculé par la transformée de Fourier
    filtre=malloc((size) * sizeof(double));
    if (filtre == NULL)
    {
        exit(0);
    }

    for(int i=0; i<size; i++)
    {

        filtre[i]=0;
    }

//   printf("frequence du filtre (Hz) ?");
//   scanf("%f",&filtre_freq);
//    printf("largeur du filtre ?");
//  scanf("%f",&filtre_large);
 




    for(int i=0; i<size; i++)
    {
        if(i>(filtre_freq-(filtre_large/2))&&i<(filtre_freq+(filtre_large/2))) // si i est dans la bande de fréquences recherchée
        {
            filtre[i]=0.5*(1-cos(2*M_PI*(i-(filtre_freq-(filtre_large/2)))/(filtre_large-1))); //plus on s'éloigne de la frequence centrale plus le coefficient du filtre est faible
        }
        else if (i>(size-1-filtre_freq-(filtre_large/2))&&i<(size-1-filtre_freq+(filtre_large/2))) // même chose mais dans le repliement du spectre 
        {
            filtre[i]=0.5*(1-cos(2*M_PI*(i-(size-1-filtre_freq-(filtre_large/2)))/(filtre_large-1)));
        }
        else
        {
            filtre[i]=0; // sinon on coupe totalement la fréquence
        }
    }

    for(int i=0; i<size/2; i++)
    {
        echantillon[2*i]=filtre[2*i]*echantillon[2*i]; // on applique alors le filtre sur l'échantillon
        echantillon[2*i+1]=filtre[2*i]*echantillon[2*i+1];
    }

    free(filtre); //libération de mémoire 
    return echantillon;



}
