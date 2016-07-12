#include "opencv2/opencv.hpp"
#include <windows.h>
#include "czh_cv.h"

using namespace cv;
using namespace std;

static enum { RED, GREEN, BLUE, WHITE, BLACK } color;
#define WORD_IN_RED SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY |	FOREGROUND_RED)
#define WORD_IN_WHITE SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);

void czh_inverseBinary(Mat & src_bw_Image, Mat & dst_bw_Image)
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
	cv::LUT(tmpImage, lookUpTable, dst_bw_Image);
}

void czh_endPoint(Mat & src_skeleton_Image, Mat & dst_endPoint_Image)
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

void czh_writeWhitePixel(Mat & srcImage, vector<Point2f> & dstWhitePoints)
{
	// 该函数输入一幅二值图像，它将找出该图像中的白色像素
	// 并且将这些像素坐标存于输入参数矢量 vector<Point2f> & dstWhitePoints 之中
	Mat src = srcImage;

	// 清除输入参数矢量中原有的数据并释放空间
	vector<Point2f>().swap(dstWhitePoints);

	Point2f curPoint;	//	用于遍历图像的当前点

	for (int i = 0; i < src.rows; i++)
	{
		for (int j = 0; j < src.cols; j++)
		{
			// 如果该点是白色，则把它的 x,y 赋给当前点数据，并将该店推进矢量中
			if (src.at<uchar>(i, j) == 255)
			{
				curPoint.x = j;
				curPoint.y = i;
				dstWhitePoints.push_back(curPoint);
			}
		}
	}

	// 输出所有白色像素信息
	cout << "White pixels coordinates:\n";
	for (int i = 0; i < dstWhitePoints.size(); i++)
	{
		cout << "\t#" << i + 1 << ":\t" << dstWhitePoints[i].x << ",\t" << dstWhitePoints[i].y << endl;
	}
}

void czh_thin_parallel(Mat & srcImage, Mat & dstImage, int iterations)
{
	// 该函数输入一个二值图像或者灰度图像，输出该图像的骨骼
	if (srcImage.type() != CV_8UC1)
	{
		cerr << "只能处理二值或灰度图像.\n";
		cerr << "读取图像函数: imread() 加上参数 \"0\" 或许可以修复该问题.\n";
		return;
	}

	Mat tmpImage;
	int p2, p3, p4, p5, p6, p7, p8, p9;
	uchar * ptrTop, *ptrCur, *ptrBot; // 8邻域中上一行，当前行和下一行的行指针
	// 因为是 8 邻域操作，所以宽和高各减1，防止指针溢出
	const int height = srcImage.rows - 1;	
	const int width = srcImage.cols - 1;
	bool isFinished = false;

	srcImage.copyTo(dstImage);

	for (int it = 0; it < iterations; it++)
	{
		// 每次迭代，都考察 tmpImage 上的点，修改 dstImage 上的数据
		// 新一次的迭代开始时，将 dstImage 上的数据复制到 tmpImage上
		dstImage.copyTo(tmpImage);
		isFinished = false;

		// 遍历扫描过程一:开始
		for (int i = 1; i < height; i++)
		{
			// 给3*3邻域中每一行行指针赋值
			ptrTop = tmpImage.ptr<uchar>(i - 1);
			ptrCur = tmpImage.ptr<uchar>(i);
			ptrBot = tmpImage.ptr<uchar>(i + 1);

			for (int j = 1; j < width; j++)	
			{	
				if (ptrCur[j] > 0)	// 如果当前像素为目标像素的话
				{
					int AP = 0;
					p2 = (ptrTop[j] > 0);
					p3 = (ptrTop[j + 1] > 0);
					p4 = (ptrCur[j + 1] > 0);
					p5 = (ptrBot[j + 1] > 0);
					p6 = (ptrBot[j] > 0);
					p7 = (ptrBot[j - 1] > 0);
					p8 = (ptrCur[j - 1] > 0);
					p9 = (ptrTop[j - 1] > 0);
					if (p2 == 0 && p3 == 1) AP++;
					if (p3 == 0 && p4 == 1) AP++;
					if (p4 == 0 && p5 == 1) AP++;
					if (p5 == 0 && p6 == 1) AP++;
					if (p6 == 0 && p7 == 1) AP++;
					if (p7 == 0 && p8 == 1) AP++;
					if (p8 == 0 && p9 == 1) AP++;
					if (p9 == 0 && p2 == 1) AP++;

					// 如果四个条件都满足的话，则删除 dstImage 中当前点
					if (((p2 + p3 + p4 + p5 + p6 + p7 + p8 + p9) >= 2) && ((p2 + p3 + p4 + p5 + p6 + p7 + p8 + p9) <= 6))
					{
						if (AP == 1)
						{
							if (p2*p4*p6 == 0)
							{
								if (p4*p6*p8 == 0)
								{
									dstImage.ptr<uchar>(i)[j] = 0;
									isFinished = true;
								}
							}
						}
					}
				}
			}
		} // 遍历扫描过程一:结束

		  // 遍历扫描过程二:开始
		dstImage.copyTo(tmpImage);
		for (int i = 1; i < height; i++)
		{
			ptrTop = tmpImage.ptr<uchar>(i - 1);
			ptrCur = tmpImage.ptr<uchar>(i);
			ptrBot = tmpImage.ptr<uchar>(i + 1);

			for (int j = 1; j < width; j++)
			{
				if (ptrCur[j] > 0)
				{
					int AP = 0;
					p2 = (ptrTop[j] > 0);
					p3 = (ptrTop[j + 1] > 0);
					p4 = (ptrCur[j + 1] > 0);
					p5 = (ptrBot[j + 1] > 0);
					p6 = (ptrBot[j] > 0);
					p7 = (ptrBot[j - 1] > 0);
					p8 = (ptrCur[j - 1] > 0);
					p9 = (ptrTop[j - 1] > 0);
					if (p2 == 0 && p3 == 1) AP++;
					if (p3 == 0 && p4 == 1) AP++;
					if (p4 == 0 && p5 == 1) AP++;
					if (p5 == 0 && p6 == 1) AP++;
					if (p6 == 0 && p7 == 1) AP++;
					if (p7 == 0 && p8 == 1) AP++;
					if (p8 == 0 && p9 == 1) AP++;
					if (p9 == 0 && p2 == 1) AP++;

					if (((p2 + p3 + p4 + p5 + p6 + p7 + p8 + p9) >= 2) && ((p2 + p3 + p4 + p5 + p6 + p7 + p8 + p9) <= 6))
					{
						if (AP == 1)
						{
							if (p2*p4*p8 == 0)
							{
								if (p2*p6*p8 == 0)
								{
									dstImage.ptr<uchar>(i)[j] = 0;
									isFinished = true;
								}
							}
						}
					}

				}
			}
		} // 遍历扫描过程二:结束

		//如果两个遍历扫描过程已经没有可以细化的像素了，则退出迭代
		if (!isFinished) break;
	}
}

