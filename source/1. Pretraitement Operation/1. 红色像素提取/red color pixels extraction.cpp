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
	cout << "\t\t该程序会抽取图像中的红色像素信息。\n\n";
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
	Mat dst(srcImage.size(), CV_8UC1);
	czh_extractColor(srcImage, dst, RED);
	czh_imwrite(dst, fileName);
	
	// 程序结束信息：报告程序所花时间
	time = ((double)getTickCount() - time) / getTickFrequency();
	cout << "Program finished:\t" << time << " second used.\n" << endl;
	waitKey();
	system("pause");
}