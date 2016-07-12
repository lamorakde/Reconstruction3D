#include "czh_binary_CV.h"
#include "opencv2/opencv.hpp"
#include <iostream>

using namespace std;
using namespace cv;

int main()
{
	// 程序帮助信息
	string programInformation = "输入一张三通道图像，该程序会先抽取红色像素点，然后对图像做膨胀以保证弥补断点，然后抽取像素图像的骨架.";
	string fileType = ".ppm";
	czh_helpInformation(programInformation);

	// 得到输入图像名称信息并输出
	cout << "Enter the input image name without " << fileType << ": ";
	string fileName, srcFileName;
	getline(cin, fileName);				// 获得输入文件名
	srcFileName = fileName + fileType;	// 确定输入图片文件类型
	Mat srcImage = imread(srcFileName);	// 读取源图像
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
	// 准备过渡 Mat 对象
	Mat dstImage(srcImage.size(), CV_8UC1);
	Mat redPixelsImage(srcImage.size(), CV_8UC1);
	Mat skeletonImage(srcImage.size(), CV_8UC1);
	// 抽取红色像素点
	czh_extractColor(srcImage, redPixelsImage, RED);
	// 对于抽取出来的红色像素的二值图像做骨骼提取
	czh_thin_LUT(redPixelsImage, skeletonImage);
	// 复制骨骼提取图像到目标输出图像 Mat 对象
	skeletonImage.copyTo(dstImage);
	// 输出图像文件
	czh_imwrite(dstImage, fileName);


	//////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////////////
	// 程序结束信息：报告程序所花时间
	time = ((double)getTickCount() - time) / getTickFrequency();
	cout << "Program finished first:\t" << time << " second used.\n" << endl;

	// 测试区域
	imshow("源图", srcImage);
	imshow("效果图", dstImage);

	waitKey();
	system("pause");
}
