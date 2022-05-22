#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include<SDL2/SDL.h>

#include "coloration.h"
  

#define SWAP(a,b) tempr=(a);(a)=(b);(b)=tempr


void four1(float data[], unsigned long nn, int isign)
{
    unsigned long n,mmax,m,j,istep,i;
    double wtemp,wr,wpr,wpi,wi,theta;
    float tempr,tempi;
    n=nn << 1;
    j=1;
    for (i=1; i<n; i+=2)
    {
        if (j > i)
        {
            SWAP(data[j],data[i]);
            SWAP(data[j+1],data[i+1]);
        }
        m=nn;
        while (m >= 2 && j > m)
        {
            j -= m;
            m >>= 1;
        }
        j += m;
    }
    mmax=2;
    while (n > mmax)
    {
        istep=mmax << 1;
        theta=isign*(6.28318530717959/mmax);
        wtemp=sin(0.5*theta);
        wpr = -2.0*wtemp*wtemp;
        wpi=sin(theta);
        wr=1.0;
        wi=0.0;
        for (m=1; m<mmax; m+=2)
        {
            for (i=m; i<=n; i+=istep)
            {
                j=i+mmax;
                tempr=wr*data[j]-wi*data[j+1];
                tempi=wr*data[j+1]+wi*data[j];
                data[j]=data[i]-tempr;
                data[j+1]=data[i+1]-tempi;
                data[i] += tempr;
                data[i+1] += tempi;
            }
            wr=(wtemp=wr)*wpr-wi*wpi+wr;
            wi=wi*wpr+wtemp*wpi+wi;
        }
        mmax=istep;
    }
}
struct wavfile //définit la structure de l entete d un wave
{
    char        id[4];          // doit contenir "RIFF"
    int         totallength;        // taille totale du fichier moins 8 bytes
    char        wavefmt[8];     // doit etre "WAVEfmt "
    int         format;             // 16 pour le format PCM
    short       pcm;              // 1 for PCM format
    short       channels;         // nombre de channels
    int         frequency;          // frequence d echantillonage
    int         bytes_per_second;   // nombre de bytes par secondes
    short       bytes_by_capture;
    short       bits_per_sample;  // nombre de bytes par echantillon
    char        data[4];        // doit contenir "data"
    int         bytes_in_data;      // nombre de bytes de la partie data
};

// retourne la fondamentale du spectre d'un signal en se basant sur la formule fo = argmaxlogprodmodcarréX(if)


float fondamentale(float *y, int N, float hf) 
{
    float *T = malloc(sizeof(float)*N);

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

    for (int j=0;j<N;j++) {
        if (T[j]>max) {
            max=T[j];
            Imax= j;

        }

    }

    float fo = (Imax)*hf/N;
    printf("Imax = %d \n", Imax);
    free(T);
 //   Ind_max=&Imax;
    return fo;
}


float gamme (float f) {

    int n ;
    float fo = 27.5 ;// fondamentale d'un clavier de piano (en Hz)
    n = 12*log2(f/fo);

    return n;
}

