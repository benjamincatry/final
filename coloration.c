#include<SDL2/SDL.h>
#include<stdio.h>
#include<stdlib.h>

// focntion pour déterminer si une touche est blanche ou noir 
// 0 pour noir, 1 pour blanche 

int color_key ( int n) {
        int res=1 ; // touche blanche de base

        if ((n<1) || (n>88)) {
                printf ("touche hors clavier");
        }

        if ((n%12==10) || (n%12==0) || (n%12==2) || (n%12==5) || (n%12==7)) {
                res = 0 ;// touche noire
        }
        return res;
}

// fonction pour connaitre le nombre de touches blanches correspondant pour le coloriage 

int num_blanche ( int n ) {
        int res = 0;
        for (int k= 1;k<=n;k++) {
                if ((k%12==10) || (k%12==0) || (k%12==2) || (k%12==5) || (k%12==7)) {
                res = res -1;
                }
                res = res +1 ;

        }
        return res ;
};





int coloration (int n, SDL_Window *window) {


int c = color_key(n); // couleur de la touche
int b = num_blanche (n) ; // position sur le piano
//printf("la couleur de la touche à colorier est %d\n",c);
//printf("la position de la touche à colorier est %d\n",b);

    SDL_Renderer *renderer = NULL ; // Initialisation de l'outil de rendu


    renderer = SDL_CreateRenderer(window,-1,SDL_RENDERER_SOFTWARE) ;
if (renderer==NULL) {
        SDL_Log(" ERREUR : renderer SDL > %s\n ", SDL_GetError()) ;
        exit(EXIT_FAILURE) ; // si l'outil est mal initialisé on quitte le programme
    }

SDL_SetRenderDrawColor(renderer,255,255,255,SDL_ALPHA_OPAQUE); // l'outil de rendu dessinera en (R,V,B,transparence) donc ici en blanc opaque
        

    SDL_Rect rectangle; // definiton d'un rectangle sdl ( qui est une structure )
    rectangle.x= 0; //point de départ du rectangle
    rectangle.y= 0;
    rectangle.w = 1716; // largeur
    rectangle.h = 225;  // hauteur
SDL_RenderFillRect(renderer, &rectangle); // permet de colorier en la couleur de l'outil de rendu l'entièreté du rectangle défini précedemment

SDL_SetRenderDrawColor(renderer,128,128,128,SDL_ALPHA_OPAQUE);

    
for ( int i=1 ; i<=52;i++) {
    SDL_RenderDrawLine(renderer,i*33,0,i*33,300); // 33 taille des touches
    }



// Coloriage de la touche blanche si elle est enregistré au piano ( on la colorie avant les touches noires pour profiter au mieux de l'usage des rectangles )
if (c==1) {

        SDL_SetRenderDrawColor(renderer,255,192,203,SDL_ALPHA_OPAQUE); // 255, 192, 203 coefficients RVB du rose.

        rectangle.x=b*33; 
        rectangle.y= 0;
        rectangle.w = 33;
        rectangle.h = 225;

        SDL_RenderFillRect(renderer, &rectangle);

}
        

SDL_SetRenderDrawColor(renderer,0,0,0,SDL_ALPHA_OPAQUE);

for (int k = 1; k<52;k++){
    if (( k%7 !=2) & ( k%7 !=5)) {
        rectangle.x=k*33-7  ; // 7 taille des touche noir
        rectangle.y= 0;
        rectangle.w = 14;
        rectangle.h = 150;

SDL_RenderFillRect(renderer, &rectangle);

    }
}

// Coloriage de la touche noir si elle est enregistré au piano
if (c==0) {


SDL_SetRenderDrawColor(renderer,255,192,203,SDL_ALPHA_OPAQUE); // 255, 192, 203 coefficianst RGB du rose.

        rectangle.x=b*33-7; 
        rectangle.y= 0;
        rectangle.w = 14;
        rectangle.h = 150;

SDL_RenderFillRect(renderer, &rectangle);

}

    
    SDL_Delay(1000) ;
SDL_RenderPresent(renderer) ;
    SDL_RenderClear(renderer) ; //fin de l'utilisation de l'outil de rendu
if (SDL_RenderClear(renderer) !=0 ) {
        SDL_Log(" ERREUR : renderclear SDL > %s\n ", SDL_GetError()) ;
        exit(EXIT_FAILURE) ; //on quitte le programmme si l'action n'a pas été faite correctement
    }



    SDL_DestroyRenderer(renderer) ;

    return EXIT_SUCCESS ;

}
