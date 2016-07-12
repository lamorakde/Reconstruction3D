#include "opencv2/opencv.hpp"
using namespace cv;
using namespace std;

void CZH_inverseBinary(Mat & src_bw_Image, Mat & dst_bw_Image)
{
	// ʹ�� LUT ��תͼ���кڰ�����
	Mat tmpImage;
	tmpImage.create(src_bw_Image.size(), src_bw_Image.type());
	tmpImage = src_bw_Image;
	uchar inverseBinaryTable[256];	// ��ʼ����ת��
	inverseBinaryTable[0] = 255;	// ��ʼ����ת��
	inverseBinaryTable[255] = 0;	// ��ʼ����ת��

	Mat lookUpTable(1, 256, CV_8U);	// ���� LUT ���ҵı�
	uchar* p = lookUpTable.data;
	for (int i = 0; i < 256; ++i)
	{
		p[i] = inverseBinaryTable[i];
	}
	LUT(tmpImage, lookUpTable, dst_bw_Image);
}

void CZH_endPoint(Mat & src_skeleton_Image, Mat & dst_endPoint_Image)
{
	// �ó�������һ��Ŀ��Ԫ��Ϊ��ɫ���صĵ� Mat �����ͼ��
	// ���򽫻�Ѱ�Ҹù���ͼ��Ķ˵�
	// ��������ͼ���ɫ����ռ����������޸ĵڶ�������ͼ��
	Mat src_image = src_skeleton_Image;
	Mat dst_image(src_image.size(), src_image.type(), Scalar(0));

	// �����ɫ��ɫ���ظ���
	Mat_<uchar>::iterator itBegin = src_image.begin<uchar>();
	Mat_<uchar>::iterator itEnd = src_image.end<uchar>();
	int blackPoints = 0;
	int whitePoints = 0;
	for (; itBegin != itEnd; itBegin++)
	{
		if (*itBegin == 0)
			blackPoints++;
		else whitePoints++;
	}

	if (whitePoints < blackPoints)
	{
		for (int i = 1; i < src_image.rows - 1; i++)
		{
			for (int j = 1; j < src_image.cols - 1; j++)
			{
				if (src_image.at<uchar>(i, j) == 255)
				{
					int sumWhite = 0;
					for (int ii = -1; ii <= 1; ii++)
					{
						for (int jj = -1; jj <= 1; jj++)
						{
							if (src_image.at<uchar>(i + ii, j + jj) == 255)
							{
								sumWhite++;
							}
						}
					}
					if (sumWhite == 2)
					{
						dst_image.at<uchar>(i, j) = 255;
					}
				}
			}
		}
		dst_endPoint_Image = dst_image;
	}
	else cerr << "�ú��������ɫ�����ṹ�Ķ˵㣬����ͼ���ǰ�ɫ���ع���ͼ��\n";
}