void czh_thin_LUT(Mat & srcImage, Mat & dstImage, int iterations)
{
	// 该函数输入一个二值图像或者灰度图像，输出该图像的骨骼
	if (srcImage.type() != CV_8UC1)
	{
		cerr << "只能处理二值或灰度图像.\n";
		cerr << "读取图像函数: imread() 加上参数 \"0\" 或许可以修复该问题.\n";
		return;
	}

	// 用于查表法的表
	int array[256] = {	0,0,1,1,0,0,1,1,1,1,0,1,1,1,0,1,
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
						1,1,0,0,1,1,1,0,1,1,0,0,1,0,0,0 };

	// 准备临时 Mat 对象
	Mat tempImage;
	srcImage.copyTo(dstImage);
	srcImage.copyTo(tempImage);

	// 为了保证效果，先做膨胀操作，使断点连接起来
	dilate(srcImage, dstImage, getStructuringElement(MORPH_RECT, Size(3, 3), Point(-1, -1)), Point(-1, -1), 3);
	
	// 该算法是用于黑色像素，所以先翻转图像进行抽取骨架操作，最后再转换回去
	MatIterator_<uchar> itBegin, itEnd;
	for (itBegin = dstImage.begin<uchar>(), itEnd = dstImage.end<uchar>(); itBegin != itEnd; itBegin++)
	{
		(*itBegin) = abs((*itBegin) - 255);
	}

	// 开始迭代
	int pointValue;

	uchar * ptrTop, *ptrCur, *ptrBot; // 8邻域中上一行，当前行和下一行的行指针
	uchar * dstPtr;	// 目标图像当前行指针

	// 因为是 8 邻域操作，所以宽和高各减1，防止指针溢出
	const int height = srcImage.rows - 1;
	const int width = srcImage.cols - 1;

	for (int loop = 0; loop < iterations; loop++)
	{
		bool changed = false;	// 判断该次迭代是否修改了像素颜色

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//																														   //
		//													遍历扫描过程一:开始                                                    //
		//																														   //
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		bool next = true;			// 重置跳跃监视符
		dstImage.copyTo(tempImage);	// 每次迭代，都将修改过后的目标图像复制给临时图像进行检查处理

		for (int i = 1; i < height; i++)
		{
			// 给3*3邻域中每一行行指针赋值
			ptrTop = tempImage.ptr<uchar>(i - 1);
			ptrCur = tempImage.ptr<uchar>(i);
			ptrBot = tempImage.ptr<uchar>(i + 1);
			dstPtr = dstImage.ptr<uchar>(i);

			for (int j = 1; j < width; j++)
			{
				if (((int)ptrCur[j - 1] + (int)ptrCur[j] + (int)ptrCur[j + 1]) == 0)	// 如果该点左右邻居都是黑色，则跳过该点不做处理
				{
					dstPtr[j] = ptrCur[j];
					continue;
				};

				if (next == false)	// 如果修改完某个点( next 会被置为 false)，则跳过下一个点不做处理
				{
					next = true;
					dstPtr[j] = ptrCur[j];
					continue;
				};

				if ((int)ptrCur[j] == 0)	// 如果某个点为黑色，进行判断
				{
					pointValue = 0;

					// 第一行
					if ((int)(ptrTop[j - 1]) == 255)
					{
						pointValue += 1;
					}
					if ((int)(ptrTop[j]) == 255)
					{
						pointValue += 2;
					}
					if ((int)(ptrTop[j + 1]) == 255)
					{
						pointValue += 4;
					}

					// 第二行
					if ((int)(ptrCur[j - 1]) == 255)
					{
						pointValue += 8;
					}
					if ((int)(ptrCur[j]) == 255)
					{
						pointValue += 0;
					}
					if ((int)(ptrCur[j + 1]) == 255)
					{
						pointValue += 16;
					}

					// 第三行
					if ((int)(ptrBot[j - 1]) == 255)
					{
						pointValue += 32;
					}
					if ((int)(ptrBot[j]) == 255)
					{
						pointValue += 64;
					}
					if ((int)(ptrBot[j + 1]) == 255)
					{
						pointValue += 128;
					}

					dstPtr[j] = 255 * array[pointValue];

					if (dstPtr[j] == 255)	
					{
						next = false;	// 如果修改了当前像素（从黑变为白），则跳过下一个点
						changed = true;	// 并且把 changed 设置为true
					}
					else dstPtr[j] = ptrCur[j];
				}
			}
		}

		// 将最上下两行，左右两列变为白色
		ptrTop = dstImage.ptr<uchar>(0);
		ptrBot = dstImage.ptr<uchar>(dstImage.rows - 1);

		for (int i = 0; i < dstImage.cols; i++)
		{
			ptrTop[i] = 255;
			ptrBot[i] = 255;
		}
		for (int i = 0; i < dstImage.rows; i++)
		{
			dstImage.at<uchar>(i, 0) = 255;
			dstImage.at<uchar>(i, dstImage.cols - 1) = 255;
		}

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//																														   //
		//													遍历扫描过程二:开始                                                    //
		//																														   //
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		next = true;			// 重置跳跃监视符
		dstImage.copyTo(tempImage);	// 每次迭代，都将修改过后的目标图像复制给临时图像进行检查处理

		for (int j = 1; j < width; j++)
		{
			for (int i = 1; i < height; i++)
			{
				// 给3*3邻域中每一行行指针赋值
				ptrTop = tempImage.ptr<uchar>(i - 1);
				ptrCur = tempImage.ptr<uchar>(i);
				ptrBot = tempImage.ptr<uchar>(i + 1);
				dstPtr = dstImage.ptr<uchar>(i);

				if (((int)ptrTop[j] + (int)ptrCur[j] + (int)ptrBot[j]) == 0)	// 如果该点上下邻居都是黑色，则跳过该点不做处理
				{
					dstPtr[j] = ptrCur[j];
					continue;
				};

				if (next == false)	// 如果修改完某个点( next 会被置为 false)，则跳过下一个点不做处理
				{
					next = true;
					dstPtr[j] = ptrCur[j];
					continue;
				};

				if ((int)ptrCur[j] == 0)	// 如果某个点为黑色，进行判断
				{
					pointValue = 0;

					// 第一行
					if ((int)(ptrTop[j - 1]) == 255)
					{
						pointValue += 1;
					}
					if ((int)(ptrTop[j]) == 255)
					{
						pointValue += 2;
					}
					if ((int)(ptrTop[j + 1]) == 255)
					{
						pointValue += 4;
					}

					// 第二行
					if ((int)(ptrCur[j - 1]) == 255)
					{
						pointValue += 8;
					}
					if ((int)(ptrCur[j]) == 255)
					{
						pointValue += 0;
					}
					if ((int)(ptrCur[j + 1]) == 255)
					{
						pointValue += 16;
					}

					// 第三行
					if ((int)(ptrBot[j - 1]) == 255)
					{
						pointValue += 32;
					}
					if ((int)(ptrBot[j]) == 255)
					{
						pointValue += 64;
					}
					if ((int)(ptrBot[j + 1]) == 255)
					{
						pointValue += 128;
					}

					dstPtr[j] = 255 * array[pointValue];

					if (dstPtr[j] == 255)	// 如果修改了当前像素（从黑变为白），则跳过下一个点
					{
						next = false;	// 如果修改了当前像素（从黑变为白），则跳过下一个点
						changed = true;	// 并且把 changed 设置为true
					}
					else dstPtr[j] = ptrCur[j];
				}
			}
		}

		// 将最上下两行，左右两列变为白色
		ptrTop = dstImage.ptr<uchar>(0);
		ptrBot = dstImage.ptr<uchar>(dstImage.rows - 1);
		for (int i = 0; i < dstImage.cols; i++)
		{
			ptrTop[i] = 255;
			ptrBot[i] = 255;
		}
		for (int i = 0; i < dstImage.rows; i++)
		{
			dstImage.at<uchar>(i, 0) = 255;
			dstImage.at<uchar>(i, dstImage.cols - 1) = 255;
		}

		if (changed == false)
		{
			break;	// 如果一次迭代没有删除任何像素，则认为已经结束细化，则直接退出
		}
	}

	// 最后再反转黑白
	for (itBegin = dstImage.begin<uchar>(), itEnd = dstImage.end<uchar>(); itBegin != itEnd; itBegin++)
	{
		(*itBegin) = abs((*itBegin) - 255);
	}
}

void czh_extractColor(Mat &srcImage, Mat & dstImage, int color = RED)
{
	// 输入参数：Mat & srcImage 是一个三通道 Mat对象昂
	// 输入参数：Mat & dstImage 是一个与 srcImage 相同尺寸的单通道Mat对象，并输出至 dstImage 对象中
	// 输入参数：int color 是想要提取的颜色，目前只支持红色（RED）
	// 该函数输入一个三通道RGB图像，抽取该图像中的 color 颜色的像素，保存为白色像素，其他像素置为黑色，结果保存在 dstImage 对象中

	if (!srcImage.data)
	{
		cerr << "打开图像失败." << endl;
		return;
	}
	if (srcImage.type() != CV_8UC3)
	{
		cerr << "该函数只支持三通道图像." << endl;
		return;
	}
	if (dstImage.type() != CV_8UC1)
	{
		cerr << "该函数输出图像对象必须为一个单通道对象." << endl;
		return;
	}

	Mat temp; srcImage.copyTo(temp); // 把源图像复制到 temp 临时对象之中
	Mat dst(srcImage.size(), CV_8UC1);	// 目标图像初始化为源图像尺寸，单通道
	MatIterator_<Vec3b> itBegin, itEnd;	// 源图像遍历迭代器
	MatIterator_<uchar> itDst;	// 目标图像遍历迭代器
	int meanRed, meanGreen, distance, distanceThreshold;;		// 阈值算法需要的参数: 红色均值，绿色均值，像素均值之差，阈值
	
	switch (color)	// 根据不同颜色，选择不同的参数
	{
		case RED:
			meanRed = 200;
			meanGreen = 23;
			distanceThreshold = 70;
			break;
		case BLUE:
			break;
		case GREEN:
			break;
	}

	for (itBegin = temp.begin<Vec3b>(), itEnd = temp.end<Vec3b>(), itDst = dst.begin<uchar>(); itBegin != itEnd; itBegin++, itDst++)
	{
		// (*itBegin)[1]  Green pixels
		// (*itBegin)[2]  Red pixels
		distance = fabs((*itBegin)[1] - meanGreen) + fabs((*itBegin)[2] - meanRed);
		if (distance < distanceThreshold)
		{
			(*itDst) = 255;
		}
		else
		{
			(*itDst) = 0;
		}
	}
	dst.copyTo(dstImage);	// 将临时目标对象赋给函数目标对象
}

