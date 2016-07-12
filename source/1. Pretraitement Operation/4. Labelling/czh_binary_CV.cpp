#include "opencv2/opencv.hpp"
#include <windows.h>

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
	// 该函数输入一个单通道图像，做最小值滤波，窗口大小 size * size
// 	if ( (srcImage.type() != CV_32FC1) || (srcImage.type() != CV_8UC1))
// 	{
// 		cerr << "输入图像必须为单通道图像.\n";
// 		return;
// 	}
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

// 	while (x < y)
// 	{
// 		pointsDirection[0].push_back(Point(x0 + x, y0 + y));
// 		pointsDirection[1].push_back(Point(x0 + y, y0 + x));// 顺序不正确
// 		pointsDirection[6].push_back(Point(x0 - y, y0 + x));
// 		pointsDirection[7].push_back(Point(x0 - x, y0 + y));// 顺序不正确
// 		pointsDirection[4].push_back(Point(x0 - x, y0 - y));
// 		pointsDirection[5].push_back(Point(x0 - y, y0 - x));// 顺序不正确
// 		pointsDirection[2].push_back(Point(x0 + y, y0 - x));
// 		pointsDirection[3].push_back(Point(x0 + x, y0 - y));// 顺序不正确
// 
// 		// 判断下一次的点坐标
// 		if (distance < 0)
// 		{
// 			distance += 4 * x + 6;
// 		}
// 		else
// 		{
// 			distance += 4 * (x - y) + 10;
// 			y--;
// 		}
// 		x++;
// 	}


	pointsDirection[0].push_back(Point(x0 + x, y0 + y));
	pointsDirection[1].push_back(Point(x0 + y, y0 + x));
	pointsDirection[6].push_back(Point(x0 - y, y0 + x));
	pointsDirection[7].push_back(Point(x0 - x, y0 + y));
	pointsDirection[4].push_back(Point(x0 - x, y0 - y));
	pointsDirection[5].push_back(Point(x0 - y, y0 - x));
	pointsDirection[2].push_back(Point(x0 + y, y0 - x));
	pointsDirection[3].push_back(Point(x0 + x, y0 - y));

	x++;

	while (x < y)
	{
		if (distance < 0)
		{
			distance += 4 * x + 6;
		}
		else
		{
			distance += 4 * (x - y) + 10;
			y--;
		}
		pointsDirection[0].push_back(Point(x0 + x, y0 + y));	
		pointsDirection[1].push_back(Point(x0 + y, y0 + x));// 顺序不正确
		pointsDirection[6].push_back(Point(x0 - y, y0 + x));	
		pointsDirection[7].push_back(Point(x0 - x, y0 + y));// 顺序不正确
		pointsDirection[4].push_back(Point(x0 - x, y0 - y));	
		pointsDirection[5].push_back(Point(x0 - y, y0 - x));// 顺序不正确
		pointsDirection[2].push_back(Point(x0 + y, y0 - x));	
		pointsDirection[3].push_back(Point(x0 + x, y0 - y));// 顺序不正确

		x++;
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
		for (int j = 0; j < pointsDirection[i].size(); j++)
		{
			pointsOfCircle.push_back(pointsDirection[i][j]);
		}
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
				while (preLabel != curLabel) {
					curLabel = preLabel;
					preLabel = labelSet[preLabel];
				}
				labelSet[i] = curLabel;
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

