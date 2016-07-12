#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
FILE *fio,*fir,*fp;


int main()
{
    // System Parameter
    unsigned char chi;
    long int *io;
    long int i,j,g,m,l,dimx,ff,dimy,n,taille,maxr,maxv,maxb;
    long int nbg;
    char entree[35],entree2[35],sortie[35],chaine[10];

    // Algorithm Parameter
    long int *pixel_coordinate;



printf("Pixels coordinate \n");
printf("This program is going to print the white pixels's coordinates into a txt file \nwhich is named  coordinates of the red pixels.txt\n");
printf("Please give the input file name without .pgm\n");
scanf("%s",entree);
strcat(entree,".pgm");

fio=fopen(entree,"rb");
fir=fopen(sortie,"wb+");

if(fio !=NULL) printf("ouverture du fichier in\n");
if(fir !=NULL) printf("ouverture du fichier out\n");

fscanf(fio,"%s\n",chaine);
fprintf(fir,"%s\n",chaine);


i=0;
while(i<1)
{chi=fgetc(fio);
 fputc(chi,fir);
 if(chi=='\n') i++;
}

fscanf(fio,"%ld %ld\n",&dimx,&dimy);
fprintf(fir,"%ld %ld\n",dimx,dimy);


fscanf(fio,"%d\n",&nbg);
fprintf(fir,"%d\n",nbg);

printf("dimx=%d dimy=%d nbg=%d\n",dimx,dimy,nbg);

io=(int *)malloc(dimx*dimy*sizeof(int));



taille=dimx*dimy;
printf("Taille=%d\n",taille);
for(i=0;i<dimy;i++)
 for(j=0;j<dimx;j++)
 {
  chi=(unsigned char)fgetc(fio);
  io[i*dimx+j]=(int)chi;
 }
 /////////////////////////////////////////////////////////////////////////////////////

 //////////////////////// Algorithm Begins  //////////////////////////////////////////

int white_pixel=0;

for(i=0;i<dimy;i++)
 for(j=0;j<dimx;j++)
 {
    if(io[i*dimx+j]==255)
    {
        ++white_pixel;
    }
 }

printf("There are %d white pixels.\n",white_pixel);
long white_pixel_coordinate_number=white_pixel*2;
pixel_coordinate=(int *)malloc(sizeof(int)*white_pixel_coordinate_number);

int ptr=0;

for(i=0;i<dimy;i++)
 for(j=0;j<dimx;j++)
 {
    if(io[i*dimx+j]==255)
    {
        pixel_coordinate[ptr]=j;
        ++ptr;
        pixel_coordinate[ptr]=i;
        ++ptr;
    }
 }

////////////////////////////////////////////

FILE *fp = fopen("coordinates of the red pixels.txt", "w+");
int enter=1;

for(i=0;i<white_pixel_coordinate_number;i++)
{
    fprintf(fp,"%d",pixel_coordinate[i]);
    ++enter;
    if((enter%2)==0)
    {
        fprintf(fp,",");
    };

    if((enter%2)==1)
    {
        fprintf(fp,"\n");
    }
}

free(io);
free(fp);

fclose(fio);fclose(fp);
return 0;
}
