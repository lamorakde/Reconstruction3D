#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
FILE *fio,*fir;


int main()
{
    // System Parameter
    unsigned char chi;
    long int *io,*ir;
    long int i,j,g,m,l,dimx,ff,dimy,n,taille,maxr,maxv,maxb;
    long int nbg;
    char entree[35],entree2[35],sortie[35],chaine[10];

    // Algorithm Parameter
    long int *itmp,*itmp2,*itmp3;
    int curLoop;
    int loop=25;  // 程序循环次数，次数越多，效果越好( 达到最好效果后不会发生任何变化 )


printf("Program Skeletons \n");
printf("This program is going to extract the images target's skeleton.\nMAKE SURE the TARGET color is WHITE and the background is black ! \n");
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
ir=(int *)malloc(dimx*dimy*sizeof(int));
itmp=(int *)malloc(dimx*dimy*sizeof(int));
itmp2=(int *)malloc(dimx*dimy*sizeof(int));
itmp3=(int *)malloc(dimx*dimy*sizeof(int));

if(ir==NULL) printf("allocation iob impossible\n");


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

 /////////////   用于查表法的表   ////////////////////////////////////////////////////
int array [256]= {0,0,1,1,0,0,1,1,1,1,0,1,1,1,0,1,
         1,1,0,0,1,1,1,1,0,0,0,0,0,0,0,1,
         0,0,1,1,0,0,1,1,1,1,0,1,1,1,0,1,
         1,1,0,0,1,1,1,1,0,0,0,0,0,0,0,1,
         1,1,0,0,1,1,0,0,0,0,0,0,0,0,0,0,
         0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
         1,1,0,0,1,1,0,0,1,1,0,1,1,1,0,1,
         0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
         0,0,1,1,0,0,1,1,1,1,0,1,1,1,0,1,
         1,1,0,0,1,1,1,1,0,0,0,0,0,0,0,1,
         0,0,1,1,0,0,1,1,1,1,0,1,1,1,0,1,
         1,1,0,0,1,1,1,1,0,0,0,0,0,0,0,0,
         1,1,0,0,1,1,0,0,0,0,0,0,0,0,0,0,
         1,1,0,0,1,1,1,1,0,0,0,0,0,0,0,0,
         1,1,0,0,1,1,0,0,1,1,0,1,1,1,0,0,
         1,1,0,0,1,1,1,0,1,1,0,0,1,0,0,0};

// 该算法是用于黑色像素，所以我先翻转图像进行抽取骨架操作，最后再转换回去
for(i=0;i<dimy;i++)
 for(j=0;j<dimx;j++)
 {
    itmp[i*dimx+j]=255-io[i*dimx+j];    // 翻转黑白
 }


int pointValue;
int next=1;

for (curLoop=0;curLoop<loop;curLoop++)
{
    //////////////  Program #1 Starts : 从此开始这段函数将使图像左右两边变小最多两个像素  /////////////////////////////
    for(i=1;i<dimy-1;i++)
     for(j=1;j<dimx-1;j++)
     {
         if(itmp[i*dimx+j]+itmp[i*dimx+j-1]+itmp[i*dimx+j+1]==0)
         {
             itmp2[i*dimx+j]=itmp[i*dimx+j];
             continue;
         }
         else if (next==0)
         {
             next=1;
             itmp2[i*dimx+j]=itmp[i*dimx+j];
             continue;
         }
         else if (itmp[i*dimx+j]==0)
         {
            pointValue=0;

            //  一行
            if(itmp[(i-1)*dimx+j-1]==255)
            {
                pointValue+=1;
            };
            if(itmp[(i-1)*dimx+j]==255)
            {
                pointValue+=2;
            };
             if(itmp[(i-1)*dimx+j+1]==255)
             {
                 pointValue+=4;
             };

             // 二行
             if(itmp[i*dimx+j-1]==255)
             {
                 pointValue+=8;
             };
             if(itmp[i*dimx+j]==255)
             {
                 pointValue+=0;
             };
             if(itmp[i*dimx+j+1]==255)
             {
                 pointValue+=16;
             };

             // 三行
             if(itmp[(i+1)*dimx+j-1]==255)
             {
                 pointValue+=32;
             };
             if(itmp[(i+1)*dimx+j]==255)
             {
                 pointValue+=64;
             };
             if(itmp[(i+1)*dimx+j+1]==255)
             {
                 pointValue+=128;
             };
             itmp2[i*dimx+j]=255*array[pointValue];

             if(itmp2[i*dimx+j]==255)
             {
                 next=0;
             }
         }
         else itmp2[i*dimx+j]=itmp[i*dimx+j];
     }

     // 将最上最下两行变白
     for(j=0;j<dimx;j++)
    {
        itmp2[j] = 255;
        itmp2[j+(dimy-1)*dimx] = 255;
    }
    // 将最左最右两列变白
    for(i=0;i<dimy;i++)
    {
        itmp2[i*dimx] = 255;
        itmp2[(i+1)*dimx-1] = 255;
    }

    for(i=0;i<dimy;i++)
     for(j=0;j<dimx;j++)
     {
         itmp[i*dimx+j]=itmp2[i*dimx+j];
     }
     //////////////  Program #1 Finishes  /////////////////////////////


     //////////////  Program #2 Starts : 从此开始这段函数将使图像上下两边变小最多两个像素  /////////////////////////////


     for(j=1;j<dimx-1;j++)   // j 为横轴方向运动
        for(i=1;i<dimy-1;i++)  // i 为纵轴方向运动
        {
         if(itmp[i*dimx+j]+itmp[(i-1)*dimx+j]+itmp[(i+1)*dimx+j]==0)
         {
             itmp2[i*dimx+j]=itmp[i*dimx+j];
             continue;
         }
         else if (next==0)
         {
             next=1;
             itmp2[i*dimx+j]=itmp[i*dimx+j];
             continue;
         }
         else if (itmp[i*dimx+j]==0)
         {
            pointValue=0;

            //  一行
            if(itmp[(i-1)*dimx+j-1]==255)
            {
                pointValue+=1;
            };
            if(itmp[(i-1)*dimx+j]==255)
            {
                pointValue+=2;
            };
             if(itmp[(i-1)*dimx+j+1]==255)
             {
                 pointValue+=4;
             };

             // 二行
             if(itmp[i*dimx+j-1]==255)
             {
                 pointValue+=8;
             };
             if(itmp[i*dimx+j]==255)
             {
                 pointValue+=0;
             };
             if(itmp[i*dimx+j+1]==255)
             {
                 pointValue+=16;
             };

             // 三行
             if(itmp[(i+1)*dimx+j-1]==255)
             {
                 pointValue+=32;
             };
             if(itmp[(i+1)*dimx+j]==255)
             {
                 pointValue+=64;
             };
             if(itmp[(i+1)*dimx+j+1]==255)
             {
                 pointValue+=128;
             };
             itmp2[i*dimx+j]=255*array[pointValue];

             if(itmp2[i*dimx+j]==255)
             {
                 next=0;
             }
         }
         else itmp2[i*dimx+j]=itmp[i*dimx+j];
        }

     // 将最上最下两行变白
     for(j=0;j<dimx;j++)
    {
        itmp2[j] = 255;
        itmp2[j+(dimy-1)*dimx] = 255;
    }
    // 将最左最右两列变白
    for(i=0;i<dimy;i++)
    {
        itmp2[i*dimx] = 255;
        itmp2[(i+1)*dimx-1] = 255;
    }

    for(i=0;i<dimy;i++)
     for(j=0;j<dimx;j++)
     {
         itmp[i*dimx+j]=itmp2[i*dimx+j];
     }

     //////////////////////////////// Program #2 Finishes  /////////////////////////////////////////
}

// This program is purposed to fix the connect point, NOT sure the quality
for(i=1;i<dimy-1;i++)
 for(j=1;j<dimx-1;j++)
 {
     if( ( (itmp[i*dimx+j-1]==0)&&(itmp[i*dimx+j+1]==0)&&(itmp[(i-1)*dimx+j]==0) )||( (itmp[i*dimx+j-1]==0)&&(itmp[i*dimx+j+1]==0)&&(itmp[(i+1)*dimx+j]==0) )|| ( (itmp[(i+1)*dimx+j]==0)&&(itmp[(i-1)*dimx+j]==0)&&(itmp[i*dimx+j-1]==0) )||( (itmp[(i+1)*dimx+j]==0)&&(itmp[(i-1)*dimx+j]==0)&&(itmp[i*dimx+j+1]==0) ) )
     {
         itmp[i*dimx+j]=0;
     }
 }

// To inverse the color again, 再次翻转我们得到的二值图像
for(i=0;i<dimy;i++)
 for(j=0;j<dimx;j++)
 {
     ir[i*dimx+j]=255-itmp[i*dimx+j];
 }

////////////////////////////////////////////

for(i=0;i<dimy;i++)
 for(j=0;j<dimx;j++)
 {
	 fputc((unsigned char)ir[i*dimx+j],fir);   // Êä³ö //
 }

free(io); //...

fclose(fio);fclose(fir);
return 0;
}
