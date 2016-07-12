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
    char entree[35],sortie[35],chaine[10],filename[35];
    int loop,curLoop;
    // Closing Algorithm parameter
    int x,y;
    int tp;

printf("##########################################################################################################\n");
printf("\t\tProgram information:\n\n");
printf("\t\t�ó�������һ�� PGM ͼ�񣬳�����������ڳ�ȡ�Ǽܣ��������Ǽ�����ֵ \n\n");
printf("##########################################################################################################\n\n");
printf("Please give the input file name without .pgm\n");
scanf("%s",entree);
strcpy(filename,entree);
strcat(entree,".pgm");
strcat(filename,".txt");
// #1 ��ʼ: ��ȡͼ����Ϣ������ʼ������ io[]
fio=fopen(entree,"rb");

if(fio !=NULL) printf("ouverture du fichier in\n");

//if(im!=0) printf("allocation de im\n");

fscanf(fio,"%s\n",chaine);

i=0;
while(i<1)
{chi=fgetc(fio);
// if((int)chi==6)  fputc((unsigned char)5,fr);
 fputc(chi,fir);
 if(chi=='\n') i++;
}

fscanf(fio,"%ld %ld\n",&dimx,&dimy);

fscanf(fio,"%d\n",&nbg);

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
// #1 ����





 // # 2:��ʼ��Ϊ����ͼ�����������㣬�������������� io[]���Ա���н�����������
 //////////////////////////////////////////////////////////////////////////////////
 //////////////////////////  Algorithm Begins   ///////////////////////////////////

////////////////////////////////////////////
//  �����������㣬������ʴ���㣬Ϊ������  //
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


/////////////////////������������㣬�ѽ���ٴθ���io[]���Ա�������� skeleton ����/////////////////////////////////////////////////////////////////

for(i=0;i<dimy;i++)
 for(j=0;j<dimx;j++)
 {
     io[i*dimx+j]=itmp7[i*dimx+j];
 }
printf("�������㣺���\n");
////////////////////////////////////////////    Dilatation Finished    ///////////////////////////////////////
// # 2:���







// # 3����ʼ������������Ľ�����йǼܳ�ȡ
/////// Algorithm Skeletons Begins  //////////////////////////////////////////////////

 /////////////   ���ڲ���ı�   ////////////////////////////////////////////////////
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

// ���㷨�����ں�ɫ���أ��������ȷ�תͼ����г�ȡ�Ǽܲ����������ת����ȥ
for(i=0;i<dimy;i++)
 for(j=0;j<dimx;j++)
 {
    itmp[i*dimx+j]=255-io[i*dimx+j];    // ��ת�ڰ�
 }

int pointValue;
int next=1;
loop=25;
for (curLoop=0;curLoop<loop;curLoop++)
{
    //////////////  Program 1 Starts : �Ӵ˿�ʼ��κ�����ʹͼ���������߱�С�����������  /////////////////////////////
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

            //  һ��
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

             // ����
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

             // ����
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

     // �������������б��
     for(j=0;j<dimx;j++)
    {
        itmp2[j] = 255;
        itmp2[j+(dimy-1)*dimx] = 255;
    }
    // �������������б��
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
//////////////  Program 1 Finishes  /////////////////////////////

//////////////  Program 2 Starts : �Ӵ˿�ʼ��κ�����ʹͼ���������߱�С�����������  /////////////////////////////


     for(j=1;j<dimx-1;j++)   // j Ϊ���᷽���˶�
        for(i=1;i<dimy-1;i++)  // i Ϊ���᷽���˶�
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

            //  һ��
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

             // ����
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

             // ����
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

     // �������������б��
     for(j=0;j<dimx;j++)
    {
        itmp2[j] = 255;
        itmp2[j+(dimy-1)*dimx] = 255;
    }
    // �������������б��
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

/////////////////////////////// Program 2 Finishes  ///////////////////////
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

// To inverse the color again, �ٴη�ת���ǵõ��Ķ�ֵͼ��
// ͬʱҲ�ٴθ�ֵ�� io���Ա������������
for(i=0;i<dimy;i++)
 for(j=0;j<dimx;j++)
 {
     io[i*dimx+j]=255-itmp[i*dimx+j];
 }

printf("�Ǽܳ�ȡ�����\n");
///////////////////////////////////////////////////////////////
// # 3:���








// # 4:����õ��ĹǼ�����ֵ
///////////////////  ��ʼ����Ǽ�����  ////////////////////////////////////

 //////////////////////// Algorithm Begins  ///////////////////////////////

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
long int *pixel_coordinate;

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

FILE *fp = fopen(filename, "w+");
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

/////////////////////////////////////////////////////////////////////////////////////

free(io);
free(fp);

fclose(fio);fclose(fp);
return 0;
}