void czh_minFilter(Mat &srcImage, Mat & dstImage, int radius)
{
	Mat src;
	srcImage.copyTo(src);
	Mat dst(srcImage.size(), srcImage.type(), Scalar::all(0));
	switch (srcImage.type())
	{
	case CV_8UC1:
	{
		int minValue;
		for (int i = radius; i < src.rows - radius; i++)	// 遍历每个像素
		{
			for (int j = radius; j < src.cols - radius; j++)
			{
				minValue = src.at<uchar>(i, j);
				// 针对每个像素，在一个窗口之中遍历寻找最小值
				for (int windows_rows_number = - radius; windows_rows_number <= radius; windows_rows_number++)
				{
					for (int windows_cols_number = - radius; windows_cols_number <= radius; windows_cols_number++)
					{
						if (src.at<uchar>(i + windows_rows_number, j + windows_cols_number) < minValue)
						{
							minValue = src.at<uchar>(i + windows_rows_number, j + windows_cols_number);
						}
					}
				}
				dst.at<uchar>(i, j) = minValue;
			}
		}
		
		for (int i = radius; i < src.rows - radius; i++)
		{
			for (int j = radius - 1; j >= 0; j--)
			{
				dst.at<uchar>(i, j) = dst.at<uchar>(i, j + 1);
			}

			for (int j = src.cols - radius; j < src.cols; j++)
			{
				dst.at<uchar>(i, j) = dst.at<uchar>(i, j - 1);
			}
		}

		for (int j = 0; j < src.cols; j++)
		{
			for (int i = radius - 1; i >= 0; i--)
			{
				dst.at<uchar>(i, j) = dst.at<uchar>(i + 1, j);
			}

			for (int i = src.rows - radius; i < src.rows; i++)
			{
				dst.at<uchar>(i, j) = dst.at<uchar>(i - 1, j);
			}
		}

		break;
	}
		
	case CV_32FC1:
	{
		float minValue;
		for (int i = radius; i < src.rows - radius; i++)	// 遍历每个像素
		{
			for (int j = radius; j < src.cols - radius; j++)
			{
				minValue = src.at<float>(i, j);
				// 针对每个像素，在一个窗口之中遍历寻找最小值
				for (int windows_rows_number = -radius; windows_rows_number <= radius; windows_rows_number++)
				{
					for (int windows_cols_number = -radius; windows_cols_number <= radius; windows_cols_number++)
					{
						if (src.at<float>(i + windows_rows_number, j + windows_cols_number) < minValue)
						{
							minValue = src.at<float>(i + windows_rows_number, j + windows_cols_number);
						}
					}
				}
				dst.at<float>(i, j) = minValue;
			}
		}

		for (int i = radius; i < src.rows - radius; i++)
		{
			for (int j = radius - 1; j >= 0; j--)
			{
				dst.at<float>(i, j) = dst.at<float>(i, j + 1);
			}

			for (int j = src.cols - radius; j < src.cols; j++)
			{
				dst.at<float>(i, j) = dst.at<float>(i, j - 1);
			}
		}

		for (int j = 0; j < src.cols; j++)
		{
			for (int i = radius - 1; i >= 0; i--)
			{
				dst.at<float>(i, j) = dst.at<float>(i + 1, j);
			}

			for (int i = src.rows - radius; i < src.rows; i++)
			{
				dst.at<float>(i, j) = dst.at<float>(i - 1, j);
			}
		}
		break;
	}
	} // switch finishes
	
	dstImage = dst;
}

void czh_imwrite(Mat &dstImage,const string imageName)
{
	// 该函数自动判断输出图像Mat对象的类型，以决定输出图像是灰度图像或者是三通道图像
	string dstFileName;
	switch (dstImage.type())
	{
	case CV_8UC1:
		dstFileName = "result " + imageName + ".pgm";
		break;
	default:
		dstFileName = "result " + imageName + ".ppm";
		break;
	}
	cout << "Output image name:\t" << dstFileName << endl;
	imwrite(dstFileName, dstImage);
}

void czh_helpInformation(string const &functionInfo)
{
	// 程序信息：程序功能
	WORD_IN_WHITE;
	cout << "************************************************************************************************************************\n";
	cout << "\t\t\t\t\tProgram information:\n";
	cout << "\t\t\t\t" << functionInfo << "\n\n";
	cout << "************************************************************************************************************************\n\n";
}

void czh_imageOpenDetect(Mat & srcImage, string & fileName, string & fileType)
{	// 检测图像对象是否已经成功打开
	string srcFileName = fileName + fileType;
	if (srcImage.data == nullptr)
	{
		WORD_IN_RED;
		cerr << "警告: 试图打开文件: " << srcFileName << " 失败，请确认文件名和文件类型正确.\n\n";
		WORD_IN_WHITE;
		cout << "Enter the input image name without " << fileType << ": ";
		fileName.clear(); srcFileName.clear();
		getline(cin, fileName);				// 获得输入文件名
		srcFileName = fileName + fileType;	// 确定输入图片文件类型
		srcImage = imread(srcFileName);	// 读取源图像
		czh_imageOpenDetect(srcImage, fileName, srcFileName);
	}
	return;
}

void czh_middlePointCircle(int x0, int y0, int radius, vector<Point> &pointsOfCircle)
{
	// 该函数计算以点 (x0, y0) 为圆心， radius为半径的圆
	// 并将圆上的点的坐标导入 vector<Point>矢量 pointsOfCircle 之中
	// 中点画圆法的另外一种优化方法
	int x = radius;
	int y = 0;
	int error = 0;

	vector<Point>().swap(pointsOfCircle);	// 清空目标矢量
	vector<vector<Point>> pointsDirection(8);	// 临时储存矢量

	while (x >= y)
	{
		pointsDirection[0].push_back(Point(x0 + x, y0 + y));
		pointsDirection[1].push_back(Point(x0 + y, y0 + x));
		pointsDirection[6].push_back(Point(x0 - y, y0 + x));
		pointsDirection[7].push_back(Point(x0 - x, y0 + y));
		pointsDirection[4].push_back(Point(x0 - x, y0 - y));
		pointsDirection[5].push_back(Point(x0 - y, y0 - x));
		pointsDirection[2].push_back(Point(x0 + y, y0 - x));
		pointsDirection[3].push_back(Point(x0 + x, y0 - y));
		
		error += 1 + 2 * y;
		y += 1;
		if (2 * (error - x) + 1 > 0)
		{
			error += 1 - 2 * x;
			x -= 1;
		}
	}

	// 因为对称性，有4个组的点旋转顺序并不正确，需要调整顺序
	for (int i = 0; i < pointsDirection[1].size() / 2; i++)
	{
		swap(pointsDirection[1][i], pointsDirection[1][pointsDirection[1].size() - 1 - i]);
		swap(pointsDirection[3][i], pointsDirection[3][pointsDirection[3].size() - 1 - i]);
		swap(pointsDirection[5][i], pointsDirection[5][pointsDirection[5].size() - 1 - i]);
		swap(pointsDirection[7][i], pointsDirection[7][pointsDirection[7].size() - 1 - i]);
	}

	for (int i = 0; i < 8; i++)
	{
		for (int j = 0; j < pointsDirection[0].size(); j++)
		{
			pointsOfCircle.push_back(pointsDirection[i][j]);
		}
	}
}

