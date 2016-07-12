#include <iostream>
#include <fstream>
#include "opencv2/opencv.hpp"

using namespace std;
using namespace cv;

void CZH_endPoint(Mat & src_skeleton_Image, Mat & dst_endPoint_Image);
void CZH_writeWhitePixel(Mat & srcImage, vector<Point2f> & dstWhitePoints);

int main()
{
	// 获取输入文件名
	cout << "This program is going to extract the end points of a skeleton image\n\n"
		<< "Please enter the input image name without .pgm:  ";
	string fileName, srcFileName, outputImageName;
	getline(cin, fileName);
	outputImageName = fileName + "_endPoints.pgm";
	srcFileName = fileName + ".pgm";
	
	// 获取源图像，并进行端点寻找
	Mat srcImage = imread(srcFileName, 0);
	Mat dstImage(srcImage.size(), srcImage.type(), Scalar(0));
	CZH_endPoint(srcImage, dstImage);

	// 输出图像
	imwrite(outputImageName, dstImage);

	// 输出白色端点像素坐标
	vector<Point2f> whitePoints;				// 用于存放白色像素的矢量
	CZH_writeWhitePixel(dstImage, whitePoints);	// 调用函数，返回白色像素坐标并存于矢量中
	string outputTextName = fileName + ".txt";	// 准备输出存有像素坐标的文本文件
	ofstream outFile;
	outFile.open(outputTextName);
	for (int i = 0; i < whitePoints.size(); i++)// 输出数据到 txt 文件
	{
		outFile << whitePoints[i].x << ",\t" << whitePoints[i].y << endl;
	}
	outFile.close();

	waitKey();
	system("pause");
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

void CZH_writeWhitePixel(Mat & srcImage, vector<Point2f> & dstWhitePoints)
{
	// 该函数输入一幅二值图像，它将找出该图像中的白色像素
	// 并且将这些像素坐标存于输入参数矢量 vector<Point2f> & dstWhitePoints 之中
	Mat src = srcImage;

	// 清除输入参数矢量中原有的数据并释放空间
	vector<Point2f>().swap(dstWhitePoints);

	Point2f curPoint;	//	用于遍历图像的当前点
	
	for (int i = 0; i < src.rows; i++)
	{
		for (int j = 0; j < src.cols ; j++)
		{
			// 如果该点是白色，则把它的 x,y 赋给当前点数据，并将该店推进矢量中
			if (src.at<uchar>(i,j) == 255)
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
		cout << "\t#" << i+1 << ":\t" << dstWhitePoints[i].x << ",\t" << dstWhitePoints[i].y << endl;
	}
}