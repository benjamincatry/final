#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include<SDL2/SDL.h>

#include "coloration.h"
#include "fft.h"
#include "fondamentale.h"
#include "filtre.h"
  

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




int main(int argc, char *argv[])
{

    /*---------------------initialisation des variables-------------------------------*/

    int i=0;
    unsigned long taille=1; //variable qui correspondra par la suite a la longueur du tableau (puissance de 2)
    int nbech=0; //nombre d echantillons extraits du fichier audio
    int puissance=0; //variable qui serta incrémentée qui correspond a l indice de la puissance de 2 pour la taille du tableau
    char fichieraudio[100];
    char fichieraudio_out[100];
    float tempstotal=0;
    float filtre_freq, filtre_large;


    /////////////////////////// Début transciption du fichier audio////////////////////////////////
    printf ("entrer le nom du fichier audio a analyser :\n");
    scanf("%s", fichieraudio);
    printf ("nom du fichier : %s\n", fichieraudio);
   


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

    /*---------------------fin de creation des tableaux dynamiques-------------------*/

    printf(" la fréquence d'échantillonage est %d \n", header.frequency);

    /*---------------------initialisation des tableaux dynamiques--------------------*/
    for(i=0; i<taille; i++)
    {
        data[i]=0;

    }
    /*---------------------fin de initialisation des tableaux dynamiques-------------*/



    /*---------------------remplissage du tableau tab avec les echantillons----------*/
    i=0;
    short value=0;
    
    while( fread(&value,(header.bits_per_sample)/8,1,wav) )
    {
        //lecture des echantillons et enregistrement dans le tableau
        data[2*i]=value;
        i++;
    }
    printf("\nnombre d'echantillons lus : %d\n",i);
    printf("nombre de valeurs sauvegardees %d\n",i);
    
    /*-----------------fin de remplissage du tableau avec les echantillons-----------*/

/////////////////////////// fin transciption du fichier audio////////////////////////////////



///////////////////////////// début de la boucle ///////////////////////////////////////

// discrétisation de notre fichier audio en fontion du temps d'affichage 

    int D_t= 1 ;// temps en seconde entre chaque coloriage de touche
    int D_i=D_t*header.frequency; // nombre de points de data dans chaque échantillon.
    printf("taille de l'échantillon %d \n", D_i);
    printf("nombre d'échantillons %ld \n", taille/(D_i));

    float *echantillon=NULL; //echantillon du tableau de l'onde temporelle.
    echantillon=malloc( D_i* sizeof(float));

    /*-------------------------inititiation du coloriage------------------------------*/

    SDL_Window *window = NULL ;

    SDL_Init(SDL_INIT_VIDEO) ;
    if ( SDL_Init(SDL_INIT_VIDEO) !=0 ) {
        SDL_Log(" ERREUR : Initialisation SDL > %s\n ", SDL_GetError()) ;
        exit(EXIT_FAILURE) ;
    }
    
window = SDL_CreateWindow(" Piano SDL ",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED, 1716,225,0) ; // 1617 = 49 *33
if (window==NULL) {
        SDL_Log( "ERREUR : window > %s\n ", SDL_GetError()) ;
        exit(EXIT_FAILURE) ;
    }


// je prend de la marge en elevant le dernier échantillon qui bloque après taille/(D_i) -> taille/(D_i)-1
    for (int k=0;k<taille/(D_i);k++) {


    if (echantillon== NULL)
    {
        exit(0);
    }

    for (int j= 0;j<D_i;j++) {


            echantillon[j]=data[k*D_i+j];

    }

    /*---------------------fft-------------------------------------------------------*/
    fft(echantillon,D_i/2,1);


    /*---------------------filtre----------------------------------------------------*/

    filtre_freq= 10010;
    filtre_large= 19980;

    echantillon = filtre(echantillon,filtre_freq, filtre_large, D_i);


    /*---------------------début recherche de la fondamentale ---------------------------*/


    float fo= fondamentale(echantillon,D_i,header.frequency, taille); 
    
    /*---------------------Touche associé sur le clavier  ---------------------------*/

    int n;
    n= gamme(fo);
    printf("la fondamentale vaut: %lf Hz \n", fo);
    printf("touche n° :%d \n",n);



    /*---------------------début coloration---------------------------*/

    if ((n>0) & (n<88)){

        coloration(n, window);
    }



}
    ///////////////////////fin de la boucle /////////////////////////////
    /*---------------------fermeture du piano----------------------------------*/

    
    SDL_DestroyWindow(window) ;
    SDL_Quit() ;


    /*---------------------liberation de la memoire----------------------------------*/


//    free(data);
 //   data = NULL;
    fclose(wav);


    free(echantillon);

    return 0;
}