void czh_BresenhamCircle(int x0, int y0, int radius, vector<Point> &pointsOfCircle)
{
	// 该函数计算以点 (x0, y0) 为圆心， radius为半径的圆
	// 并将圆上的点的坐标以12点钟方向开始顺时针方向依次导入 vector<Point>矢量 pointsOfCircle 之中
	// ref : algorithm of Bresenham
	vector<Point>().swap(pointsOfCircle);	// 清空目标矢量
	vector<vector<Point>> pointsDirection(8);	// 临时储存矢量
	int x, y, distance;
	x = 0;
	y = radius;
	distance = 3 - 2 * radius;

	while (x < y)
	{
		pointsDirection[0].push_back(Point(x0 + x, y0 + y));
		pointsDirection[1].push_back(Point(x0 + y, y0 + x));// 顺序不正确
		pointsDirection[6].push_back(Point(x0 - y, y0 + x));
		pointsDirection[7].push_back(Point(x0 - x, y0 + y));// 顺序不正确
		pointsDirection[4].push_back(Point(x0 - x, y0 - y));
		pointsDirection[5].push_back(Point(x0 - y, y0 - x));// 顺序不正确
		pointsDirection[2].push_back(Point(x0 + y, y0 - x));
		pointsDirection[3].push_back(Point(x0 + x, y0 - y));// 顺序不正确

		// 判断下一次的点坐标
		if (distance < 0)
		{
			distance += 4 * x + 6;
		}
		else
		{
			distance += 4 * (x - y) + 10;
			y--;
		}
		x++;
	}


// 	pointsDirection[0].push_back(Point(x0 + x, y0 + y));
// 	pointsDirection[1].push_back(Point(x0 + y, y0 + x));
// 	pointsDirection[6].push_back(Point(x0 - y, y0 + x));
// 	pointsDirection[7].push_back(Point(x0 - x, y0 + y));
// 	pointsDirection[4].push_back(Point(x0 - x, y0 - y));
// 	pointsDirection[5].push_back(Point(x0 - y, y0 - x));
// 	pointsDirection[2].push_back(Point(x0 + y, y0 - x));
// 	pointsDirection[3].push_back(Point(x0 + x, y0 - y));
// 
// 	x++;
// 
// 	while (x < y)
// 	{
// 		if (distance < 0)
// 		{
// 			distance += 4 * x + 6;
// 		}
// 		else
// 		{
// 			distance += 4 * (x - y) + 10;
// 			y--;
// 		}
// 		pointsDirection[0].push_back(Point(x0 + x, y0 + y));	
// 		pointsDirection[1].push_back(Point(x0 + y, y0 + x));// 顺序不正确
// 		pointsDirection[6].push_back(Point(x0 - y, y0 + x));	
// 		pointsDirection[7].push_back(Point(x0 - x, y0 + y));// 顺序不正确
// 		pointsDirection[4].push_back(Point(x0 - x, y0 - y));	
// 		pointsDirection[5].push_back(Point(x0 - y, y0 - x));// 顺序不正确
// 		pointsDirection[2].push_back(Point(x0 + y, y0 - x));	
// 		pointsDirection[3].push_back(Point(x0 + x, y0 - y));// 顺序不正确
// 
// 		x++;
// 	}

	// 因为对称性，有4个组的点旋转顺序并不正确，需要调整顺序
	for (int i = 0; i < pointsDirection[1].size() / 2; i++)
	{
		swap(pointsDirection[1][i], pointsDirection[1][pointsDirection[1].size() - 1 - i]);
		swap(pointsDirection[3][i], pointsDirection[3][pointsDirection[3].size() - 1 - i]);
		swap(pointsDirection[5][i], pointsDirection[5][pointsDirection[5].size() - 1 - i]);
		swap(pointsDirection[7][i], pointsDirection[7][pointsDirection[7].size() - 1 - i]);
	}

	for (int i = 0; i < 8; i++)
	{
		for (int j = 0; j < pointsDirection[i].size(); j++)
		{
			pointsOfCircle.push_back(pointsDirection[i][j]);
		}
	}
}

void czh_BresenhamLine(Point pt0, Point pt1, vector<Point> &linePoints)
{
	// 该函数计算以点 pt0 为起点， 点 pt1 为终点的直线，并把直线点存于 linePoints 中
	linePoints.clear();	// 清空用于存放点的向量
	
	int x0 = pt0.x;
	int y0 = pt0.y;
	int x1 = pt1.x;
	int y1 = pt1.y;

	bool directionRightToLeft = false;	// 是否倒序排点

	bool steep = (abs(y1 - y0) > abs(x1 - x0));
	if (steep)	// 如果斜率大于1，则把斜率变为  1/原来的斜率 来计算，排点的时候颠倒 x 和 y坐标
	{
		swap(x0, y0);
		swap(x1, y1);
	}
	if (x0 > x1)	// 当起点x坐标比终点x坐标大，则从反方向排点
	{
		directionRightToLeft = true;
		swap(x0, x1);
		swap(y0, y1);
	}
	int deltax = x1 - x0;
	int deltay = abs(y1 - y0);
	int error = deltax / 2;
	int ystep;
	int y = y0;

	if (y0 < y1)	// 看 y 方向是向上还是向下，采取不一样的步进策略
		ystep = 1;
	else
		ystep = -1;

	for (int x = x0; x <= x1; x++)
	{
		if (steep)	// 根据原来斜率是否小于1，采取不一样的排点方式：(x,y)或者(y,x)
		{
			linePoints.push_back(Point(y, x));
		}
		else
		{
			linePoints.push_back(Point(x, y));
		}
		error = error - deltay;
		if (error < 0)
		{
			y = y + ystep;
			error = error + deltax;
		}
	}

	// 默认推点顺序是从左上角向右下角依次放点，如果初始点是右下角，则颠倒点向量顺序
	if (directionRightToLeft == true)
	{
		for (int i = 0; i < linePoints.size() / 2; i++)
			swap(linePoints[i], linePoints[linePoints.size() - 1 - i]);
	}
}

void czh_Circle_Profile(const Mat & srcImage, const int x0, const int y0, const int radius)
{
	// 展示以选定点为圆心，radius为半径的圆的灰度变化

	// 对以被选择的点为圆心，radius为半径画一个圆，圆上的点坐标被存在 pointsOfCircle 矢量之中
	vector<Point> pointsOfCircle;
	czh_BresenhamCircle(x0, y0, radius, pointsOfCircle);

	// 将圆上的点的灰度值存在 valuesOfPoints 矢量之中
	vector<int> valuesOfPoints;	// 清空目标矢量
	for (int i = 0; i < pointsOfCircle.size(); i++)
	{
		valuesOfPoints.push_back((int)srcImage.at<uchar>(pointsOfCircle[i]));
	}
	valuesOfPoints.push_back((int)srcImage.at<uchar>(pointsOfCircle[0]));	// 额外添加一个元素，它与第一个像素的灰度相同

	int profileHeight = 255;	// profile图像高度
	int profileWidth = pointsOfCircle.size() * 6;	// profile 图像宽度
	Mat profileImage(profileHeight, profileWidth, CV_8UC1, Scalar::all(0));

	for (int x = 0; x < pointsOfCircle.size(); x++)
	{
		int y = profileHeight - valuesOfPoints[x] / 2;
		for (; y < profileHeight; y++)
		{
			profileImage.at<uchar>(y, 4 * x) = 255;
			profileImage.at<uchar>(y, 4 * x + 1) = 255;
			profileImage.at<uchar>(y, 4 * x + 2) = 255;
			profileImage.at<uchar>(y, 4 * x + 3) = 255;
		}
	}
	imshow("Profile", profileImage);
}

Mat czh_Line_Profile(const Mat & srcImage, Point pt0, Point pt1, bool sameSize)
{
	// 展示以选定的两个点为端点的直线在图中的灰度变化,返回变化图像以
	vector<Point> linePts;	// 用于存放直线上的点的点向量
	czh_BresenhamLine(pt0, pt1, linePts);	// 找出直线上的点，放进 linePts

	// 将线上的点的灰度值存在 valuesOfPoints 矢量之中
	vector<int> valuesOfPoints;	// 清空目标矢量
	for (int i = 0; i < linePts.size(); i++)
	{
		valuesOfPoints.push_back((int)srcImage.at<uchar>(linePts[i]));
	}

	int profileHeight = 255;	// profile图像高度
	int profileWidth;	// profile 图像宽度

	if(!sameSize)
		profileWidth = linePts.size() * 6;	
	else profileWidth = linePts.size();

	Mat profileImage(profileHeight, profileWidth, CV_8UC1, Scalar::all(0));

	for (int x = 0; x < linePts.size(); x++)
	{
		int y = profileHeight - valuesOfPoints[x] / 2;
		for (; y < profileHeight; y++)
		{
			if (!sameSize)
			{
				profileImage.at<uchar>(y, 4 * x) = 255;
				profileImage.at<uchar>(y, 4 * x + 1) = 255;
				profileImage.at<uchar>(y, 4 * x + 2) = 255;
				profileImage.at<uchar>(y, 4 * x + 3) = 255;
			}
			else
			{
				profileImage.at<uchar>(y, x) = 255;
			}
		}
	}
	// imshow("Line Profile", profileImage);
	return profileImage;
}

