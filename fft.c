#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>


#define SWAP(a,b) tempr=(a);(a)=(b);(b)=tempr


void fft(float data[], unsigned long nn, int isign)
{
    unsigned long n,mmax,m,j,istep,i;
    double wtemp,wr,wpr,wpi,wi,theta;
    float tempr,tempi;
    n=nn << 1; // décalage d'un bit vers la droite en écriture binaire
    j=1;
    
    // boucle permettant de faire un symétrique entre les données afin de mieux les exploiter par la suite lors des sommations
    
    for (i=1; i<n; i+=2)
    {
        if (j > i)
        {
            SWAP(data[j],data[i]); // Les indices pairs sont les parties réelles et les indices impairs sont les parties imaginaires des données
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
        theta=isign*(6.28318530717959/mmax); // theta = pi puis pi/2 ... ( on remultiplie par 1/2 à chaque boucle )
        wtemp=sin(0.5*theta);
        wpr = -2.0*wtemp*wtemp; //Oméga réel ( pour faire la partie réelle dans la fft )
        wpi=sin(theta); //Oméga imaginaire ( pour faire la partie imaginaire de la fft )
        
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
