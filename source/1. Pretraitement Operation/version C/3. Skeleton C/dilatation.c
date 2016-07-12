#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
FILE *fio,*fir;


int main()
{
    // System parameter
    long int *io,*ir,*itmp,*itmp2,*itmp3,*itmp4,*itmp5,*itmp6,*itmp7;
    long int i,j,ic,jc,m,l,dimx,ff,dimy,n,taille,maxr,maxv,maxb;
    unsigned char chi;
    long int nbg;
    char entree[35],sortie[35],chaine[10];
    int loop,curLoop;
    // Closing Algorithm parameter
    int x,y;
    int tp;



printf("This program is going to do dilatation operation. \n");
printf("Please give the input file name without .pgm\n");
scanf("%s",entree);
strcat(entree,".pgm");

printf("Please give the output file name without .pgm\n");
scanf("%s",sortie);
strcat(sortie,".pgm");

fio=fopen(entree,"rb");
fir=fopen(sortie,"wb+");

if(fio !=NULL) printf("ouverture du fichier in\n");
if(fir !=NULL) printf("ouverture du fichier out\n");

//if(im!=0) printf("allocation de im\n");

fscanf(fio,"%s\n",chaine);
fprintf(fir,"%s\n",chaine);


i=0;
while(i<1)
{chi=fgetc(fio);
// if((int)chi==6)  fputc((unsigned char)5,fr);
 fputc(chi,fir);
 if(chi=='\n') i++;
}

fscanf(fio,"%ld %ld\n",&dimx,&dimy);
fprintf(fir,"%ld %ld\n",dimx,dimy);


fscanf(fio,"%d\n",&nbg);
fprintf(fir,"%d\n",nbg);

printf("dimx=%d dimy=%d nbg=%d\n",dimx,dimy,nbg);

io=(int *)malloc(dimx*dimy*sizeof(int));
itmp=(int *)malloc(dimx*dimy*sizeof(int));
itmp2=(int *)malloc(dimx*dimy*sizeof(int));
itmp3=(int *)malloc(dimx*dimy*sizeof(int));
itmp4=(int *)malloc(dimx*dimy*sizeof(int));
itmp5=(int *)malloc(dimx*dimy*sizeof(int));
itmp6=(int *)malloc(dimx*dimy*sizeof(int));
itmp7=(int *)malloc(dimx*dimy*sizeof(int));
ir=(int *)malloc(dimx*dimy*sizeof(int));


if(ir==NULL) printf("allocation iob impossible\n");

taille=dimx*dimy;
printf("taille=%d\n",taille);
for(i=0;i<dimy;i++)
 for(j=0;j<dimx;j++)
 {
  chi=(unsigned char)fgetc(fio);
  io[i*dimx+j]=(int)chi;
 }

 printf("he");
 //////////////////////////////////////////////////////////////////////////////////
 //////////////////////////  Algorithm Begins   ///////////////////////////////////

////////////////////////////////////////////
//  先做膨胀运算，在做腐蚀运算，为闭运算  //
///////  Closing for remove the noise //////

// First dilatation
for(i=1;i<dimy-1;i++)
  for(j=1;j<dimx-1;j++)
{
    if(io[i*dimx+j]==255)
    {
        itmp[(i-1)*dimx+j-1]=255;
        itmp[(i-1)*dimx+j]=255;
        itmp[(i-1)*dimx+j+1]=255;
        itmp[i*dimx+j-1]=255;
        itmp[i*dimx+j]=255;          // point central
        itmp[i*dimx+j+1]=255;
        itmp[(i+1)*dimx+j-1]=255;
        itmp[(i+1)*dimx+j]=255;
        itmp[(i+1)*dimx+j+1]=255;
    }
}

// Second dilatation
for(i=1;i<dimy-1;i++)
  for(j=1;j<dimx-1;j++)
{
    if(itmp[i*dimx+j]==255)
    {
        itmp2[(i-1)*dimx+j-1]=255;
        itmp2[(i-1)*dimx+j]=255;
        itmp2[(i-1)*dimx+j+1]=255;
        itmp2[i*dimx+j-1]=255;
        itmp2[i*dimx+j]=255;          // point central
        itmp2[i*dimx+j+1]=255;
        itmp2[(i+1)*dimx+j-1]=255;
        itmp2[(i+1)*dimx+j]=255;
        itmp2[(i+1)*dimx+j+1]=255;
    }
}

// Third Dilatation
for(i=1;i<dimy-1;i++)
  for(j=1;j<dimx-1;j++)
{
    if(itmp2[i*dimx+j]==255)
    {
        itmp3[(i-1)*dimx+j-1]=255;
        itmp3[(i-1)*dimx+j]=255;
        itmp3[(i-1)*dimx+j+1]=255;
        itmp3[i*dimx+j-1]=255;
        itmp3[i*dimx+j]=255;          // point central
        itmp3[i*dimx+j+1]=255;
        itmp3[(i+1)*dimx+j-1]=255;
        itmp3[(i+1)*dimx+j]=255;
        itmp3[(i+1)*dimx+j+1]=255;
    }
}

// Fourth Dilatation
for(i=1;i<dimy-1;i++)
  for(j=1;j<dimx-1;j++)
{
    if(itmp3[i*dimx+j]==255)
    {
        itmp4[(i-1)*dimx+j-1]=255;
        itmp4[(i-1)*dimx+j]=255;
        itmp4[(i-1)*dimx+j+1]=255;
        itmp4[i*dimx+j-1]=255;
        itmp4[i*dimx+j]=255;          // point central
        itmp4[i*dimx+j+1]=255;
        itmp4[(i+1)*dimx+j-1]=255;
        itmp4[(i+1)*dimx+j]=255;
        itmp4[(i+1)*dimx+j+1]=255;
    }
}

// Fourth Dilatation
for(i=1;i<dimy-1;i++)
  for(j=1;j<dimx-1;j++)
{
    if(itmp4[i*dimx+j]==255)
    {
        itmp5[(i-1)*dimx+j-1]=255;
        itmp5[(i-1)*dimx+j]=255;
        itmp5[(i-1)*dimx+j+1]=255;
        itmp5[i*dimx+j-1]=255;
        itmp5[i*dimx+j]=255;          // point central
        itmp5[i*dimx+j+1]=255;
        itmp5[(i+1)*dimx+j-1]=255;
        itmp5[(i+1)*dimx+j]=255;
        itmp5[(i+1)*dimx+j+1]=255;
    }
}

// Fourth Dilatation
for(i=1;i<dimy-1;i++)
  for(j=1;j<dimx-1;j++)
{
    if(itmp5[i*dimx+j]==255)
    {
        itmp6[(i-1)*dimx+j-1]=255;
        itmp6[(i-1)*dimx+j]=255;
        itmp6[(i-1)*dimx+j+1]=255;
        itmp6[i*dimx+j-1]=255;
        itmp6[i*dimx+j]=255;          // point central
        itmp6[i*dimx+j+1]=255;
        itmp6[(i+1)*dimx+j-1]=255;
        itmp6[(i+1)*dimx+j]=255;
        itmp6[(i+1)*dimx+j+1]=255;
    }
}

// Fourth Dilatation
for(i=1;i<dimy-1;i++)
  for(j=1;j<dimx-1;j++)
{
    if(itmp6[i*dimx+j]==255)
    {
        itmp7[(i-1)*dimx+j-1]=255;
        itmp7[(i-1)*dimx+j]=255;
        itmp7[(i-1)*dimx+j+1]=255;
        itmp7[i*dimx+j-1]=255;
        itmp7[i*dimx+j]=255;          // point central
        itmp7[i*dimx+j+1]=255;
        itmp7[(i+1)*dimx+j-1]=255;
        itmp7[(i+1)*dimx+j]=255;
        itmp7[(i+1)*dimx+j+1]=255;
    }
}

////////////////////////////////////////////    Dilatation Finished    ///////////////////////////////////////


//////////////////////////////////////////// Algorithm Skeletons Begins  /////////////////////////////////////



////////////////////////////////////////////

for(i=0;i<dimy;i++)
 for(j=0;j<dimx;j++)
 {
     ir[i*dimx+j]=itmp7[i*dimx+j];
	 fputc((unsigned char)ir[i*dimx+j],fir);   // Êä³ö //
 }


free(io); //...

fclose(fio);fclose(fir);
return 0;
}
