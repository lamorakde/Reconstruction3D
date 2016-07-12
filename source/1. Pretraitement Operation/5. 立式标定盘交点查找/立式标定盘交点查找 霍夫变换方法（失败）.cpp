#include "opencv2/opencv.hpp"
#include <iostream>
#include <windows.h>
#include <fstream>
#include <math.h>
#include "czh_cv.h"

using namespace std;
using namespace cv;

#define WINDOW_NAME "霍夫变换"
#define WINDOW_NAME_BW "阈值分割"
#define WINDOW_NAME_VALLEY "Valley Detection"

Mat srcImage, lineImage, bwSrcImage, bwResultImage;

Mat g_houghSrcImage, g_houghResultImage;
int g_bwThreshold;
int g_bwThresholdMax = 255;
int g_thresholdHough;
int g_thresholdHoughMax = 150;
int g_thresholdRho;
int g_thresholdRhoMax = 15;
int g_thresholdAngle;
int g_thresholdAngleMax = 15;

Mat g_valleySrcImage, g_valleyResultImage;
int g_valleyRadius = 1, g_valleyRadiusMax = 50;
int g_valleyThreshold1 = 1, g_valleyThreshold2 = 255, g_valleyThresholdMax = 255;

void on_Trackbar_Threshold(int, void*)
{
	threshold(bwSrcImage, bwResultImage, g_bwThreshold, 255, 0);
	imshow(WINDOW_NAME_BW, bwResultImage);
}

void on_Trackbar_Hough(int, void*)
{
	// 将得到的边缘图像进行霍夫线变换，将得到的线参数放置于 lineVector 向量之中
	vector<Vec2f> lineVector;
	Mat houghSrcImage = g_houghSrcImage;
	czh_myHoughLines(houghSrcImage, lineVector, 1, CV_PI / 180, g_thresholdHough, g_thresholdRho, g_thresholdAngle);

	// 在 lineImage 之中绘制侦测到的线
	Mat tempLineImage(houghSrcImage.size(), CV_8UC1, Scalar::all(0));	// 后续操作需要的线图像
	Mat lineShowImage(houghSrcImage.size(), CV_8UC3, Scalar::all(0));// 直观观察的线图像+二值图像

																	 // 将二值图像的值赋给一个三通道图像 lineShowImage，以便于绘制红色线条
	MatIterator_<Vec3b> itVec3bBegin, itVec3bEnd;
	MatIterator_<uchar> itUcharBegin, itUcharEnd, itDst, itSrc;
	for (itVec3bBegin = lineShowImage.begin<Vec3b>(), itVec3bEnd = lineShowImage.end<Vec3b>(), itSrc = houghSrcImage.begin<uchar>(); itVec3bBegin != itVec3bEnd; itVec3bBegin++, itSrc++)
		(*itVec3bBegin)[0] = (*itVec3bBegin)[1] = (*itVec3bBegin)[2] = (*itSrc);

	for (size_t i = 0; i < lineVector.size(); i++)
	{
		float rho = lineVector[i][0], theta = lineVector[i][1];
		Point pt1, pt2;
		double a = cos(theta), b = sin(theta);
		double x0 = a*rho, y0 = b*rho;
		pt1.x = cvRound(x0 + 1000 * (-b));
		pt1.y = cvRound(y0 + 1000 * (a));
		pt2.x = cvRound(x0 - 1000 * (-b));
		pt2.y = cvRound(y0 - 1000 * (a));
		line(tempLineImage, pt1, pt2, Scalar(255, 255, 255), 1, 8); // 在线条图像中绘制侦测到的线条（白色）
		line(lineShowImage, pt1, pt2, Scalar(0, 0, 255), 1, CV_AA);	// 在二值图像中绘制侦测到的线条（红色）
	}

	cv::imshow(WINDOW_NAME, lineShowImage);	// 显示线检测结果
	lineImage = tempLineImage;	// 用于后边计算的线图像
}