// Mat CZH_skeleton(Mat src_bw_image)
// {
// 	// �ó�������һ�� Mat ���͵Ķ�ֵͼ�����һ�� Mat ���͵Ķ�ֵ�Ǽ�ͼ��
// 	// �ó�����ȡ��ɫĿ��Ԫ�صĹǼܣ�
// 	// ���ڲ���ı�
// 	int array[256] = { 0,0,1,1,0,0,1,1,1,1,0,1,1,1,0,1,
// 						1,1,0,0,1,1,1,1,0,0,0,0,0,0,0,1,
// 						0,0,1,1,0,0,1,1,1,1,0,1,1,1,0,1,
// 						1,1,0,0,1,1,1,1,0,0,0,0,0,0,0,1,
// 						1,1,0,0,1,1,0,0,0,0,0,0,0,0,0,0,
// 						0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
// 						1,1,0,0,1,1,0,0,1,1,0,1,1,1,0,1,
// 						0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
// 						0,0,1,1,0,0,1,1,1,1,0,1,1,1,0,1,
// 						1,1,0,0,1,1,1,1,0,0,0,0,0,0,0,1,
// 						0,0,1,1,0,0,1,1,1,1,0,1,1,1,0,1,
// 						1,1,0,0,1,1,1,1,0,0,0,0,0,0,0,0,
// 						1,1,0,0,1,1,0,0,0,0,0,0,0,0,0,0,
// 						1,1,0,0,1,1,1,1,0,0,0,0,0,0,0,0,
// 						1,1,0,0,1,1,0,0,1,1,0,1,1,1,0,0,
// 						1,1,0,0,1,1,1,0,1,1,0,0,1,0,0,0 };
// 	Mat tmpImage, dstImage;
// 	tmpImage.create(src_bw_image.size(), src_bw_image.type());
// 	dstImage.create(src_bw_image.size(), src_bw_image.type());
// 	int pointValue;
// 	int next = 1;
// 	int loop = 25;  // ����ѭ������������Խ�࣬Ч��Խ��( �ﵽ���Ч���󲻻ᷢ���κα仯 )
// 	for (int curLoop = 0; curLoop < loop; curLoop++)
// 	{
// 		//////////////  Program #1 Starts : �Ӵ˿�ʼ��κ�����ʹͼ���������߱�С�����������  /////////////////////////////
// 		for (int i = 1; i < src_bw_image.rows - 1; i++)
// 		{
// 			for (int j = 1; j < src_bw_image.cols - 1; j++)
// 			{
// 				if (src_bw_image.at<uchar>(i, j) + src_bw_image.at<uchar>(i, j - 1) + src_bw_image.at<uchar>(i, j + 1) == 0)
// 				{
// 					tmpImage.at<uchar>(i, j) = src_bw_image.at<uchar>(i, j);
// 					continue;
// 				}
// 				else if (next == 0)
// 				{
// 					next = 1;
// 					tmpImage.at<uchar>(i, j) = src_bw_image.at<uchar>(i, j);
// 					continue;
// 				}
// 				else if (src_bw_image.at<uchar>(i, j) == 0)
// 				{
// 					pointValue = 0;
// 
// 					//  һ��
// 					if (src_bw_image.at<uchar>(i - 1, j - 1) == 255)
// 					{
// 						pointValue += 1;
// 					};
// 					if (src_bw_image.at<uchar>(i - 1, j) == 255)
// 					{
// 						pointValue += 2;
// 					};
// 					if (src_bw_image.at<uchar>(i - 1, j + 1) == 255)
// 					{
// 						pointValue += 4;
// 					};
// 
// 					// ����
// 					if (src_bw_image.at<uchar>(i, j - 1) == 255)
// 					{
// 						pointValue += 8;
// 					};
// 					if (src_bw_image.at<uchar>(i, j) == 255)
// 					{
// 						pointValue += 0;
// 					};
// 					if (src_bw_image.at<uchar>(i, j + 1) == 255)
// 					{
// 						pointValue += 16;
// 					};
// 
// 					// ����
// 					if (src_bw_image.at<uchar>(i + 1, j - 1) == 255)
// 					{
// 						pointValue += 32;
// 					};
// 					if (src_bw_image.at<uchar>(i + 1, j) == 255)
// 					{
// 						pointValue += 64;
// 					};
// 					if (src_bw_image.at<uchar>(i + 1, j + 1) == 255)
// 					{
// 						pointValue += 128;
// 					};
// 					tmpImage.at<uchar>(i, j) = 255 * array[pointValue];
// 
// 					if (tmpImage.at<uchar>(i, j) == 255)
// 					{
// 						next = 0;
// 					}
// 				}
// 				else tmpImage.at<uchar>(i, j) = src_bw_image.at<uchar>(i, j);
// 			}
// 		}
// 		// �������������б��
// 		for (int j = 0; j < src_bw_image.cols; j++)
// 		{
// 			tmpImage.at<uchar>(0, j) = 255;
// 			tmpImage.at<uchar>(src_bw_image.rows - 1, j) = 255;
// 		}
// 		// �������������б��
// 		for (int i = 0; i < src_bw_image.rows; i++)
// 		{
// 			tmpImage.at<uchar>(i, 0) = 255;
// 			tmpImage.at<uchar>(i, src_bw_image.cols - 1) = 255;
// 		}
// 
// 		//for (int i = 0; i < src_bw_image.rows; i++)
// 		//	for (int j = 0; j < src_bw_image.cols; j++)
// 		//	{
// 		//		itmp[i*dimx + j] = itmp2[i*dimx + j];
// 		//	}
// 
// 		//////////////  Program #2 Starts : �Ӵ˿�ʼ��κ�����ʹͼ���������߱�С�����������  /////////////////////////////
// 
// 		for (int j = 1; j < src_bw_image.cols - 1; j++)			// j Ϊ���᷽���˶�
// 		{
// 			for (int i = 1; i < src_bw_image.rows - 1; i++)		// i Ϊ���᷽���˶�
// 			{
// 				if (tmpImage.at<uchar>(i, j) + tmpImage.at<uchar>(i - 1, j) + tmpImage.at<uchar>(i + 1, j) == 0)
// 				{
// 					dstImage.at<uchar>(i, j) = tmpImage.at<uchar>(i, j);
// 					continue;
// 				}
// 				else if (next == 0)
// 				{
// 					next = 1;
// 					dstImage.at<uchar>(i, j) = tmpImage.at<uchar>(i, j);
// 					continue;
// 				}
// 				else if (tmpImage.at<uchar>(i, j) == 0)
// 				{
// 					pointValue = 0;
// 
// 					//  һ��
// 					if (tmpImage.at<uchar>(i - 1, j - 1) == 255)
// 					{
// 						pointValue += 1;
// 					};
// 					if (tmpImage.at<uchar>(i - 1, j) == 255)
// 					{
// 						pointValue += 2;
// 					};
// 					if (tmpImage.at<uchar>(i - 1, j + 1) == 255)
// 					{
// 						pointValue += 4;
// 					};
// 
// 					// ����
// 					if (tmpImage.at<uchar>(i, j - 1) == 255)
// 					{
// 						pointValue += 8;
// 					};
// 					if (tmpImage.at<uchar>(i, j) == 255)
// 					{
// 						pointValue += 0;
// 					};
// 					if (tmpImage.at<uchar>(i, j + 1) == 255)
// 					{
// 						pointValue += 16;
// 					};
// 
// 					// ����
// 					if (tmpImage.at<uchar>(i + 1, j - 1) == 255)
// 					{
// 						pointValue += 32;
// 					};
// 					if (tmpImage.at<uchar>(i + 1, j) == 255)
// 					{
// 						pointValue += 64;
// 					};
// 					if (tmpImage.at<uchar>(i + 1, j + 1) == 255)
// 					{
// 						pointValue += 128;
// 					};
// 					dstImage.at<uchar>(i, j) = 255 * array[pointValue];
// 
// 					if (dstImage.at<uchar>(i, j) == 255)
// 					{
// 						next = 0;
// 					}
// 				}
// 				else dstImage.at<uchar>(i, j) = tmpImage.at<uchar>(i, j);
// 			}
// 		}
// 		// �������������б��
// 		for (int j = 0; j < src_bw_image.cols; j++)
// 		{
// 			dstImage.at<uchar>(0, j) = 255;
// 			dstImage.at<uchar>(src_bw_image.rows - 1, j) = 255;
// 		}
// 		// �������������б��
// 		for (int i = 0; i < src_bw_image.rows; i++)
// 		{
// 			dstImage.at<uchar>(i, 0) = 255;
// 			dstImage.at<uchar>(i, src_bw_image.cols - 1) = 255;
// 		}
// 	}
// 	return dstImage;
// }