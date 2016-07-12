#include "czh_binary_CV.h"
#include "opencv2/opencv.hpp"
#include <iostream>
#include <vector>

using namespace std;
using namespace cv;

int main()
{
	// 程序帮助信息
	string programInformation = "该程序会为二值图像做连通域标记操作: two - pass method.";
	string fileType = ".ppm";
	czh_helpInformation(programInformation);

	// 得到输入图像名称信息并输出
	cout << "Enter the input image name without " << fileType << ": ";
	string fileName, srcFileName;
	getline(cin, fileName);				// 获得输入文件名
	srcFileName = fileName + fileType;	// 确定输入图片文件类型
	Mat srcImage = imread(srcFileName, 0);	// 读取源图像
	czh_imageOpenDetect(srcImage, fileName, fileType);	// 检测源图像是否成功打开
	// 显示源图像信息
	cout << "\nInput image name:\t" << srcFileName << endl
		<< "Image size:\t\t" << srcImage.cols << "*" << srcImage.rows << endl
		<< "Image pixels:\t\t" << srcImage.cols*srcImage.rows << endl
		<< "Image type:\t\t" << srcImage.type() << endl;

	// 时钟信息，记录程序算法使用时间
	double time = static_cast<double>(getTickCount());

	//////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////////////
	// 开始算法运算
	
	
	imshow("bn", srcImage);


	Mat dstImage(srcImage.size(), CV_8UC1);
	czh_labeling(srcImage, dstImage);


	
	//////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////////////

	// 程序结束信息：报告程序所花时间
	time = ((double)getTickCount() - time) / getTickFrequency();
	cout << "Program finished first:\t" << time << " second used.\n" << endl;

	// 测试区域
	imshow("srcImage", srcImage);
	imshow("dstImage", dstImage);
	czh_imwrite(dstImage, "labeling");

	waitKey();
	system("pause");
}