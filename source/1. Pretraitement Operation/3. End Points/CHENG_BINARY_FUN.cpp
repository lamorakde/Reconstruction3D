#include "opencv2/opencv.hpp"
using namespace cv;
using namespace std;

void CZH_inverseBinary(Mat & src_bw_Image, Mat & dst_bw_Image)
{
	// 使用 LUT 反转图像中黑白像素
	Mat tmpImage;
	tmpImage.create(src_bw_Image.size(), src_bw_Image.type());
	tmpImage = src_bw_Image;
	uchar inverseBinaryTable[256];	// 初始化反转表
	inverseBinaryTable[0] = 255;	// 初始化反转表
	inverseBinaryTable[255] = 0;	// 初始化反转表

	Mat lookUpTable(1, 256, CV_8U);	// 用于 LUT 查找的表
	uchar* p = lookUpTable.data;
	for (int i = 0; i < 256; ++i)
	{
		p[i] = inverseBinaryTable[i];
	}
	LUT(tmpImage, lookUpTable, dst_bw_Image);
}

void CZH_endPoint(Mat & src_skeleton_Image, Mat & dst_endPoint_Image)
{
	// 该程序输入一个目标元素为白色像素的的 Mat 类骨骼图像
	// 程序将会寻找该骨骼图像的端点
	// 如果输入的图像白色像素占大多数，则不修改第二个参数图像
	Mat src_image = src_skeleton_Image;
	Mat dst_image(src_image.size(), src_image.type(), Scalar(0));

	// 计算黑色白色像素个数
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
	else cerr << "该函数计算白色骨骼结构的端点，输入图像不是白色像素骨骼图像！\n";
}

// Mat CZH_skeleton(Mat src_bw_image)
// {
// 	// 该程序输入一个 Mat 类型的二值图像，输出一个 Mat 类型的二值骨架图像
// 	// 该程序会抽取黑色目标元素的骨架，
// 	// 用于查表法的表
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
// 	int loop = 25;  // 程序循环次数，次数越多，效果越好( 达到最好效果后不会发生任何变化 )
// 	for (int curLoop = 0; curLoop < loop; curLoop++)
// 	{
// 		//////////////  Program #1 Starts : 从此开始这段函数将使图像左右两边变小最多两个像素  /////////////////////////////
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
// 					//  一行
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
// 					// 二行
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
// 					// 三行
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
// 		// 将最上最下两行变白
// 		for (int j = 0; j < src_bw_image.cols; j++)
// 		{
// 			tmpImage.at<uchar>(0, j) = 255;
// 			tmpImage.at<uchar>(src_bw_image.rows - 1, j) = 255;
// 		}
// 		// 将最左最右两列变白
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
// 		//////////////  Program #2 Starts : 从此开始这段函数将使图像上下两边变小最多两个像素  /////////////////////////////
// 
// 		for (int j = 1; j < src_bw_image.cols - 1; j++)			// j 为横轴方向运动
// 		{
// 			for (int i = 1; i < src_bw_image.rows - 1; i++)		// i 为纵轴方向运动
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
// 					//  一行
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
// 					// 二行
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
// 					// 三行
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
// 		// 将最上最下两行变白
// 		for (int j = 0; j < src_bw_image.cols; j++)
// 		{
// 			dstImage.at<uchar>(0, j) = 255;
// 			dstImage.at<uchar>(src_bw_image.rows - 1, j) = 255;
// 		}
// 		// 将最左最右两列变白
// 		for (int i = 0; i < src_bw_image.rows; i++)
// 		{
// 			dstImage.at<uchar>(i, 0) = 255;
// 			dstImage.at<uchar>(i, src_bw_image.cols - 1) = 255;
// 		}
// 	}
// 	return dstImage;
// }