int main(int argc, char *argv[])
{



    int i=0;
    unsigned long taille=1; //variable qui correspondra par la suite a la longueur du tableau (puissance de 2)
    int nbech=0; //nombre d echantillons extraits du fichier audio
    int puissance=0; //variable qui serta incrémentée qui correspond a l indice de la puissance de 2 pour la taille du tableau
    char fichieraudio[100];
    char fichieraudio_out[100];
    float tempstotal=0;
    float filtre_freq, filtre_large;


    /*---------------------selection du fichier audio-------------------------------*/
    printf ("entrer le nom du fichier audio a analyser :\n");
    scanf("%s", fichieraudio);
    printf ("nom du fichier : %s\n", fichieraudio);
    /*--------------fin de selection du fichier audio-------------------------------*/



    /*---------------------ouverture du wave----------------------------------------*/
    FILE *wav = fopen(fichieraudio,"rb"); //ouverture du fichier wave
    struct wavfile header; //creation du header

    if ( wav == NULL )
    {
        printf("\nne peut pas ouvrir le fichier demande, verifier le nom\n");
        printf("ne pas oublier l'extention .wav\n");
        exit(1);
    }
    /*---------------------fin de ouverture du wave---------------------------------*/



    /*---------------------lecture de l entete et enregistrement dans header--------*/
    //initialise le header par l'entete du fichier wave
    //verifie que le fichier posséde un entete compatible
    if ( fread(&header,sizeof(header),1,wav) < 1 )
    {
        printf("\nne peut pas lire le header\n");
        exit(1);
    }
    if (    header.id[0] != 'R'|| header.id[1] != 'I'|| header.id[2] != 'F'|| header.id[3] != 'F' )
    {
        printf("\nerreur le fichier n'est pas un format wave valide\n");
        exit(1);
    }
    if (header.channels!=1)
    {
        printf("\nerreur : le fichier n'est pas mono\n");
        exit(1);
    }
    /*----------------fin de lecture de l entete et enregistrement dans header-------*/



    /*-------------------------determiner la taille des tableaux---------------------*/
    nbech=(header.bytes_in_data/header.bytes_by_capture);
    printf ("\nle fichier audio contient %d echantillons\n",nbech);
    while (nbech>taille)
    {
        taille=taille*2;
        puissance=puissance+1;
    }
    printf ("nombre de points traites : 2^%d=%ld\n",puissance,taille);
    tempstotal=(1./header.frequency)*taille;
    printf ("temps total : %f s\n",tempstotal);
    taille=taille*2;
    printf("taille= %ld\n",taille);
    /*------------------fin de determiner la taille des tableaux---------------------*/



    /*---------------------creation des tableaux dynamiques--------------------------*/
    float *data=NULL; //tableau de l'onde temporelle
    data=malloc( (taille) * sizeof(float));
    if (data == NULL)
    {
        exit(0);
    }

    double *filtre=NULL; //tableau calculé par la transformée de fourrier
    filtre=malloc((taille) * sizeof(double));
    if (filtre == NULL)
    {
        exit(0);
    }


    /*---------------------fin de creation des tableaux dynamiques-------------------*/

    printf(" la fréquence d'échantillonage est %d \n", header.frequency);

    /*---------------------initialisation des tableaux dynamiques--------------------*/
    for(i=0; i<taille; i++)
    {
        data[i]=0;
        filtre[i]=0;
    }
    /*---------------------fin de initialisation des tableaux dynamiques-------------*/



    /*---------------------remplissage du tableau tab avec les echantillons----------*/
    i=0;
    short value=0;
    FILE *dat=fopen("onde.dat","w"); //fichier data des echantillons
    FILE *dat2=fopen("tf.dat","w");//fichier.dat des fft
    FILE *dat3=fopen("tf_filtre.dat","w");//fichier.dat du graph1d de pgplot
    while( fread(&value,(header.bits_per_sample)/8,1,wav) )
    {
        //lecture des echantillons et enregistrement dans le tableau
        data[2*i]=value;
        i++;
    }
    printf("\nnombre d'echantillons lus : %d\n",i);
    printf("nombre de valeurs sauvegardees %d\n",i);
    for (i=0; i<taille; i++)
    {
        fprintf(dat,"%lf\n", data[i]);
//permet de sauvegarder le tableau dans le fichier data.dat pour vérification manuelle des données
    }
    /*-----------------fin de remplissage du tableau avec les echantillons-----------*/




/*--------------------- début tableau de données et de fréquences ------------------------------------------------*/
/*
    float *y=NULL; //tableau de l'onde temporelle
    y=malloc( (taille) * sizeof(float));
    if (y== NULL)
    {
        exit(0);
    }
    float *x=NULL; //tableau de l'onde temporelle
    x=malloc( (taille) * sizeof(float));
    if (x== NULL)
    {
        exit(0);
    }





 for(i=0; i<taille/2; i++)
    {
        //y[i]=10*log10((data[2*i]/taille)*(data[2*i]/taille));
        y[i] = data[2*i] ;
        x[i]=((2*i*header.frequency/taille));
        fprintf (dat2,"%f %f\n",x[i],y[i]);

    }
*/

/*--------------------- fin tableau de données et de fréquences ------------------------------------------------*/




///////////////////////////// début de la boucle ///////////////////////////////////////

// discrétisation de notre fichier audio en fontion du temps d'affichage 
/*
    unsigned long D_t= 1 ;// temps en seconde entre chaque coloriage de touche
    unsigned long D_i=D_t*header.frequency; // nombre de points de data dans chaque échantillon.
    printf("taille de l'échantillon %ld \n", D_i);
    printf("nombre d'échantillons %ld \n", taille/(D_i));
    float *echantillon=NULL; //echantillon du tableau de l'onde temporelle.
    echantillon=malloc( D_i* sizeof(float));

// je prend de la marge en elevant le dernier échantillon qui bloque après taille/(D_i) -> taille/(D_i)-1
    for (int k=0;k<taille/(D_i);k++) {


    if (echantillon== NULL)
    {
        exit(0);
    }
    for (int j= 0;j<D_i;j++) {


            echantillon[j]=data[j];

    }
*/
    /*---------------------fft-------------------------------------------------------*/
    four1(data,taille/2,1);
    /*---------------------fin de fft------------------------------------------------*/

    /*---------------------filtre----------------------------------------------------*/
//   printf("frequence du filtre (Hz) ?");
//   scanf("%f",&filtre_freq);
//    printf("largeur du filtre ?");
//  scanf("%f",&filtre_large);
 
    filtre_freq= 10010;
    filtre_large= 19980;



    printf(" filtre_freq :%f \n filtre_large :%f \n",filtre_freq,filtre_large);

    for(i=0; i<taille; i++)
    {
        if(i>(filtre_freq-(filtre_large/2))&&i<(filtre_freq+(filtre_large/2)))
        {
            filtre[i]=0.5*(1-cos(2*M_PI*(i-(filtre_freq-(filtre_large/2)))/(filtre_large-1)));
        }
        else if (i>(taille-1-filtre_freq-(filtre_large/2))&&i<(taille-1-filtre_freq+(filtre_large/2)))
        {
            filtre[i]=0.5*(1-cos(2*M_PI*(i-(taille-1-filtre_freq-(filtre_large/2)))/(filtre_large-1)));
        }
        else
        {
            filtre[i]=0;
        }
    }

    for(i=0; i<taille/2; i++)
    {
        data[2*i]=filtre[2*i]*data[2*i];
        data[2*i+1]=filtre[2*i]*data[2*i+1];
    }
    /*
    for(i=0; i<taille; i++)
    {
        fprintf (dat3,"%f %f\n",(1.*i*header.frequency/taille),data[i]);

    }
    */
    /*--------------fin de filtre----------------------------------------------------*/



 /*---------------------début de la recherche de la fondamentale------------------------------*/
 /*   int N= taille;
    int *Ind_max=malloc(sizeof(int));
    float fo = fondamental(echantillon,N,header.frequency,Ind_max);
    printf("la fondamentale du signal est %f \n",fo);


    /*---------------------fin de la recherche de la fondamentale------------------------------*/
   
    /*---------------------fft inverse-----------------------------------------------*/
    //data_modif contient le spectre en fréquence du signal dont on ne conserve que les fréquences intéressantes
/*    float *data_modif = malloc(sizeof(float)*taille);

    for(int i=1;i<taille;i++) {
        data_modif[i]=0;
    }
    printf("%d\n",*Ind_max);
    // à modifier .
    data_modif[12442]= 2093.094;
    
    four1(data_modif,taille/2,-1);
*/
    /*--------------fin de fft inverse-----------------------------------------------*/




    /*---------------------creation fichier wave-------------------------------------*/
    /*
    printf ("entrer le nom du fichier audio filtre pour l enregistrement :\n");
    scanf("%s", fichieraudio_out);
    printf ("nom du fichier : %s\n", fichieraudio_out); 
    FILE *waveout = fopen(fichieraudio_out,"wb"); //ouverture du fichier wave
    fwrite(&header,sizeof(header),1,waveout);
    
    for(i=0; i<nbech; i++)
    {
        
   value=fabs*((data[2*i]*2)/taille); //on peut noter la normalisation /taille de la tf inverse
      fwrite(&value,(header.bits_per_sample)/8,1,waveout);
      

    }
    */ 
    /*---------------------fin de creation fichier wave------------------------------*/

    /*---------------------début de l'affichage------------------------------*/


/*
    FILE *gnuplot = popen("gnuplot", "w");
    fprintf(gnuplot, "plot '-'\n");
    for (i = 0; i < taille; i++) {
    fprintf(gnuplot, "%f %f\n", x[i], y[i]); }
    fprintf(gnuplot, "e\n"); 
    fflush(gnuplot);

*/
    /*---------------------fin de l'affichage------------------------------*/

   
    /*---------------------début test---------------------------*/


    float fo= fondamentale(data,taille,header.frequency); 
    printf("fréquence fondamentale est : %lf \n", fo);
    int n;
    n= gamme(fo);

    printf("%d\n",n);




    /*---------------------fin test---------------------------*/

    /*---------------------début coloriage---------------------------*/

    coloration(n);


    /*---------------------fin coloriage---------------------------*/

//}
    ///////////////////////fin de la boucle /////////////////////////////

    /*---------------------liberation de la memoire----------------------------------*/
    //liberation de la ram des malloc

    free(data);
    data = NULL;
//    free(filtre);
//    filtre = NULL;
    fclose(wav);
//    fclose(waveout);
//    fclose(dat);
//    fclose(dat2);
//   fclose(dat3);
//   free(echantillon);

    /*---------------------fin de liberation de la memoire---------------------------*/




    return 0;
}