void czh_labeling(Mat & src, Mat & dst)
{
	// 该函数会使用 two - pass 算法针对二值图像做连通域标记

	// 准备函数临时对象
	Mat srcImage;
	src.copyTo(srcImage);

	///////////////////////////////////////////////////////////
	//				      first pass                         //
	///////////////////////////////////////////////////////////

	// 因为是 8 邻域操作，所以宽和高各减1，防止指针溢出
	const int height = srcImage.rows - 1;
	const int width = srcImage.cols - 1;

	uchar *data_cur, *data_prev;	// 当前行和上一行行指针
	int label = 1;
	vector<int> labelSet;
	labelSet.push_back(0);
	labelSet.push_back(1);

	///////////////////////////////////////////////////////////////////////////////////////////////////
	////////									处理第一行						    			///////
	////////									   开始							    			///////
	data_cur = srcImage.ptr<uchar>(0);

	if (data_cur[0] != 0)	// 如果第一行的第一个元素不为0，则给它一个label
	{
		labelSet.push_back(++label);	// 则在 labelSet 中新添加一个 label
		labelSet[label] = label;		// 给这个位置的 label 赋值
		data_cur[0] = label;			// 修改源图像素灰度值为 label
	}

	for (int j = 1; j < srcImage.cols; j++)
	{
		if (data_cur[j] == 0)
		{	// 如果当前像素不是目标像素，则跳过该像素
			continue;
		}
		int left = data_cur[j - 1];	// 左点灰度值
		int cnt = 0;		// 用于计算左点和上点一个有多少个有效值
		int neighborLabels[2];
		if (left != 0)		// 如果左点是有效值
		{
			neighborLabels[cnt++] = left;	// 把左点灰度值赋给 neighborLabels 中一个元素
		}

		if (cnt == 0)	// 如果左点为0
		{
			labelSet.push_back(++label);	// 则在 labelSet 中新添加一个 label
			labelSet[label] = label;		// 给这个位置的 label 赋值
			data_cur[j] = label;			// 修改源图像素灰度值为 label
			continue;						// 继续下一次循环
		}
		int smallestLabel = neighborLabels[0];				// 如果左点和上点只有一个有效值，那么就选择该有效值的 label 作为小的label
		data_cur[j] = smallestLabel;	// 将源图像该点灰度值赋值为小的 label

		// 保存最小等价表  
		for (int k = 0; k < cnt; k++)
		{
			int tempLabel = neighborLabels[k];
			int & oldSmallestLabel = labelSet[tempLabel];
			if (oldSmallestLabel > smallestLabel)
			{
				labelSet[oldSmallestLabel] = smallestLabel;
			}
			else if (oldSmallestLabel < smallestLabel)
			{
				labelSet[smallestLabel] = oldSmallestLabel;
			}
		}
	}
	////////									处理第一行						    			///////
	////////									   结束							    			///////
	///////////////////////////////////////////////////////////////////////////////////////////////////

	///////////////////////////////////////////////////////////////////////////////////////////////////
	////////									处理中间						    			///////
	////////									  开始							    			///////
	for (int i = 1; i < height; i++)
	{
		data_cur = srcImage.ptr<uchar>(i);		// 当前行行指针
		data_prev = srcImage.ptr<uchar>(i - 1);	// 上一行行指针

		// 处理该行第一个元素
		if (data_cur[0] != 0)	// 如果该一行的第一个元素不为0，检查它的顶部元素，如果顶部元素未被标记，则给它一个label
		{
			int up = data_prev[0];		// 上点灰度值
			int neighborLabels[2];		// 用于存储左点和上点的灰度值
			int cnt = 0;		// 用于计算左点和上点一个有多少个有效值
			if (up != 0)		// 如果上点是有效值
			{
				neighborLabels[cnt++] = up;		// 把上点灰度值赋给 neighborLabels 中另一个元素
			};
			if (cnt == 0)	// 如果上点为0，给当前点一个 label
			{
				labelSet.push_back(++label);	// 则在 labelSet 中新添加一个 label
				labelSet[label] = label;		// 给这个位置的 label 赋值
				data_cur[0] = label;			// 修改源图像素灰度值为 label
			}
			else
			{
				int smallestLabel = neighborLabels[0];				// 如果左点和上点只有一个有效值，那么就选择该有效值的 label 作为小的label
				data_cur[0] = smallestLabel;	// 将源图像该点灰度值赋值为小的 label

				// 保存最小等价表  
				for (int k = 0; k < cnt; k++)
				{
					int tempLabel = neighborLabels[k];
					int & oldSmallestLabel = labelSet[tempLabel];
					if (oldSmallestLabel > smallestLabel)
					{
						labelSet[oldSmallestLabel] = smallestLabel;
					}
					else if (oldSmallestLabel < smallestLabel)
					{
						labelSet[smallestLabel] = oldSmallestLabel;
					}
				}
				// 每经过一个目标点，且它左点或上点有标记的话，就更新连通域等效最小 label 
				// 将等效连通域中的 label 替换为连通域中最小 label 
				// 所以每次遍历一个新点，连通域中的各个 label 总是该连通域中最小的 label
				for (size_t i = 2; i < labelSet.size(); i++)
				{
					int curLabel = labelSet[i];
					int preLabel = labelSet[curLabel];
					while (preLabel != curLabel)
					{
						curLabel = preLabel;
						preLabel = labelSet[preLabel];
					}
					labelSet[i] = curLabel;
				}
			}
		}

		// 处理每一行的后面的元素
		for (int j = 1; j < srcImage.cols; j++)
		{	// 遍历图中每一个像素
			if (data_cur[j] == 0)
			{	// 如果当前像素不是目标像素，则跳过该像素
				continue;
			}
			int left = data_cur[j - 1];	// 左点灰度值
			int up = data_prev[j];		// 上点灰度值
			int neighborLabels[2];		// 用于存储左点和上点的灰度值
			int cnt = 0;		// 用于计算左点和上点一个有多少个有效值
			if (left != 0)		// 如果左点是有效值
			{
				neighborLabels[cnt++] = left;	// 把左点灰度值赋给 neighborLabels 中一个元素
			}
			if (up != 0)		// 如果上点是有效值
			{
				neighborLabels[cnt++] = up;		// 把上点灰度值赋给 neighborLabels 中另一个元素
			}
			if (cnt == 0)	// 如果左点和上点都为0
			{
				labelSet.push_back(++label);	// 则在 labelSet 中新添加一个 label
				labelSet[label] = label;		// 给这个位置的 label 赋值
				data_cur[j] = label;			// 修改源图像素灰度值为 label
				continue;						// 继续下一次循环
			}
			int smallestLabel = neighborLabels[0];				// 如果左点和上点只有一个有效值，那么就选择该有效值的 label 作为小的label
			if (cnt == 2 && neighborLabels[1] < smallestLabel)	// 如果左点和上点都有有效值，选择小的一个 label
			{
				smallestLabel = neighborLabels[1];
			}
			data_cur[j] = smallestLabel;	// 将源图像该点灰度值赋值为小的 label

			// 保存最小等价表  
			for (int k = 0; k < cnt; k++)
			{
				int tempLabel = neighborLabels[k];
				int & oldSmallestLabel = labelSet[tempLabel];
				if (oldSmallestLabel > smallestLabel)
				{
					// 当原连通域最小 label 比当前最小 label 大时，赋给当前点最小 label 并更新连通域最小 label
					labelSet[oldSmallestLabel] = smallestLabel;
				}
				else if (oldSmallestLabel < smallestLabel)
				{
					labelSet[smallestLabel] = oldSmallestLabel;
				}
			}
			// 每经过一个目标点，且它左点或上点有标记的话，就更新连通域等效最小 label 
			// 将等效连通域中的 label 替换为连通域中最小 label 
			// 所以每次遍历一个新点，连通域中的各个 label 总是该连通域中最小的 label
			for (size_t i = 2; i < labelSet.size(); i++)
			{
				int curLabel = labelSet[i];
				int preLabel = labelSet[curLabel];
				while (preLabel != curLabel)
				{
					curLabel = preLabel;
					preLabel = labelSet[preLabel];
				}
				labelSet[i] = curLabel;
			}
		}
	}

	////////									处理中间						    			///////
	////////									  结束							    			///////
	///////////////////////////////////////////////////////////////////////////////////////////////////


	///////////////////////////////////////////////////////////////////////////////////////////////////
	////////								处理最后一行						    			///////
	////////									开始							    			///////
	data_cur = srcImage.ptr<uchar>(srcImage.rows - 1);
	data_prev = srcImage.ptr<uchar>(srcImage.rows - 2);	// 上一行行指针

	if (data_cur[0] != 0)	// 如果该一行的第一个元素不为0，检查它的顶部元素，如果顶部元素未被标记，则给它一个label
	{
		int up = data_prev[0];		// 上点灰度值
		int neighborLabels[2];		// 用于存储左点和上点的灰度值
		int cnt = 0;		// 用于计算左点和上点一个有多少个有效值
		if (up != 0)		// 如果上点是有效值
		{
			neighborLabels[cnt++] = up;		// 把上点灰度值赋给 neighborLabels 中另一个元素
		};
		if (cnt == 0)	// 如果上点为0，给当前点一个 label
		{
			labelSet.push_back(++label);	// 则在 labelSet 中新添加一个 label
			labelSet[label] = label;		// 给这个位置的 label 赋值
			data_cur[0] = label;			// 修改源图像素灰度值为 label
		}
		else
		{
			int smallestLabel = neighborLabels[0];				// 如果左点和上点只有一个有效值，那么就选择该有效值的 label 作为小的label
			data_cur[0] = smallestLabel;	// 将源图像该点灰度值赋值为小的 label

			// 保存最小等价表  
			for (int k = 0; k < cnt; k++)
			{
				int tempLabel = neighborLabels[k];
				int & oldSmallestLabel = labelSet[tempLabel];
				if (oldSmallestLabel > smallestLabel)
				{
					labelSet[oldSmallestLabel] = smallestLabel;
				}
				else if (oldSmallestLabel < smallestLabel)
				{
					labelSet[smallestLabel] = oldSmallestLabel;
				}
			}
			// 每经过一个目标点，且它左点或上点有标记的话，就更新连通域等效最小 label 
			// 将等效连通域中的 label 替换为连通域中最小 label 
			// 所以每次遍历一个新点，连通域中的各个 label 总是该连通域中最小的 label
			for (size_t i = 2; i < labelSet.size(); i++)
			{
				int curLabel = labelSet[i];
				int preLabel = labelSet[curLabel];
				while (preLabel != curLabel)
				{
					curLabel = preLabel;
					preLabel = labelSet[preLabel];
				}
				labelSet[i] = curLabel;
			}
		}
	}

	for (int j = 1; j < srcImage.cols; j++)
	{
		if (data_cur[j] == 0)
		{	// 如果当前像素不是目标像素，则跳过该像素
			continue;
		}
		int left = data_cur[j - 1];	// 左点灰度值
		int up = data_prev[j];		// 上点灰度值
		int neighborLabels[2];		// 用于存储左点和上点的灰度值
		int cnt = 0;		// 用于计算左点和上点一个有多少个有效值
		if (left != 0)		// 如果左点是有效值
		{
			neighborLabels[cnt++] = left;	// 把左点灰度值赋给 neighborLabels 中一个元素
		}
		if (up != 0)		// 如果上点是有效值
		{
			neighborLabels[cnt++] = up;		// 把上点灰度值赋给 neighborLabels 中另一个元素
		}
		if (cnt == 0)	// 如果左点和上点都为0
		{
			labelSet.push_back(++label);	// 则在 labelSet 中新添加一个 label
			labelSet[label] = label;		// 给这个位置的 label 赋值
			data_cur[j] = label;			// 修改源图像素灰度值为 label
			continue;						// 继续下一次循环
		}
		int smallestLabel = neighborLabels[0];				// 如果左点和上点只有一个有效值，那么就选择该有效值的 label 作为小的label
		if (cnt == 2 && neighborLabels[1] < smallestLabel)	// 如果左点和上点都有有效值，选择小的一个 label
		{
			smallestLabel = neighborLabels[1];
		}
		data_cur[j] = smallestLabel;	// 将源图像该点灰度值赋值为小的 label

		// 保存最小等价表  
		for (int k = 0; k < cnt; k++)
		{
			int tempLabel = neighborLabels[k];
			int & oldSmallestLabel = labelSet[tempLabel];
			if (oldSmallestLabel > smallestLabel)
			{
				labelSet[oldSmallestLabel] = smallestLabel;
			}
			else if (oldSmallestLabel < smallestLabel)
			{
				labelSet[smallestLabel] = oldSmallestLabel;
			}
		}
		// 每经过一个目标点，且它左点或上点有标记的话，就更新连通域等效最小 label 
		// 将等效连通域中的 label 替换为连通域中最小 label 
		// 所以每次遍历一个新点，连通域中的各个 label 总是该连通域中最小的 label
		for (size_t i = 2; i < labelSet.size(); i++)
		{
			int curLabel = labelSet[i];
			int preLabel = labelSet[curLabel];
			while (preLabel != curLabel)
			{
				curLabel = preLabel;
				preLabel = labelSet[preLabel];
			}
			labelSet[i] = curLabel;
		}
	}
////////								处理最后一行						    			///////
////////									结束							    			///////
///////////////////////////////////////////////////////////////////////////////////////////////////

		// 更新等价对列表,将最小标号给重复区域  
		for (size_t i = 2; i < labelSet.size(); i++)
		{
			int curLabel = labelSet[i];
			int preLabel = labelSet[curLabel];
			while (preLabel != curLabel) {
				curLabel = preLabel;
				preLabel = labelSet[preLabel];
			}
			labelSet[i] = curLabel;
		}

		///////////////////////////////////////////////////////////
		//				     second pass                         //
		///////////////////////////////////////////////////////////

		// 将图中各个像素点 label 换成同连通域最小 label
		for (int i = 0; i < srcImage.rows; i++)
		{
			data_cur = srcImage.ptr<uchar>(i);		// 当前行行指针
			for (int j = 0; j < srcImage.cols; j++)
			{
				data_cur[j] = labelSet[data_cur[j]];
			}
		}

		srcImage.copyTo(dst);
}

