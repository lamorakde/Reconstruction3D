#include "opencv2/opencv.hpp"
using namespace cv;
using namespace std;

static enum { RED, GREEN, BLUE, WHITE, BLACK } color;

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
	LUT(tmpImage, lookUpTable, dst_bw_Image);
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

void czh_skeleton(Mat & srcImage, Mat & dstImage, int iterations = 10)
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