void on_Trackvar_Valley(int, void *)
{
	if (g_valleyThreshold2 < g_valleyThreshold1)
		g_valleyThreshold2 = g_valleyThreshold1;

	czh_findValley(g_valleySrcImage, g_valleyResultImage, g_valleyRadius, g_valleyThreshold1, g_valleyThreshold2);
	imshow(WINDOW_NAME_VALLEY, g_valleyResultImage);
}
int main()
{
	// 程序帮助信息
	string programInformation = "该程序会使用霍夫线变换方法自动寻找立式标定图中的角点.";
	string fileType = ".ppm";
	czh_helpInformation(programInformation);

	// 得到输入图像名称信息并输出
	std::cout << "Enter the input image name without " << fileType << ": ";
	string fileName, srcFileName;
	getline(cin, fileName);				// 获得输入文件名
	srcFileName = fileName + fileType;	// 确定输入图片文件类型

	Mat src = imread(srcFileName, 0);	// 读取源图像

	czh_imageOpenDetect(src, fileName, fileType);	// 检测源图像是否成功打开
	// 显示源图像信息
	std::cout << "\nInput image name:\t" << srcFileName << endl
		<< "Image size:\t\t" << src.cols << "*" << src.rows << endl
		<< "Image pixels:\t\t" << src.cols*src.rows << endl
		<< "Image type:\t\t" << src.type() << endl;

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//																							    										//
	//													图像预处理																		    //
	//																															    		//
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	Mat grayImage;
	src.copyTo(grayImage);
	imshow("gray scale image", grayImage);

	// 显示 profile
	bool showProfile = false;
	if (showProfile == true)
	{
		Point p0(185, 576);
		Point p1(337, 377);
		Mat profileImage = czh_Line_Profile(grayImage, p0, p1, false);
		cv::imshow("srcImage", grayImage);
		// czh_imwrite(finalImage, "profile");
		waitKey();
	}

	// 时钟信息，记录程序算法使用时间
	double timeTotal = static_cast<double>(getTickCount());
	double time = static_cast<double>(getTickCount());

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//																																				  //
	//															开始算法运算																		  //
	//																																				  //
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	src.copyTo(srcImage);
	Mat blurredImage;
	GaussianBlur(srcImage, blurredImage, Size(3, 3), 0, 0);
	
	//...............................................//
	//												 //
	//				       STEP.1                    //
	//			      Valley Detection	     	   	 //
	//...............................................//

	namedWindow(WINDOW_NAME_VALLEY);
	srcImage.copyTo(g_valleySrcImage);
	createTrackbar("Radius", WINDOW_NAME_VALLEY, &g_valleyRadius, g_valleyRadiusMax, on_Trackvar_Valley);
	createTrackbar("Low Threshold", WINDOW_NAME_VALLEY, &g_valleyThreshold1, g_valleyThresholdMax, on_Trackvar_Valley);
	createTrackbar("High Threshold", WINDOW_NAME_VALLEY, &g_valleyThreshold2, g_valleyThresholdMax, on_Trackvar_Valley);
	waitKey();

	//...............................................//
	//												 //
	//				       STEP.2                    //
	//			      霍夫变换检测直线	     	   	 //
	//												 //
	//...............................................//

	// 滑动条选择最佳霍夫线阈值：开始
	g_houghResultImage.copyTo(g_houghSrcImage);
	namedWindow(WINDOW_NAME);
	g_thresholdHough = 130;
	g_thresholdAngle = 0;
	g_thresholdRho = 0;
	createTrackbar("Hough Threshold", WINDOW_NAME, &g_thresholdHough, g_thresholdHoughMax, on_Trackbar_Hough);
	createTrackbar("Angle", WINDOW_NAME, &g_thresholdAngle, g_thresholdAngleMax, on_Trackbar_Hough);
	createTrackbar("Rho", WINDOW_NAME, &g_thresholdRho, g_thresholdRhoMax, on_Trackbar_Hough);
	// 滑动条选择最佳霍夫线阈值：结束

	// 按下任意键继续下边的操作
	waitKey(0);

	//...............................................//
	//												 //
	//				       STEP.3                    //
	//			    根据直线图像检测交点	   	   	 //
	//												 //
	//...............................................//

	time = static_cast<double>(getTickCount());
	Mat potentialPointImage(srcImage.size(), CV_8UC1, Scalar::all(0));

	uchar *rowCur, *rowTop, *rowBot, *ptrDst;	// 当前行，上一行，下一行的行指针
	for (int i = 1; i < lineImage.rows - 1; i++)
	{	// 遍历得到的线图像每个像素，寻找可能是交点的点
		rowTop = lineImage.ptr<uchar>(i - 1);
		rowCur = lineImage.ptr<uchar>(i);
		rowBot = lineImage.ptr<uchar>(i + 1);
		ptrDst = potentialPointImage.ptr<uchar>(i);
		for (int j = 1; j < lineImage.cols - 1; j++)	// 遍历 lineImage 中每一个像素
			if (rowCur[j] == 255)	// 如果当前像素是前景像素
			{	// 如果它8个邻域中有且只有4个非零像素，则可能是交点
				if ((int)(rowCur[j - 1] + rowCur[j + 1] + rowTop[j - 1] + rowTop[j] + rowTop[j + 1] + rowBot[j - 1] + rowBot[j] + rowBot[j + 1]) == 1020)
					ptrDst[j] = 255;
			}
	}
	// 对已经找到的交点图像做连通域标记，以便于下一步的重心查找
	Mat labelledPointImage;
	czh_labeling(potentialPointImage, labelledPointImage);

	time = ((double)getTickCount() - time) / getTickFrequency();
	cout << "STEP.2 交点初步判定+连通域标记 finished :\t" << time << " second used.\n" << endl;

	//...............................................//
	//												 //
	//				     STEP.4                      //
	//					寻找重心					 //
	//												 //
	//...............................................//

	time = static_cast<double>(getTickCount());

	Mat dstImage(srcImage.size(), CV_8UC1, Scalar::all(0));
	vector<Point> centerPoints;
	czh_centerOfGravity(labelledPointImage, dstImage, centerPoints);	// 寻找重心

	time = ((double)getTickCount() - time) / getTickFrequency();
	cout << "STEP.2 交点初步判定+连通域标记 finished :\t" << time << " second used.\n" << endl;

	//...............................................//
	//												 //
	//				     STEP.5                      //
	//				   输出点坐标					 //
	//												 //
	//...............................................//

	// 把角点坐标输出为 txt 文件
	ofstream ffout;
	string outtFileName = "result " + fileName + ".txt";
	ffout.open(outtFileName);
	for (int i = 0; i < centerPoints.size(); i++)
	{
		ffout << centerPoints[i].x << ",\t" << centerPoints[i].y << endl;
	}
	ffout.close();

	//////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////////////

	// 程序结束信息：报告程序所花时间
	timeTotal = ((double)getTickCount() - timeTotal) / getTickFrequency();
	std::cout << "Program finished :\t" << timeTotal << " second used.\n" << endl;

	// 测试区域
	imshow("blur image", blurredImage);
	imshow("lineImage", lineImage);

	cv::waitKey();
	std::system("pause");
}