void czh_labeling_backup(Mat & src, Mat & dst)
{
	// 该函数会使用 two - pass 算法针对二值图像做连通域标记

	// 准备函数临时对象
	Mat srcImage; 
	src.copyTo(srcImage);

	///////////////////////////////////////////////////////////
	//				      first pass                         //
	///////////////////////////////////////////////////////////

	// 因为是 8 邻域操作，所以宽和高各减1，防止指针溢出
	const int height = srcImage.rows - 1;
	const int width = srcImage.cols - 1;

	uchar *data_cur, *data_prev;	// 当前行和上一行行指针
	int label = 1;
	vector<int> labelSet;
	labelSet.push_back(0);
	labelSet.push_back(1);
	for (int i = 1; i < height; i++)
	{
		data_cur = srcImage.ptr<uchar>(i);		// 当前行行指针
		data_prev = srcImage.ptr<uchar>(i - 1);	// 上一行行指针

		for (int j = 1; j < width; j++)
		{	// 遍历图中每一个像素
			if (data_cur[j] == 0)
			{	// 如果当前像素不是目标像素，则跳过该像素
				continue;
			}
			int left = data_cur[j - 1];	// 左点灰度值
			int up = data_prev[j];		// 上点灰度值
			int neighborLabels[2];		// 用于存储左点和上点的灰度值
			int cnt = 0;		// 用于计算左点和上点一个有多少个有效值
			if (left != 0)		// 如果左点是有效值
			{
				neighborLabels[cnt++] = left;	// 把左点灰度值赋给 neighborLabels 中一个元素
			}
			if (up != 0)		// 如果上点是有效值
			{
				neighborLabels[cnt++] = up;		// 把上点灰度值赋给 neighborLabels 中另一个元素
			}
			if (cnt == 0)	// 如果左点和上点都为0
			{
				labelSet.push_back(++label);	// 则在 labelSet 中新添加一个 label
				labelSet[label] = label;		// 给这个位置的 label 赋值
				data_cur[j] = label;			// 修改源图像素灰度值为 label
				continue;						// 继续下一次循环
			}
			int smallestLabel = neighborLabels[0];				// 如果左点和上点只有一个有效值，那么就选择该有效值的 label 作为小的label
			if (cnt == 2 && neighborLabels[1] < smallestLabel)	// 如果左点和上点都有有效值，选择小的一个 label
			{
				smallestLabel = neighborLabels[1];
			}
			data_cur[j] = smallestLabel;	// 将源图像该点灰度值赋值为小的 label

			// 保存最小等价表  
			for (int k = 0; k < cnt; k++)
			{
				int tempLabel = neighborLabels[k];
				int & oldSmallestLabel = labelSet[tempLabel];
				if (oldSmallestLabel > smallestLabel)
				{
					labelSet[oldSmallestLabel] = smallestLabel;
				}
				else if (oldSmallestLabel < smallestLabel)
				{
					labelSet[smallestLabel] = oldSmallestLabel;
				}
			}

		}
	}

	// 更新等价对列表,将最小标号给重复区域  
	for (size_t i = 2; i < labelSet.size(); i++)
	{
		int curLabel = labelSet[i];
		int preLabel = labelSet[curLabel];
		while (preLabel != curLabel) {
			curLabel = preLabel;
			preLabel = labelSet[preLabel];
		}
		labelSet[i] = curLabel;
	}

	///////////////////////////////////////////////////////////
	//				     second pass                         //
	///////////////////////////////////////////////////////////

	// 将图中各个像素点 label 换成同连通域最小 label
	for (int i = 0; i < height; i++)
	{
		data_cur = srcImage.ptr<uchar>(i);		// 当前行行指针
		for (int j = 0; j < width; j++)
		{
			data_cur[j] = labelSet[data_cur[j]];
		}
	}

	srcImage.copyTo(dst);
}


////////////////////////////////////////////////////////////////////////////////
//							霍夫变换所需结构								////
struct LinePolar															////
{																			////
	float rho;																////
	float angle;															////
};																			////																		////
////////////////////////////////////////////////////////////////////////////////

