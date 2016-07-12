#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "PixelNode.h"
FILE *fio,*fir;

// 该程序输入一幅二值图像，并选定一个面积阈值threshold.
// 1. 将二值图像按照8领域连通标记成不同的连通域
// 2. 如果某连通域面积大于面积阈值threshold的话，就置为白色，否则当作噪声去除

int main()
{
    // System Parameter
    unsigned char chi;
    long int *io,*ir;
    long int i,j,g,m,l,dimx,ff,dimy,n,taille,maxr,maxv,maxb;
    long int nbg;
    char entree[35],entree2[35],sortie[35],chaine[10];

    // Algorithm Parameter
    long int *itmp,*itmp2,*itmp3, *ioLabel;;
    int label = 1;
    int threshold;

    printf("Debruiter \n");
    printf("This program is going to eliminate the noise. \n");
    printf("Please give the input file name without .pgm\n");
    scanf("%s",entree);
    strcat(entree,".pgm");

    printf("Please give the output file name without .pgm\n");
    scanf("%s",sortie);
    strcat(sortie,".pgm");

    printf("Please give the surface threshold for eliminate noise.\n");
    scanf("%d",&threshold);

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
    ioLabel=(int *)malloc(dimx*dimy*sizeof(int));

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

    // 初始化标记图像(ioLabel)为0
    for(i=0;i<dimy;i++)
     for(j=0;j<dimx;j++)
     {
         ioLabel[i*dimx+j]=0;
     }

    // 如果某一个点为白色(目标点),而且他的八邻点都是没有标记过的点时，将其标记为新的标记 label，然后 label标记 +1 更新
    // 如果某一个点为白色(目标点),但是他的八邻点之中有标记过的点时，将其标记为八邻点之中标记值最小的标记
    // 函数 minNeighborsLabel(ioLabel,dimx,i,j) 实现寻找八邻点之间最小的标记值
    // label为第一次遍历图像之中图中所有标记个数，同时也是图中现在所有灰度值不为零的像素的最大个数
    for(i=1;i<dimy-1;i++)
     for(j=1;j<dimx-1;j++)
     {
         if(io[i*dimx+j]==255)
         {
             if( (ioLabel[i*dimx+j-1]==0)&&(ioLabel[i*dimx+j+1]==0)&&(ioLabel[(i-1)*dimx+j-1]==0)&&(ioLabel[(i-1)*dimx+j]==0)&&(ioLabel[(i-1)*dimx+j+1]==0)&&(ioLabel[(i+1)*dimx+j-1]==0)&&(ioLabel[(i+1)*dimx+j]==0)&&(ioLabel[(i+1)*dimx+j+1]==0) )
             {
                 ioLabel[i*dimx+j]=label;
                 label+=1;
             }
             else
             {
                 ioLabel[i*dimx+j]=minNeighborsLabel(ioLabel,dimx,i,j);
             }
         }
     }

    // 然后多次遍历图像点，如果一个标记点的八邻点之中有更小的标记，则换为更小的标记
    // 由于算法实现问题，我这里多做几次保证效果
    // 有待改进
     int loopTime = 10;  // 多做几次，保证合并label
     {
         for(i=1;i<dimy-1;i++)
         for(j=1;j<dimx-1;j++)
         {
             if(ioLabel[i*dimx+j]!=0)
             {
                 ioLabel[i*dimx+j]=minNeighborsLabel(ioLabel,dimx,i,j);
             }
         }
     }
    // label 加一，考虑到还有灰度值0，方便计算
    label=label+1;

    // 创建一个数组how_many_points_in_each_label，数组内存放每个 label 有多少个像素值 (类似于直方图，也就是每个连通域面积)
    int how_many_points_in_each_label[label];
    int labelPtr;

    for(labelPtr=0;labelPtr<label;labelPtr++)
    {
        how_many_points_in_each_label[labelPtr]=0;
    }

    long int tmpValue;

    for(i=0;i<dimy;i++)
     for(j=0;j<dimx;j++)
     {
         tmpValue=ioLabel[i*dimx+j];
         how_many_points_in_each_label[tmpValue]++;
     }

    // 显示每个how_many_points_in_each_label单位里有多少个像素，how_many_points_in_each_label引索值为灰度值，可以和直方图配合检验
    for(labelPtr=0;labelPtr<label;labelPtr++)
    {
        printf("#%d:    %d\n",labelPtr,how_many_points_in_each_label[labelPtr]);
    }

    // 遍历图像之中每一个点
    // 在每个点的基础上进行遍历how_many_points_in_each_label的每个引索值
    // 查看 how_many_points_in_each_label 每个 label 包含的像素数目是否大于输入的面积阈值 threshold
    // 如果是，则看该点像素值和该引索值是否一致，如果一致，则将其置为白色
    for(i=0;i<dimy;i++)
     for(j=0;j<dimx;j++)
     {
         for(labelPtr=1;labelPtr<label;labelPtr++)
         {
             if( how_many_points_in_each_label[labelPtr] > threshold )
             {
                 if (ioLabel[i*dimx+j]==labelPtr)
                 {
                     ioLabel[i*dimx+j]=255;
                 }
             }
         }
     }

    ////////////////////////////////////////////

    for(i=0;i<dimy;i++)
     for(j=0;j<dimx;j++)
     {
         fputc((unsigned char)ioLabel[i*dimx+j],fir);   // Êä³ö //
     }

    free(io); //...

    fclose(fio);fclose(fir);
    return 0;
}

// 该函数输入图像像素数组地址，横轴尺寸，和对应横纵坐标来确定像素位置
// 寻找
int minNeighborsLabel (int *ioLabel,int dimx, int i,int j)
{
    int loopX,loopY;  // 用于8领域遍历用的循环变量

    int jump=0;       // 用于控制跳出

    int min;

    // 用于寻找8领域中 label 不为0 的一个点，作为初始值
    for (loopY = -1; loopY <= 1; loopY++)
    {
        for (loopX = -1; loopX <= 1; loopX++)
        {
            if( ioLabel[(i+loopY)*dimx+j+loopX]!=0 )
                {
                min=ioLabel[(i+loopY)*dimx+j+loopX];
                jump=1;
                break;
                }
        }
        if(jump==1)
        {
            break;
        }
    }

    // 用初始值比较8邻点大小，选择小的一个为min，继续寻找，直到8邻点都比较结束
    for (loopY = -1; loopY <= 1; loopY++)
    for (loopX = -1; loopX <= 1; loopX++)
    {
        if( (ioLabel[(i+loopY)*dimx+j+loopX]!=0)&&(ioLabel[(i+loopY)*dimx+j+loopX]<min) )
        {
            min=ioLabel[(i+loopY)*dimx+j+loopX];
        }
    }
    //返回 min
    return min;
}
