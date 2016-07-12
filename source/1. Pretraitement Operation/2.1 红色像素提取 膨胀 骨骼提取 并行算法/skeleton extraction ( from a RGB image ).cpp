#include "czh_binary_CV.h"
#include <opencv2/core.hpp>
#include <iostream>
#include <fstream>
using namespace std;
using namespace cv;

int main()
{
	// 程序信息：程序功能
	cout << "************************************************************************************************************************\n";
	cout << "\t\t\tProgram information:\n";
	cout << "\t\t输入一张三通道图像，该程序会先抽取红色像素点，然后对图像做膨胀以保证弥补断点，然后抽取像素图像的骨架\n\n";
	cout << "************************************************************************************************************************\n\n";
	// 得到输入图像名称信息并输出
	cout << "Enter the input image name:";
	string fileName, srcFileName;
	getline(cin, fileName);
	srcFileName = fileName + ".ppm";
	Mat srcImage = imread(srcFileName);
	cout << "Input image name:\t" << srcFileName << endl
		 << "Image size:\t\t" << srcImage.cols << "*" << srcImage.rows << endl
		 << "Image pixels:\t\t" << srcImage.cols*srcImage.rows << endl
		 << "Image type:\t\t" << srcImage.type() << endl;
	// 时钟信息，记录程序算法使用时间
	double time = static_cast<double>(getTickCount());

	// 开始算法运算
	// 准备过渡 Mat 对象
	Mat dst(srcImage.size(), CV_8UC1);
	Mat redPixelsImage(srcImage.size(), CV_8UC1); 
	Mat dilatedImage(srcImage.size(), CV_8UC1);
	Mat skeletonImage(srcImage.size(), CV_8UC1);
	// 抽取红色像素点
	czh_extractColor(srcImage, redPixelsImage, RED);
	// 对于抽取出来的红色像素图像做膨胀，以弥补断点
	dilate(redPixelsImage, dilatedImage, getStructuringElement(MORPH_RECT, Size(3, 3), Point(-1, -1)), Point(-1, -1),3);
	// 对于膨胀之后的二值图像做骨骼提取
	czh_skeleton(dilatedImage, skeletonImage);
	// 复制骨骼提取图像到目标输出图像 Mat 对象
	skeletonImage.copyTo(dst);
	// 输出图像文件
	czh_imwrite(dst, fileName);
	
	// 程序结束信息：报告程序所花时间
	time = ((double)getTickCount() - time) / getTickFrequency();
	cout << "Program finished:\t" << time << " second used.\n" << endl;
	waitKey();
	system("pause");
}