void czh_myHoughLines(const Mat & img, std::vector<Vec2f>& lines, float rho, float theta, int thresholdTotal, int thresholdRho, int thresholdAngle)
{
	// 此函数是根据OpenCV里的标准霍夫线变换改编的，核心算法并未改变，但是修改了判定条件
	// # 1 该函数中首先按照霍夫变换将直线参数保持在叠加器中（或者是在霍夫平面之中）
	// # 2 对霍夫平面做连通域标记
	// # 3 对每个连通域求一个重心，输出到目标 lines 向量中
	// # 4 然后归并参数对相近的直线

	int i, j;
	float irho = 1 / rho;
	CV_Assert(img.type() == CV_8UC1);	// 检测图像是否为单通道

										//存放图像数据
	const uchar* image = img.ptr();
	//步长，宽，高
	int step = (int)img.step;
	int width = img.cols;
	int height = img.rows;

	//利用theta和rho这两个分辨率，计算hough图的高宽
	int numangle = cvRound(CV_PI / theta);
	int numrho = cvRound(((width + height) * 2 + 1) / rho);

	//_accum是存放hough图中累积值，及对应hough图的二维矩阵
	AutoBuffer<int> _accum((numangle + 2) * (numrho + 2));

	//计算离散的sin(theta)和cos(theta)
	AutoBuffer<float> _tabSin(numangle);
	AutoBuffer<float> _tabCos(numangle);
	int *accum = _accum;
	float *tabSin = _tabSin, *tabCos = _tabCos;

	memset(accum, 0, sizeof(accum[0]) * (numangle + 2) * (numrho + 2));	//内存赋值为零

	float ang = 0;
	for (int n = 0; n < numangle; ang += theta, n++)
	{
		tabSin[n] = (float)(sin((double)ang) * irho);
		tabCos[n] = (float)(cos((double)ang) * irho);
	}


	/////////////////////////////////////////////////////////////////////////////////////////////
	//																						   //
	// 	      # 1 首先按照霍夫变换将直线参数保持在叠加器中（或者是在霍夫平面之中）	   		   //
	//																						   //
	/////////////////////////////////////////////////////////////////////////////////////////////

	for (i = 0; i < height; i++)
		for (j = 0; j < width; j++)
		{	// 遍历源图像每一个像素
			if (image[i * step + j] != 0)	// 如果该像素是前景像素
				for (int n = 0; n < numangle; n++)	// 则绘制它的霍夫变换曲线，并储存在相应位置
				{
					int r = cvRound(j * tabCos[n] + i * tabSin[n]);
					r += (numrho - 1) / 2;
					accum[(n + 1) * (numrho + 2) + r + 1]++;
				}
		}

	/////////////////////////////////////////////////////////////////////////////////////////////
	//																						   //
	//			                    	# 2 对霍夫平面做连通域标记				     		   //
	//																						   //
	/////////////////////////////////////////////////////////////////////////////////////////////
	Mat maskImage(numangle + 2, numrho + 2, CV_8UC1, Scalar::all(0));

	for (int r = 0; r < numrho; r++)
		for (int n = 0; n < numangle; n++)
		{
			int base = (n + 1) * (numrho + 2) + r + 1;
			if (accum[base] > thresholdTotal)	// 对于霍夫平面上的参数，如果某一参数对的值超过阈值，则在 mask 图像中相应的点置为白色
			{
				maskImage.at<uchar>(n + 1, r + 1) = 255; // 注意这里为了和原霍夫变换函数保持一致，mask 图像尺寸和 accum 尺寸一样
			}
		}

	czh_labeling(maskImage, maskImage);

	/////////////////////////////////////////////////////////////////////////////////////////////
	//																						   //
	//				# 3 对每个连通域求一个重心，输出到目标 lines 向量中						   //
	//																						   //
	/////////////////////////////////////////////////////////////////////////////////////////////
	// 维持一个大小为256的矢量，每一个矢量元素（子矢量）是一个点矢量，用于存放像素灰度值相同的点
	vector<vector<Point>> pointsSet(256);	// 比如像素灰度为 1 的点都存放于 pointSet[1] 之中

	Point curPoint;	// 当前点Point对象
	int curValue;	// 当前点灰度值
	MatIterator_<uchar> itBegin, itEnd;	// 图像迭代器

	for (itBegin = maskImage.begin<uchar>(), itEnd = maskImage.end<uchar>(); itBegin != itEnd; itBegin++)
	{	// 遍历标记图像中每一个像素，将相同像素的点放进一个矢量
		if ((*itBegin) != 0)	// 如果当前点灰度值不为零
		{
			curValue = (*itBegin);	// 当前点灰度值
			curPoint = itBegin.pos();	// 当前点Point对象
			pointsSet[curValue].push_back(curPoint);	// 把当前点压入对应矢量中
		}
	}

	for (int i = 0; i < pointsSet.size(); i++)
	{	// 遍历 pointsSet 的所有256个子矢量

		if (pointsSet[i].size() == 0)
		{	// 如果某个子矢量中一个点都没有，则跳过
			continue;
		}

		// 初始化
		int nCur, rCur;
		int total_n_values = 0, total_r_values = 0, total_values = 0;

		for (int j = 0; j < pointsSet[i].size(); j++)
		{	// 遍历每一个子矢量中的点元素,求得该连通域的 r 值和 n 值
			nCur = pointsSet[i][j].y - 1;
			rCur = pointsSet[i][j].x - 1;
			total_n_values += accum[(nCur + 1) * (numrho + 2) + rCur + 1] * nCur;
			total_r_values += accum[(nCur + 1) * (numrho + 2) + rCur + 1] * rCur;
			total_values += accum[(nCur + 1) * (numrho + 2) + rCur + 1];
		}

		// 求得该灰度值点群的重心
		nCur = cvRound(total_n_values / total_values);
		rCur = cvRound(total_r_values / total_values);

		LinePolar line;
		line.rho = (rCur - (numrho - 1)*0.5f) * rho;
		line.angle = nCur * theta;
		lines.push_back(Vec2f(line.rho, line.angle));
	}


	/////////////////////////////////////////////////////////////////////////////////////////////
	//																						   //
	//								# 4 然后归并参数对相近的直线				     		   //
	//																						   //
	/////////////////////////////////////////////////////////////////////////////////////////////
	// 在已经求得的直线参数对向量中遍历
	for (size_t i = 0; i < lines.size(); i++)
	{
		float rhoCur = lines[i][0];
		float angleCur = lines[i][1];

		float rhoTotal = rhoCur;
		float angleTotal = angleCur;
		int numProche = 1;

		// 如果有其他的直线参数对和当前直线参数对相近，则求得他们的平均值赋给当前参数对，其他相近参数对置零
		for (size_t j = i + 1; j < lines.size(); j++)
		{	
			if ( (abs(lines[j][0] - rhoCur) < thresholdRho) && ( abs(lines[j][1] - angleCur) / CV_PI * 180 < thresholdAngle))
			{
				rhoTotal += lines[j][0];
				angleTotal += lines[j][1];
				numProche++;
				lines[j][0] = lines[j][1] = 0;
			}
		}
		
		lines[i][0] = rhoTotal / numProche;
		lines[i][1] = angleTotal / numProche;
	}

	/////////////////////////////////////////////////////////////////////////////////////////////
	//																						   //
	//								# 4 然后归并参数对相近的直线				     		   //
	//																						   //
	/////////////////////////////////////////////////////////////////////////////////////////////
	vector<Vec2f> finalLines;
	Vec2f tempLine;
	for (size_t i = 0; i < lines.size(); i++)
	{
		if ((lines[i][0] == 0) && (lines[i][1] == 0))	// 如果是已经被置零的参数对，则跳过
			continue;
		// 否则将该参数对添加到最终向量之中
		tempLine[0] = lines[i][0];
		tempLine[1] = lines[i][1];
		finalLines.push_back(tempLine);
	}

	lines.clear();	// 清空原向量
	lines = finalLines;	// 将最终项链赋给原向量
}

void czh_centerOfGravity(Mat & labelledImage, Mat & dstImage, vector<Point> & centerPoints)
{
	// 该函数针对已经被连通域标记处理过的图像寻找每个连通域的重心，输出至 dstImage 之中,并把重心坐标输出到 centerPoints 之中

	Mat tempImage(labelledImage.size(), CV_8UC1, Scalar::all(0));
	// 维持一个大小为256的矢量，每一个矢量元素（子矢量）是一个点矢量，用于存放像素灰度值相同的点
	vector<vector<Point>> pointsSet(256);	// 比如像素灰度为 1 的点都存放于 pointSet[1] 之中

	Point curPoint;	// 当前点Point对象
	int curValue;	// 当前点灰度值
	MatIterator_<uchar> itBegin, itEnd;	// 图像迭代器

	for (itBegin = labelledImage.begin<uchar>(), itEnd = labelledImage.end<uchar>(); itBegin != itEnd; itBegin++)
	{	// 遍历标记图像中每一个像素，将相同像素的点放进一个矢量
		if ((*itBegin) != 0)	// 如果当前点灰度值不为零
		{
			curValue = (*itBegin);	// 当前点灰度值
			curPoint = itBegin.pos();	// 当前点Point对象
			pointsSet[curValue].push_back(curPoint);	// 把当前点压入对应矢量中
		}
	}

	vector<Point> cornerCenterPointSet;	// 准备最终的交点点矢量

	for (int i = 0; i < pointsSet.size(); i++)
	{	// 遍历 pointsSet 的所有256个子矢量
		if (pointsSet[i].size() == 0)
		{	// 如果某个子矢量中一个点都没有，则跳过
			continue;
		}

		// 初始化 x,y 值总和
		int total_x = 0;
		int total_y = 0;;
		int total_points = pointsSet[i].size();// 该灰度值有多少个点

		for (int j = 0; j < pointsSet[i].size(); j++)
		{	// 遍历每一个子矢量中的点元素,求得x总值和y总值
			total_x += pointsSet[i][j].x;
			total_y += pointsSet[i][j].y;
		}

		// 求得该灰度值点群的重心
		int x = total_x / total_points;
		int y = total_y / total_points;
		cornerCenterPointSet.push_back(Point(x, y));	// 把该重心压入 cornerPointSet 矢量中,以备以后使用
		tempImage.at<uchar>(y, x) = 255;	// 把最终图像 Mat 对象中把角点置为白色
	}

	dstImage = tempImage;
	vector<Point>().swap(centerPoints);	// 清空输出向量
	centerPoints = cornerCenterPointSet;// 将点坐标向量赋给输出向量
}

void czh_bwAreaOpen(Mat & srcImage, Mat & dstImage, int zoneHeight, int zoneWidth, int threshold)
{
	// 	该函数实现了 Matlab 中 bwareaopen 函数的功能：对一个二值图像去噪，去除连通域面积小于 threshold 的点

	Mat tempImage, tempRectImage;
	srcImage.copyTo(tempImage);

	int numRows = srcImage.rows / zoneHeight;
	int numCols = srcImage.cols / zoneWidth;

	Point curPoint;	// 当前点Point对象
	int curValue;	// 当前点灰度值
	MatIterator_<uchar> itBegin, itEnd;	// 图像迭代器

	for (int i = 0; i < numRows; i++)
		for (int j = 0; j < numCols; j++)
		{
			// 针对每一个子块儿做连通域标记
			tempRectImage = srcImage(Rect(j*zoneWidth, i*zoneHeight, zoneWidth, zoneHeight));
			czh_labeling(tempRectImage, tempRectImage);

			// 维持一个大小为256的矢量，每一个矢量元素（子矢量）是一个点矢量，用于存放像素灰度值相同的点
			vector<vector<Point>> pointsSet(256);	// 比如像素灰度为 1 的点都存放于 pointSet[1] 之中

			for (itBegin = tempRectImage.begin<uchar>(), itEnd = tempRectImage.end<uchar>(); itBegin != itEnd; itBegin++)
			{	// 遍历标记图像中每一个像素，将相同像素的点放进一个矢量
				if ((*itBegin) != 0)	// 如果当前点灰度值不为零
				{
					curValue = (*itBegin);	// 当前点灰度值
					curPoint = itBegin.pos();	// 当前点Point对象
					pointsSet[curValue].push_back(curPoint);	// 把当前点压入对应矢量中
				}
			}

			for (int itPts = 0; itPts < pointsSet.size(); itPts++)
			{	// 遍历 pointsSet 的所有256个子矢量
				if (pointsSet[itPts].size() == 0)
				{	// 如果某个子矢量中一个点都没有，则跳过
					continue;
				}

				if (pointsSet[itPts].size() < threshold)
				{	// 如果某个连通域面积小于阈值，则把该连通域上的点置为黑色
					for (int nPts = 0; nPts < pointsSet[itPts].size(); nPts++)
						tempImage.at<uchar>(pointsSet[itPts][nPts].y + i*zoneHeight, pointsSet[itPts][nPts].x + j*zoneWidth) = 0;
				}
			}
		}
	dstImage = tempImage;
}

void czh_guidedFilter(Mat & srcImage, Mat & guidanceImage, Mat & dstImage, int radius, float epsilon, int outputType)
{
	// 该函数实现了何凯明博士的 guided filter 引导滤波算法
	CV_Assert(radius >= 2 && epsilon > 0);
	CV_Assert(srcImage.data != NULL && srcImage.channels() == 1);
	CV_Assert(guidanceImage.channels() == 1);
	CV_Assert(srcImage.rows == guidanceImage.rows && srcImage.cols == guidanceImage.cols);

	// 开始算法
	Mat P, I;

	// 将源图像和引导图像转换为浮点型: 以便于后边的除法运算
	srcImage.convertTo(I, CV_32F, 1.0 / 255.0);	// 引导图像 I = guidance image
	srcImage.convertTo(P, CV_32F, 1.0 / 255.0);	// 原图像 P = source image

	//	计算I*p和I*I  
	Mat image_IP, image_II;
	multiply(P, I, image_IP);	// image_IP = image I * image P
	multiply(I, I, image_II);	// image_I2 = image I * image I

	// 滤波窗口大小
	Size window_size(2 * radius + 1, 2 * radius + 1);

	//计算各种均值  
	Mat mean_I, mean_P, corr_I, corr_IP;
	boxFilter(I, mean_I, CV_32F, window_size);	// meanI = mean of input image I
	boxFilter(P, mean_P, CV_32F, window_size);	// meanP = mean of guidance image P
	boxFilter(image_II, corr_I, CV_32F, window_size);	// corr_I = mean of image_I2
	boxFilter(image_IP, corr_IP, CV_32F, window_size);	// corr_IP = mean of image_IP

	//计算Ip的协方差和I的方差  
	Mat var_I, cov_IP;
	var_I = corr_I - mean_I.mul(mean_I);
	cov_IP = corr_IP - mean_I.mul(mean_P);

	// 求 a,b
	Mat a, b;
	var_I += epsilon;
	divide(cov_IP, var_I, a);
	b = mean_P - a.mul(mean_I);

	//对包含像素i的所有a、b做平均  
	Mat mean_a, mean_b;
	boxFilter(a, mean_a, CV_32F, window_size);
	boxFilter(b, mean_b, CV_32F, window_size);

	// 做最终的计算
	Mat tempDstImage(I.size(), CV_32F);
	tempDstImage = mean_a.mul(I) + mean_b;

	dstImage = tempDstImage;	// 将最终结果以 dstImage 参数返回
	if (outputType == CV_8UC1)
	{
		dstImage.convertTo(dstImage, CV_8UC1, 255);	// 转换格式
	}
	else if ((outputType == CV_32FC1) || (outputType == CV_32FC1)){}
}

void czh_findValley(Mat & srcImage, Mat & dstImage, int valleyRadius, int valleyThreshold1, int valleyThreshold2)
{
	// 该函数查找图像中的 valley 像素

	CV_Assert(srcImage.channels() == 1 && dstImage.channels() == 1);
	CV_Assert(valleyRadius >= 1);
	CV_Assert(valleyThreshold1 >= 0 && valleyThreshold2 <= 255 && valleyThreshold1 <= valleyThreshold2);

	int curValue;
	bool jumpThisPixel_Horizontal, jumpThisPixel_rVertical;
	uchar *ptr_u_src, *ptr_u_dst;
	Mat tempImage(srcImage.size(), CV_8UC1, Scalar::all(0));

	for (int i = valleyRadius; i < srcImage.rows - valleyRadius; i++)	// 遍历每一个允许范围内的像素
	{
		ptr_u_src = srcImage.ptr(i);	// 当前源图像像素行指针
		ptr_u_dst = tempImage.ptr(i);	// 当前目标图像像素行指针

		for (int j = valleyRadius; j < srcImage.cols - valleyRadius; j++)// 遍历每一个允许范围内的像素
		{
			if (ptr_u_src[j] < valleyThreshold1 || ptr_u_src[j] > valleyThreshold2)	// 如果当前像素灰度值比阈值小，则跳过该像素
				continue;

			curValue = ptr_u_src[j];			// 当前点像素值
			jumpThisPixel_Horizontal = false;	// 跳过此水平像素判别符
			jumpThisPixel_rVertical = false;	// 跳过此垂直像素判别符

			////////////////////////////////////////
			//   horizontal 水平方向查找 valley   //
			////////////////////////////////////////
			// 横向遍历左右一定范围内像素，如果有比当前像素更小的像素，则当前像素不是 valley
			for (int ptr = -valleyRadius; ptr <= valleyRadius; ptr++)
				if (ptr_u_src[j + ptr] < curValue)
				{
					jumpThisPixel_Horizontal = true;
					break;
				}

			// 如果没有跳过，则说明该点是横向局部最小值，则在目标图像上置为白色
			if (jumpThisPixel_Horizontal == false)
				ptr_u_dst[j] = 255;

			//////////////////////////////////////
			//   vertical 垂直方向查找 valley   //
			//////////////////////////////////////
			// 纵向遍历左右一定范围内像素，如果有比当前像素更小的像素，则当前像素不是 valley
			for (int ptr = -valleyRadius; ptr <= valleyRadius; ptr++)
				if (srcImage.at<uchar>(i + ptr, j) < curValue)
				{
					jumpThisPixel_rVertical = true;
					break;
				}

			// 如果没有跳过，则说明该点是纵向局部最小值，则在目标图像上置为白色
			if (jumpThisPixel_rVertical == false)
				tempImage.at<uchar>(i, j) = 255;
		}
	}
	dstImage = tempImage;
}