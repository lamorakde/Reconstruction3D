#include "opencv2/opencv.hpp"
#include <iostream>
#include <windows.h>
#include <fstream>
#include <math.h>
#include "czh_binary_CV.h"

using namespace std;
using namespace cv;

#define WINDOW_NAME "霍夫变换"
#define WINDOW_NAME_BW "阈值分割"

Mat srcImage, lineImage, bwImage;
int g_bwThreshold;
int g_bwThresholdMax = 255;
int g_thresholdHough;
int g_thresholdHoughMax = 150;
int g_thresholdRho;
int g_thresholdRhoMax = 15;
int g_thresholdAngle;
int g_thresholdAngleMax = 15;

void on_Trackbar_Threshold(int, void*)
{
	threshold(srcImage, bwImage, g_bwThreshold, 255, 0);
	imshow(WINDOW_NAME_BW, bwImage);
}

void on_Trackbar_Hough(int, void*)
{
	// 将得到的边缘图像进行霍夫线变换，将得到的线参数放置于 lineVector 向量之中
	vector<Vec2f> lineVector;
	Mat houghSrcImage = srcImage;
	czh_myHoughLines(houghSrcImage, lineVector, 1, CV_PI / 180,g_thresholdHough,g_thresholdRho,g_thresholdAngle);

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

int main()
{
	// 程序帮助信息
	string programInformation = "该程序会使用霍夫线变换方法自动寻找立式标定图中的角点.";
	string fileType = ".ppm";
	czh_helpInformation(programInformation);

	// 得到输入图像名称信息并输出
	std::cout << "Enter the input image name without " << fileType << ": ";
	string fileName, srcFileName;
	fileName = "image 2";
	// getline(cin, fileName);				// 获得输入文件名
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
	src.copyTo(srcImage);
// 	imshow("gray scale image", srcImage);
// 
// 	// 显示 profile
// 	bool showProfile = true;
// 	if (showProfile == true)
// 	{
// 		Point p0(185, 576);
// 		Point p1(337, 377);
// 		Mat profileImage = czh_Line_Profile(srcImage, p0, p1);
// 		cv::imshow("srcImage", srcImage);
// 		// czh_imwrite(finalImage, "profile");
// 		waitKey();
// 	}
	

	// 时钟信息，记录程序算法使用时间
	double timeTotal = static_cast<double>(getTickCount());
	double time = static_cast<double>(getTickCount());

	uchar *ptr_uDst;	// uchar 类型指针
	float total = 0;
	double maxValue, minValue;
	int high = 255;
	int low = 180;
	//...............................................//
	//												 //
	//				       STEP.1                    //
	//				      Vertical			   	   	 //
	//												 //
	//...............................................//
	
	///////////////////////////////////////////////////
	//		# 1 模板矩阵以及相关参数
	///////////////////////////////////////////////////
 	int M_vertical[7][3] = { { high,low,high },{ high,low,high },{ high,low,high },{ high,low,high },{ high,low,high },{ high,low,high },{ high,low,high } };
 	
 	for (int i = 0; i < 7; i++)
 		for (int j = 0; j < 3; j++)
 		{
 			total += M_vertical[i][j];
 		}
 	float mean_M = total / 21;	// M_vertical 的均值
 
 	total = 0;
 	for (int i = 0; i < 7; i++)
 		for (int j = 0; j < 3; j++)
 		{
 			total += (M_vertical[i][j] - mean_M)*(M_vertical[i][j] - mean_M);
 		}
 
 	float var_M = total / 20;		// M_vertical 的方差var，这里分母是 21 - 1
 	float sigma_M = sqrt(var_M);	// M_vertical 的标准差 sigma

	/////////////////////////////////////////////////
	//		# 2 在原灰度图中嵌套模板进行计算
	/////////////////////////////////////////////////

	// 创建并初始化 rho 向量，它与图像同样的尺寸
 	vector<vector<double>> rhoVertical(srcImage.rows);
 	for (int i = 0; i < srcImage.rows; i++)
 		for (int j = 0; j < srcImage.cols; j++)
 		{
 			rhoVertical[i].push_back(0);
 		}
 
 	for (int i = 3; i < srcImage.rows - 3;i++)
 		for (int j = 1; j < srcImage.cols - 1; j++)
 		{
 			total = 0;
 			for (int win_row = -3; win_row <= 3; win_row++)
 				for (int win_cols = -1; win_cols <= 1; win_cols++)
 				{
 					total += srcImage.at<uchar>(i + win_row, j + win_cols);
 				}
 			// 当前 I 上的点套用模板之下的平均值
 			float mean_I = total / 21;
 			
 			// 当前 I 上的点套用模板之下的方差和标准差
 			total = 0;
 			for (int win_row = -3; win_row <= 3; win_row++)
 				for (int win_cols = -1; win_cols <= 1; win_cols++)
 				{
 					total += (srcImage.at<uchar>(i + win_row, j + win_cols) - mean_I)*(srcImage.at<uchar>(i + win_row, j + win_cols) - mean_I);
 				}
 			float var_I = total / 20;	// 这里分母是 21 - 1
 			float sigma_I = sqrt(var_I);
 			
 			if (sigma_I == 0)
 				continue;
 
 			total = 0;
 			for (int win_row = -3; win_row <= 3; win_row++)
 				for (int win_cols = -1; win_cols <= 1; win_cols++)
 				{
 					total += (srcImage.at<uchar>(i + win_row, j + win_cols) - mean_I)*(M_vertical[win_row + 3][win_cols + 1] - mean_M);
 				}
 			rhoVertical[i][j] = total / (sigma_I*sigma_M);
 		}
 
 	// 寻找 rhoVertical 中最大最小值
	maxValue = rhoVertical[3][1];
	minValue = rhoVertical[3][1];
 	for (int i = 3; i < srcImage.rows - 3; i++)
 		for (int j = 1; j < srcImage.cols - 1; j++)
 		{
 			if (rhoVertical[i][j] > maxValue)
 				maxValue = rhoVertical[i][j];
 			if (rhoVertical[i][j] < minValue)
 				minValue = rhoVertical[i][j];
 		}
 
 	// 归一化
 	Mat outputVertical(srcImage.size(), CV_8UC1, Scalar::all(0));
 	for (int i = 3; i < srcImage.rows - 3; i++)
 	{
 		ptr_uDst = outputVertical.ptr(i);
 		for (int j = 1; j < srcImage.cols - 1; j++)
 		{
 			double cur = (rhoVertical[i][j] - minValue) / (maxValue - minValue);
 			if (cur < 0)
 				cur = 0;
 			ptr_uDst[j] = cvRound(255 * cur);
 		}
 	}
 	imshow("output Vertical", outputVertical);

	//...............................................//
	//												 //
	//				       STEP.2                    //
	//				     Horizontal		    	   	 //
	//												 //
	//...............................................//
	
	///////////////////////////////////////////////////
	//		# 1 模板矩阵以及相关参数
	///////////////////////////////////////////////////

	int M_horizontal[3][7] = { { high,high,high,high,high,high,high },{ low,low,low,low,low,low,low },{ high,high,high,high,high,high,high } };

	///////////////////////////////////////////////////
	//		# 2 在原灰度图中嵌套模板进行计算
	///////////////////////////////////////////////////

	// 创建并初始化 rho 向量，它与图像同样的尺寸
	vector<vector<double>> rhoHorizontal(srcImage.rows);
	for (int i = 0; i < srcImage.rows; i++)
		for (int j = 0; j < srcImage.cols; j++)
		{
			rhoHorizontal[i].push_back(0);
		}

	for (int i = 1; i < srcImage.rows - 1; i++)
		for (int j = 3; j < srcImage.cols - 3; j++)
		{
			total = 0;
			for (int win_row = -1; win_row <= 1; win_row++)
				for (int win_cols = -3; win_cols <= 3; win_cols++)
				{
					total += srcImage.at<uchar>(i + win_row, j + win_cols);
				}
			// 当前 I 上的点套用模板之下的平均值
			float mean_I = total / 21;

			// 当前 I 上的点套用模板之下的方差和标准差
			total = 0;
			for (int win_row = -1; win_row <= 1; win_row++)
				for (int win_cols = -3; win_cols <= 3; win_cols++)
				{
					total += (srcImage.at<uchar>(i + win_row, j + win_cols) - mean_I)*(srcImage.at<uchar>(i + win_row, j + win_cols) - mean_I);
				}
			float var_I = total / 20;	// 这里分母是 21 - 1
			float sigma_I = sqrt(var_I);

			if (sigma_I == 0)
				continue;

			total = 0;
			for (int win_row = -1; win_row <= 1; win_row++)
				for (int win_cols = -3; win_cols <= 3; win_cols++)
				{
					total += (srcImage.at<uchar>(i + win_row, j + win_cols) - mean_I)*(M_horizontal[win_row + 1][win_cols + 3] - mean_M);
				}
			rhoHorizontal[i][j] = total / (sigma_I*sigma_M);
		}

	// 寻找 rhoVertical 中最大最小值
	maxValue = rhoHorizontal[1][3];
	minValue = rhoHorizontal[1][3];
	for (int i = 1; i < srcImage.rows - 1; i++)
		for (int j = 3; j < srcImage.cols - 3; j++)
		{
			if (rhoHorizontal[i][j] > maxValue)
				maxValue = rhoHorizontal[i][j];
			if (rhoHorizontal[i][j] < minValue)
				minValue = rhoHorizontal[i][j];
		}

	// 归一化
	Mat outputHorizontal(srcImage.size(), CV_8UC1, Scalar::all(0));
	for (int i = 1; i < srcImage.rows - 1; i++)
	{
		ptr_uDst = outputHorizontal.ptr(i);
		for (int j = 3; j < srcImage.cols - 3; j++)
		{
			double cur = (rhoHorizontal[i][j] - minValue) / (maxValue - minValue);
			if (cur < 0)
				cur = 0;
			ptr_uDst[j] = cvRound(255 * cur);
		}
	}
	imshow("output Horizontal", outputHorizontal);

	//...............................................//
	//												 //
	//				       STEP.3                    //
	//				        右上		    	   	 //
	//												 //
	//...............................................//

	///////////////////////////////////////////////////
	//		# 1 模板矩阵以及相关参数
	///////////////////////////////////////////////////
	int M_topRight[9][9] = { { 0,0,0,0,0,0,high,0,0 },{ 0,0,0,0,0,high,0,low,0 },{ 0,0,0,0,high,0,low,0,high },
							 { 0,0,0,high,0,low,0,high,0 },{ 0,0,high,0,low,0,high,0,0 },{ 0,high,0,low,0,high,0,0,0 },
							 { high,0,low,0,high,0,0,0,0 },{ 0,low,0,high,0,0,0,0,0 },{ 0,0,high,0,0,0,0,0,0 } };
 
	///////////////////////////////////////////////////
	//		# 2 在原灰度图中嵌套模板进行计算
	///////////////////////////////////////////////////

	// 创建并初始化 rho 向量，它与图像同样的尺寸
	vector<vector<double>> rhoTopRight(srcImage.rows);
	for (int i = 0; i < srcImage.rows; i++)
		for (int j = 0; j < srcImage.cols; j++)
		{
			rhoTopRight[i].push_back(0);
		}
 
	for (int i = 4; i < srcImage.rows - 4; i++)
		for (int j = 4; j < srcImage.cols - 4; j++)
		{
			total = 0;
			for (int win_row = -4; win_row <= 4; win_row++)
				for (int win_cols = -4; win_cols <= 4; win_cols++)
				{
					if (M_topRight[win_row + 4][win_cols + 4] != 0)
						total += srcImage.at<uchar>(i + win_row, j + win_cols);
				}
			// 当前 I 上的点套用模板之下的平均值
			float mean_I = total / 21;

			// 当前 I 上的点套用模板之下的方差和标准差
			total = 0;
			for (int win_row = -4; win_row <= 4; win_row++)
				for (int win_cols = -4; win_cols <= 4; win_cols++)
				{
					if (M_topRight[win_row + 4][win_cols + 4] != 0)
						total += (srcImage.at<uchar>(i + win_row, j + win_cols) - mean_I)*(srcImage.at<uchar>(i + win_row, j + win_cols) - mean_I);
				}
			float var_I = total / 20;	// 这里分母是 21 - 1
			float sigma_I = sqrt(var_I);

			if (sigma_I == 0)
				continue;

			total = 0;
			for (int win_row = -4; win_row <= 4; win_row++)
				for (int win_cols = -4; win_cols <= 4; win_cols++)
					if (M_topRight[win_row + 4][win_cols + 4] != 0)
					{
						total += (srcImage.at<uchar>(i + win_row, j + win_cols) - mean_I)*(M_topRight[win_row + 4][win_cols + 4] - mean_M);
					}

			rhoTopRight[i][j] = total / (sigma_I*sigma_M);
		}

	// 寻找 rhoTopRight 中最大最小值
	// 需要修改
	maxValue = rhoTopRight[4][4];
	minValue = rhoTopRight[4][4];
	for (int i = 4; i < srcImage.rows - 4; i++)
		for (int j = 4; j < srcImage.cols - 4; j++)
		{
			if (rhoTopRight[i][j] > maxValue)
				maxValue = rhoTopRight[i][j];
			if (rhoTopRight[i][j] < minValue)
				minValue = rhoTopRight[i][j];
		}

	// 归一化
	Mat outputTopRight(srcImage.size(), CV_8UC1, Scalar::all(0));
	for (int i = 4; i < srcImage.rows - 4; i++)
	{
		ptr_uDst = outputTopRight.ptr(i);
		for (int j = 4; j < srcImage.cols - 4; j++)
		{
			double cur = (rhoTopRight[i][j] - minValue) / (maxValue - minValue);
			if (cur < 0)
				cur = 0;
			ptr_uDst[j] = cvRound(255 * cur);
		}
	}
	imshow("output Top Right 45", outputTopRight);

	//...............................................//
	//												 //
	//				       STEP.4                    //
	//				        左上		    	   	 //
	//												 //
	//...............................................//

	///////////////////////////////////////////////////
	//		# 1 模板矩阵以及相关参数
	///////////////////////////////////////////////////
 	int M_topLeft[9][9] = { { 0,0,high,0,0,0,0,0,0 },{ 0,low,0,high,0,0,0,0,0 },{ high,0,low,0,high,0,0,0,0 },
							{ 0,high,0,low,0,high,0,0,0 },{ 0,0,high,0,low,0,high,0,0 },{ 0,0,0,high,0,low,0,high,0 } ,
							{ 0,0,0,0,high,0,low,0,high } ,{ 0,0,0,0,0,high,0,low,0 } ,{ 0,0,0,0,0,0,high,0,0 } };

	///////////////////////////////////////////////////
	//		# 2 在原灰度图中嵌套模板进行计算
	///////////////////////////////////////////////////

	// 创建并初始化 rho 向量，它与图像同样的尺寸
	vector<vector<double>> rhoTopLeft(srcImage.rows);
	for (int i = 0; i < srcImage.rows; i++)
		for (int j = 0; j < srcImage.cols; j++)
		{
			rhoTopLeft[i].push_back(0);
		}
	
	for (int i = 4; i < srcImage.rows - 4; i++)
		for (int j = 4; j < srcImage.cols - 4; j++)
		{
			total = 0;
			for (int win_row = -4; win_row <= 4; win_row++)
				for (int win_cols = -4; win_cols <= 4; win_cols++)
				{
					if (M_topLeft[win_row + 4][win_cols + 4] != 0)
						total += srcImage.at<uchar>(i + win_row, j + win_cols);
				}
			// 当前 I 上的点套用模板之下的平均值
			float mean_I = total / 21;
			
			// 当前 I 上的点套用模板之下的方差和标准差
			total = 0;
			for (int win_row = -4; win_row <= 4; win_row++)
				for (int win_cols = -4; win_cols <= 4; win_cols++)
				{
					if (M_topLeft[win_row + 4][win_cols + 4] != 0)
						total += (srcImage.at<uchar>(i + win_row, j + win_cols) - mean_I)*(srcImage.at<uchar>(i + win_row, j + win_cols) - mean_I);
				}
			float var_I = total / 20;	// 这里分母是 21 - 1
			float sigma_I = sqrt(var_I);

			if (sigma_I == 0)
				continue;

			total = 0;
			for (int win_row = -4; win_row <= 4; win_row++)
				for (int win_cols = -4; win_cols <= 4; win_cols++)
					if (M_topLeft[win_row + 4][win_cols + 4] != 0)
					{
						total += (srcImage.at<uchar>(i + win_row, j + win_cols) - mean_I)*(M_topLeft[win_row + 4][win_cols + 4] - mean_M);
					}

			rhoTopLeft[i][j] = total / (sigma_I*sigma_M);
		}
	
	// 寻找 rhoTopRight 中最大最小值
	// 需要修改
	maxValue = rhoTopLeft[4][4];
	minValue = rhoTopLeft[4][4];
	for (int i = 4; i < srcImage.rows - 4; i++)
		for (int j = 4; j < srcImage.cols - 4; j++)
		{
			if (rhoTopLeft[i][j] > maxValue)
				maxValue = rhoTopLeft[i][j];
			if (rhoTopLeft[i][j] < minValue)
				minValue = rhoTopLeft[i][j];
		}
	// 归一化
	Mat outputTopLeft(srcImage.size(), CV_8UC1, Scalar::all(0));
	for (int i = 4; i < srcImage.rows - 4; i++)
	{
		ptr_uDst = outputTopLeft.ptr(i);
		for (int j = 4; j < srcImage.cols - 4; j++)
		{
			double cur = (rhoTopLeft[i][j] - minValue) / (maxValue - minValue);
			if (cur < 0)
				cur = 0;
			ptr_uDst[j] = cvRound(255 * cur);
		}
	}
	imshow("output TopLeft 45", outputTopLeft);

	//...............................................//
	//												 //
	//				       STEP.5                    //
	//				      右上pi/8		    	   	 //
	//												 //
	//...............................................//

	///////////////////////////////////////////////////
	//		# 1 模板矩阵以及相关参数
	///////////////////////////////////////////////////
	int M_topRight_225[7][7] = { { 0,0,0,0,0,high,high },{ 0,0,high,high,high,low,low },{ high,high,low,low,low,high,high },
								 { low,low,high,high,high,0,0 },{ high,high,0,0,0,0,0 },{ 0,0,0,0,0,0,0 },
								 { 0,0,0,0,0,0,0 } };
  
 	///////////////////////////////////////////////////
 	//		# 2 在原灰度图中嵌套模板进行计算
 	///////////////////////////////////////////////////
	
	// 创建并初始化 rho 向量，它与图像同样的尺寸
	vector<vector<double>> rhoTopRight_225(srcImage.rows);
	for (int i = 0; i < srcImage.rows; i++)
		for (int j = 0; j < srcImage.cols; j++)
		{
			rhoTopRight_225[i].push_back(0);
		}
 
	for (int i = 3; i < srcImage.rows - 3; i++)
		for (int j = 3; j < srcImage.cols - 3; j++)
		{
			total = 0;
			for (int win_row = -3; win_row <= 3; win_row++)
				for (int win_cols = -3; win_cols <= 3; win_cols++)
				{
					if (M_topRight_225[win_row + 3][win_cols + 3] != 0)
						total += srcImage.at<uchar>(i + win_row, j + win_cols);
				}
			// 当前 I 上的点套用模板之下的平均值
			float mean_I = total / 21;

			// 当前 I 上的点套用模板之下的方差和标准差
			total = 0;
			for (int win_row = -3; win_row <= 3; win_row++)
				for (int win_cols = -3; win_cols <= 3; win_cols++)
				{
					if (M_topRight_225[win_row + 3][win_cols + 3] != 0)
						total += (srcImage.at<uchar>(i + win_row, j + win_cols) - mean_I)*(srcImage.at<uchar>(i + win_row, j + win_cols) - mean_I);
				}
			float var_I = total / 20;	// 这里分母是 21 - 1
			float sigma_I = sqrt(var_I);

			if (sigma_I == 0)
				continue;

			total = 0;
			for (int win_row = -3; win_row <= 3; win_row++)
				for (int win_cols = -3; win_cols <= 3; win_cols++)
					if (M_topRight_225[win_row + 3][win_cols + 3] != 0)
					{
						total += (srcImage.at<uchar>(i + win_row, j + win_cols) - mean_I)*(M_topRight_225[win_row + 3][win_cols + 3] - mean_M);
					}

			rhoTopRight_225[i][j] = total / (sigma_I*sigma_M);
		}

	// 寻找 rhoTopRight 中最大最小值
	// 需要修改
	maxValue = rhoTopRight_225[3][3];
	minValue = rhoTopRight_225[3][3];
	for (int i = 3; i < srcImage.rows - 3; i++)
		for (int j = 3; j < srcImage.cols - 3; j++)
		{
			if (rhoTopRight_225[i][j] > maxValue)
				maxValue = rhoTopRight_225[i][j];
			if (rhoTopRight_225[i][j] < minValue)
				minValue = rhoTopRight_225[i][j];
		}

	// 归一化
	Mat outputTopRight225(srcImage.size(), CV_8UC1, Scalar::all(0));
	for (int i = 3; i < srcImage.rows - 3; i++)
	{
		ptr_uDst = outputTopRight225.ptr(i);
		for (int j = 3; j < srcImage.cols - 3; j++)
		{
			double cur = (rhoTopRight_225[i][j] - minValue) / (maxValue - minValue);
			if (cur < 0)
				cur = 0;
			ptr_uDst[j] = cvRound(255 * cur);
		}
	}
	imshow("output TopRight 22.5", outputTopRight225);

	//...............................................//
	//												 //
	//				       STEP.6                    //
	//				      左上pi/8		    	   	 //
	//												 //
	//...............................................//

	///////////////////////////////////////////////////
	//		# 1 模板矩阵以及相关参数
	///////////////////////////////////////////////////
	int M_topLeft_225[7][7] = { { high,high,0,0,0,0,0 },{ low,low,high,high,high,0,0 },{ high,high,low,low,low,high,high },
								 { 0,0,high,high,high,low,low },{ 0,0,0,0,0,high,high },
								 { 0,0,0,0,0,0,0 },{ 0,0,0,0,0,0,0 } };

	///////////////////////////////////////////////////
	//		# 2 在原灰度图中嵌套模板进行计算
	///////////////////////////////////////////////////

	// 创建并初始化 rho 向量，它与图像同样的尺寸
	vector<vector<double>> rhoTopLeft_225(srcImage.rows);
	for (int i = 0; i < srcImage.rows; i++)
	for (int j = 0; j < srcImage.cols; j++)
	{
		rhoTopLeft_225[i].push_back(0);
	}

	for (int i = 3; i < srcImage.rows - 3; i++)
	for (int j = 3; j < srcImage.cols - 3; j++)
	{
		total = 0;
		for (int win_row = -3; win_row <= 3; win_row++)
			for (int win_cols = -3; win_cols <= 3; win_cols++)
			{
				if (M_topLeft_225[win_row + 3][win_cols + 3] != 0)
					total += srcImage.at<uchar>(i + win_row, j + win_cols);
			}
		// 当前 I 上的点套用模板之下的平均值
		float mean_I = total / 21;

		// 当前 I 上的点套用模板之下的方差和标准差
		total = 0;
		for (int win_row = -3; win_row <= 3; win_row++)
			for (int win_cols = -3; win_cols <= 3; win_cols++)
			{
				if (M_topLeft_225[win_row + 3][win_cols + 3] != 0)
					total += (srcImage.at<uchar>(i + win_row, j + win_cols) - mean_I)*(srcImage.at<uchar>(i + win_row, j + win_cols) - mean_I);
			}
		float var_I = total / 20;	// 这里分母是 21 - 1
		float sigma_I = sqrt(var_I);

		if (sigma_I == 0)
			continue;

		total = 0;
		for (int win_row = -3; win_row <= 3; win_row++)
			for (int win_cols = -3; win_cols <= 3; win_cols++)
				if (M_topLeft_225[win_row + 3][win_cols + 3] != 0)
				{
					total += (srcImage.at<uchar>(i + win_row, j + win_cols) - mean_I)*(M_topLeft_225[win_row + 3][win_cols + 3] - mean_M);
				}

		rhoTopLeft_225[i][j] = total / (sigma_I*sigma_M);
	}

	// 寻找 rhoTopRight 中最大最小值
	// 需要修改
	maxValue = rhoTopLeft_225[3][3];
	minValue = rhoTopLeft_225[3][3];
	for (int i = 3; i < srcImage.rows - 3; i++)
	for (int j = 3; j < srcImage.cols - 3; j++)
	{
		if (rhoTopLeft_225[i][j] > maxValue)
			maxValue = rhoTopLeft_225[i][j];
		if (rhoTopLeft_225[i][j] < minValue)
			minValue = rhoTopLeft_225[i][j];
	}

	// 归一化
	Mat outputTopLeft225(srcImage.size(), CV_8UC1, Scalar::all(0));
	for (int i = 3; i < srcImage.rows - 3; i++)
	{
	ptr_uDst = outputTopLeft225.ptr(i);
	for (int j = 3; j < srcImage.cols - 3; j++)
	{
		double cur = (rhoTopLeft_225[i][j] - minValue) / (maxValue - minValue);
		if (cur < 0)
			cur = 0;
		ptr_uDst[j] = cvRound(255 * cur);
	}
	}
	imshow("output TopLeft 22.5", outputTopLeft225);

	//...............................................//
	//												 //
	//				       STEP.7                    //
	//				     右上 3pi/8		    	   	 //
	//												 //
	//...............................................//

	///////////////////////////////////////////////////
	//		# 1 模板矩阵以及相关参数
	///////////////////////////////////////////////////
	int M_topRight_675[7][7] = { { 0,0,high,low,high,0,0 },{ 0,0,high,low,high,0,0 },{ 0,high,low,high,0,0,0 },
								 { 0,high,low,high,0,0,0 },{ 0,high,low,high,0,0,0 },
								 { high,low,high,0,0,0,0 },{ high,low,high,0,0,0,0 } };

	///////////////////////////////////////////////////
	//		# 2 在原灰度图中嵌套模板进行计算
	///////////////////////////////////////////////////

	// 创建并初始化 rho 向量，它与图像同样的尺寸
	vector<vector<double>> rhoTopRight_675(srcImage.rows);
	for (int i = 0; i < srcImage.rows; i++)
		for (int j = 0; j < srcImage.cols; j++)
		{
			rhoTopRight_675[i].push_back(0);
		}

	for (int i = 3; i < srcImage.rows - 3; i++)
		for (int j = 3; j < srcImage.cols - 3; j++)
		{
			total = 0;
			for (int win_row = -3; win_row <= 3; win_row++)
				for (int win_cols = -3; win_cols <= 3; win_cols++)
				{
					if (M_topRight_675[win_row + 3][win_cols + 3] != 0)
						total += srcImage.at<uchar>(i + win_row, j + win_cols);
				}
			// 当前 I 上的点套用模板之下的平均值
			float mean_I = total / 21;

			// 当前 I 上的点套用模板之下的方差和标准差
			total = 0;
			for (int win_row = -3; win_row <= 3; win_row++)
				for (int win_cols = -3; win_cols <= 3; win_cols++)
				{
					if (M_topRight_675[win_row + 3][win_cols + 3] != 0)
						total += (srcImage.at<uchar>(i + win_row, j + win_cols) - mean_I)*(srcImage.at<uchar>(i + win_row, j + win_cols) - mean_I);
				}
			float var_I = total / 20;	// 这里分母是 21 - 1
			float sigma_I = sqrt(var_I);

			if (sigma_I == 0)
				continue;

			total = 0;
			for (int win_row = -3; win_row <= 3; win_row++)
				for (int win_cols = -3; win_cols <= 3; win_cols++)
					if (M_topRight_675[win_row + 3][win_cols + 3] != 0)
					{
						total += (srcImage.at<uchar>(i + win_row, j + win_cols) - mean_I)*(M_topRight_675[win_row + 3][win_cols + 3] - mean_M);
					}

			rhoTopRight_675[i][j] = total / (sigma_I*sigma_M);
		}

	// 寻找 rhoTopRight 中最大最小值
	// 需要修改
	maxValue = rhoTopRight_675[3][3];
	minValue = rhoTopRight_675[3][3];
	for (int i = 3; i < srcImage.rows - 3; i++)
		for (int j = 3; j < srcImage.cols - 3; j++)
		{
			if (rhoTopRight_675[i][j] > maxValue)
				maxValue = rhoTopRight_675[i][j];
			if (rhoTopRight_675[i][j] < minValue)
				minValue = rhoTopRight_675[i][j];
		}
	cout << "Max = " << maxValue << endl << "Min = " << minValue << endl; // test

	// 归一化
	Mat outputTopRight_675(srcImage.size(), CV_8UC1, Scalar::all(0));
	for (int i = 3; i < srcImage.rows - 3; i++)
	{
		ptr_uDst = outputTopRight_675.ptr(i);
		for (int j = 3; j < srcImage.cols - 3; j++)
		{
			double cur = (rhoTopRight_675[i][j] - minValue) / (maxValue - minValue);
			if (cur < 0)
				cur = 0;
			ptr_uDst[j] = cvRound(255 * cur);
		}
	}
	imshow("output outputTop Right 675", outputTopRight_675);

	//...............................................//
	//												 //
	//				       STEP.8                    //
	//				     左上 3pi/8		    	   	 //
	//												 //
	//...............................................//

	///////////////////////////////////////////////////
	//		# 1 模板矩阵以及相关参数
	///////////////////////////////////////////////////
	int M_topLeft_675[7][7] = { { 0,0,high,low,high,0,0 },{ 0,0,high,low,high,0,0 },
								{ 0,0,0,high,low,high,0 },{ 0,0,0,high,low,high,0 },{ 0,0,0,high,low,high,0 },
								{ 0,0,0,0,high,low,high},{ 0,0,0,0,high,low,high } };

	///////////////////////////////////////////////////
	//		# 2 在原灰度图中嵌套模板进行计算
	///////////////////////////////////////////////////
	// 创建并初始化 rho 向量，它与图像同样的尺寸
	vector<vector<double>> rhoTopLeft_675(srcImage.rows);
	for (int i = 0; i < srcImage.rows; i++)
		for (int j = 0; j < srcImage.cols; j++)
		{
			rhoTopLeft_675[i].push_back(0);
		}

	for (int i = 3; i < srcImage.rows - 3; i++)
		for (int j = 3; j < srcImage.cols - 3; j++)
		{
			total = 0;
			for (int win_row = -3; win_row <= 3; win_row++)
				for (int win_cols = -3; win_cols <= 3; win_cols++)
				{
					if (M_topLeft_675[win_row + 3][win_cols + 3] != 0)
						total += srcImage.at<uchar>(i + win_row, j + win_cols);
				}
			// 当前 I 上的点套用模板之下的平均值
			float mean_I = total / 21;

			// 当前 I 上的点套用模板之下的方差和标准差
			total = 0;
			for (int win_row = -3; win_row <= 3; win_row++)
				for (int win_cols = -3; win_cols <= 3; win_cols++)
				{
					if (M_topLeft_675[win_row + 3][win_cols + 3] != 0)
						total += (srcImage.at<uchar>(i + win_row, j + win_cols) - mean_I)*(srcImage.at<uchar>(i + win_row, j + win_cols) - mean_I);
				}
			float var_I = total / 20;	// 这里分母是 21 - 1
			float sigma_I = sqrt(var_I);

			if (sigma_I == 0)
				continue;

			total = 0;
			for (int win_row = -3; win_row <= 3; win_row++)
				for (int win_cols = -3; win_cols <= 3; win_cols++)
					if (M_topLeft_675[win_row + 3][win_cols + 3] != 0)
					{
						total += (srcImage.at<uchar>(i + win_row, j + win_cols) - mean_I)*(M_topLeft_675[win_row + 3][win_cols + 3] - mean_M);
					}

			rhoTopLeft_675[i][j] = total / (sigma_I*sigma_M);
		}

	// 寻找 rhoTopRight 中最大最小值
	// 需要修改
	maxValue = rhoTopLeft_675[3][3];
	minValue = rhoTopLeft_675[3][3];
	for (int i = 3; i < srcImage.rows - 3; i++)
		for (int j = 3; j < srcImage.cols - 3; j++)
		{
			if (rhoTopLeft_675[i][j] > maxValue)
				maxValue = rhoTopLeft_675[i][j];
			if (rhoTopLeft_675[i][j] < minValue)
				minValue = rhoTopLeft_675[i][j];
		}
	cout << "Max = " << maxValue << endl << "Min = " << minValue << endl; // test
	// 归一化
	Mat outputTopLeft_675(srcImage.size(), CV_8UC1, Scalar::all(0));
	for (int i = 3; i < srcImage.rows - 3; i++)
	{
		ptr_uDst = outputTopLeft_675.ptr(i);
		for (int j = 3; j < srcImage.cols - 3; j++)
		{
			double cur = (rhoTopLeft_675[i][j] - minValue) / (maxValue - minValue);
			if (cur < 0)
				cur = 0;
			ptr_uDst[j] = cvRound(255 * cur);
		}
	}
	imshow("output TopLeft 675", outputTopLeft_675);

	Mat finalImage(srcImage.size(), CV_8UC1, Scalar::all(0));
	int maxGrayValue = 0;
	uchar *image1, *image2, *image3, *image4, *image5, *image6, *image7, *image8, *dst;;
	for (int i = 0; i < finalImage.rows; i++)
	{
		image1 = outputVertical.ptr(i);
		image2 = outputHorizontal.ptr(i);
		image3 = outputTopRight.ptr(i);
		image4 = outputTopLeft.ptr(i);
		image5 = outputTopRight225.ptr(i);
		image6 = outputTopLeft225.ptr(i);;
		image7 = outputTopRight_675.ptr(i);
		image8 = outputTopLeft_675.ptr(i);
		dst = finalImage.ptr(i);
		for (int j = 0; j < finalImage.cols; j++)
		{
			maxGrayValue = image1[j];
			if (image2[j] > maxGrayValue)
				maxGrayValue = image2[j];
			if (image3[j] > maxGrayValue)
				maxGrayValue = image3[j];
			if (image4[j] > maxGrayValue)
				maxGrayValue = image4[j];
			if (image5[j] > maxGrayValue)
				maxGrayValue = image5[j];
			if (image6[j] > maxGrayValue)
				maxGrayValue = image6[j];
			if (image7[j] > maxGrayValue)
				maxGrayValue = image7[j];
			if (image8[j] > maxGrayValue)
				maxGrayValue = image8[j];
			dst[j] = maxGrayValue;
		}
	}
	imshow("final image", finalImage);
	czh_imwrite(finalImage, "d");
	int i = 206;
	int j = 3;

	cout << "outputHorizontal" << (int)outputHorizontal.at<uchar>(i, j) << endl
		<< "outputVertical" << (int)outputVertical.at<uchar>(i, j) << endl
		<< "outputTopLeft" << (int)outputTopLeft.at<uchar>(i, j) << endl
		<< "outputTopRight" << (int)outputTopRight.at<uchar>(i, j) << endl
		<< "outputTopLeft225" << (int)outputTopLeft225.at<uchar>(i, j) << endl
		<< "outputTopRight225" << (int)outputTopRight225.at<uchar>(i, j) << endl
		<< "outputTopLeft_675" << (int)outputTopLeft_675.at<uchar>(i, j) << endl
		<< "outputTopRight_675" << (int)outputTopRight_675.at<uchar>(i, j) << endl;

	// 显示 profile
	showProfile = false;
	if (showProfile == true)
	{
		Point p0(185, 576);
		Point p1(337, 377);
		Mat profileImage = czh_Line_Profile(finalImage, p0, p1);
		cv::imshow("srcImage", finalImage);
		czh_imwrite(finalImage, "profile");
		waitKey();
	}

	srcImage = finalImage;
	namedWindow(WINDOW_NAME_BW);
	createTrackbar("Threshold", WINDOW_NAME_BW, &g_bwThreshold, g_bwThresholdMax, on_Trackbar_Threshold);
	waitKey();

// 	dilate(bwImage, srcImage, getStructuringElement(MORPH_RECT, Size(3, 3), Point(-1, -1)), Point(-1, -1),3);
// 	erode(srcImage, srcImage, getStructuringElement(MORPH_RECT, Size(3, 3), Point(-1, -1)), Point(-1, -1), 3);
// 	imshow("dilate", srcImage);

	// 做去噪操作
	Mat denoisedImage;
	czh_bwAreaOpen(bwImage, denoisedImage, 50, 100, 5);
	imshow("去噪图像", denoisedImage);



	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//																																				  //
	//															开始算法运算																		  //
	//																																				  //
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//...............................................//
	//												 //
	//				       STEP.1                    //
	//			      霍夫变换检测直线	     	   	 //
	//												 //
	//...............................................//

// 	// 滑动条选择最佳霍夫线阈值：开始
// 	srcImage = denoisedImage;
// 	namedWindow(WINDOW_NAME);
// 	g_thresholdHough = 130;
// 	g_thresholdAngle = 0;
// 	g_thresholdRho = 0;
// 	createTrackbar("Hough Threshold", WINDOW_NAME, &g_thresholdHough, g_thresholdHoughMax, on_Trackbar_Hough);
// 	createTrackbar("Angle", WINDOW_NAME, &g_thresholdAngle, g_thresholdAngleMax, on_Trackbar_Hough);
// 	createTrackbar("Rho", WINDOW_NAME, &g_thresholdRho, g_thresholdRhoMax, on_Trackbar_Hough);
// 	// 滑动条选择最佳霍夫线阈值：结束
// 
// 	// 按下任意键继续下边的操作
// 	waitKey(0);	
// 	imshow("lineImage", lineImage);
	 
	//...............................................//
	//												 //
	//				       STEP.2                    //
	//			    根据直线图像检测交点	   	   	 //
	//												 //
	//...............................................//
	
// 	time = static_cast<double>(getTickCount());
// 	
// 	Mat potentialPointImage(srcImage.size(), CV_8UC1, Scalar::all(0));
// 	
// 	uchar *rowCur, *rowTop, *rowBot, *ptrDst;	// 当前行，上一行，下一行的行指针
// 	for (int i = 1; i < lineImage.rows - 1; i++)
// 	{	// 遍历得到的线图像每个像素，寻找可能是交点的点
// 		rowTop = lineImage.ptr<uchar>(i - 1);
// 		rowCur = lineImage.ptr<uchar>(i);
// 		rowBot = lineImage.ptr<uchar>(i + 1);
// 		ptrDst = potentialPointImage.ptr<uchar>(i);
// 		for (int j = 1; j < lineImage.cols - 1; j++)	// 遍历 lineImage 中每一个像素
// 		{
// 			if (rowCur[j] == 255)	// 如果当前像素是前景像素
// 			{	// 如果它8个邻域中有且只有4个非零像素，则可能是交点
// 				if ((int)(rowCur[j - 1] + rowCur[j + 1] + rowTop[j - 1] + rowTop[j] + rowTop[j + 1] + rowBot[j - 1] + rowBot[j] + rowBot[j + 1]) == 1020)
// 					ptrDst[j] = 255;
// 			}
// 		}
// 	}
// 	// 对已经找到的交点图像做连通域标记，以便于下一步的重心查找
// 	Mat labelledPointImage;
// 	czh_labeling(potentialPointImage, labelledPointImage);
// 	
// 	time = ((double)getTickCount() - time) / getTickFrequency();
// 	cout << "STEP.2 交点初步判定+连通域标记 finished :\t" << time << " second used.\n" << endl;

// 	//...............................................//
// 	//												 //
// 	//				     STEP.3                      //
// 	//					寻找重心					 //
// 	//												 //
// 	//...............................................//
// 	
// 	time = static_cast<double>(getTickCount());
// 	
// 	Mat dstImage(srcImage.size(), CV_8UC1, Scalar::all(0));
// 	vector<Point> centerPoints;
// 	czh_centerOfGravity(labelledPointImage, dstImage, centerPoints);	// 寻找重心
// 	
// 	time = ((double)getTickCount() - time) / getTickFrequency();
// 	cout << "STEP.2 交点初步判定+连通域标记 finished :\t" << time << " second used.\n" << endl;
// 	
// 	
// 	//...............................................//
// 	//												 //
// 	//				     STEP.4                      //
// 	//				   输出点坐标					 //
// 	//												 //
// 	//...............................................//
// 	
// 	// 把角点坐标输出为 txt 文件
// 	ofstream ffout;
// 	string outtFileName = "result " + fileName + ".txt";
// 	ffout.open(outtFileName);
// 	for (int i = 0; i < centerPoints.size(); i++)
// 	{
// 		ffout << centerPoints[i].x << ",\t" << centerPoints[i].y << endl;
// 	}
// 	ffout.close();
// 
// 	//////////////////////////////////////////////////////////////////////////////////////////////////////
// 	//////////////////////////////////////////////////////////////////////////////////////////////////////

	// 程序结束信息：报告程序所花时间
	timeTotal = ((double)getTickCount() - timeTotal) / getTickFrequency();
	std::cout << "Program finished :\t" << timeTotal << " second used.\n" << endl;

	// 测试区域
//	imshow("Gray scale image", src);	// 显示原始灰度图像
//	imshow("【二值图像以及线条】", lineShowImage);	// 显示二值图像
//	imshow("【线检测效果】", lineImage);	// 显示线检测结果
// 	imshow("【潜在交点检测效果】", potentialPointImage);			// 显示最终结果图像
// 	imshow("【最终交点检测效果】", dstImage);	// 显示第三步效果图

	cv::waitKey();
	std::system("pause");
}

// Back up 
/*
void backUp_method_one()
{
	// 轮廓查找方法一：开始
	Mat dilatedImage, closedImage;
	Mat element = getStructuringElement(MORPH_RECT, Size(3, 3));	// 形态学构造元素
																	// 对源灰度图像做闭操作：先做膨胀，再做腐蚀
	dilate(src, dilatedImage, element);	// 对元灰度图做膨胀
	erode(dilatedImage, closedImage, element);	// 对膨胀图做腐蚀，得到闭图像

												// 用得到的闭图像减去原灰度图，得到标定盘方框轮廓
	src.copyTo(srcImage);
	imshow("原始灰度图", srcImage);
	MatIterator_<uchar> itUcharBegin, itUcharEnd, itDst, itSrc;
	for (itUcharBegin = closedImage.begin<uchar>(), itUcharEnd = closedImage.end<uchar>(), itDst = srcImage.begin<uchar>(), itSrc = src.begin<uchar>(); itUcharBegin != itUcharEnd; itUcharBegin++, itDst++, itSrc++)
	{
		(*itDst) = (*itUcharBegin) - (*itSrc);
	}
	imshow("Method # 1", srcImage);
	threshold(srcImage, srcImage, 100, 255, CV_THRESH_OTSU);	// Otsu阈值分割
	imshow("Otsu # 1", srcImage);
	// 轮廓查找方法一：结束
}

void backUp_method_two()
{
	// 轮廓查找方法二：开始
	Mat dilatedImage1, dilatedImage2, dilatedImage3, dilatedImage4;
	Mat closedImage1, closedImage2, closedImage3, closedImage4;
	Mat element1(3, 5, CV_8UC1, Scalar::all(255));	// 形态学构造元素# 1
	Mat element2(7, 7, CV_8UC1, Scalar::all(0));	// 形态学构造元素# 2
	Mat element3(5, 3, CV_8UC1, Scalar::all(255));	// 形态学构造元素# 3
	Mat element4(7, 7, CV_8UC1, Scalar::all(0));	// 形态学构造元素# 4

													// 形态学构造元素# 2 赋值
	element2.at<uchar>(0, 4) = 255;
	element2.at<uchar>(1, 3) = element2.at<uchar>(1, 5) = 255;
	element2.at<uchar>(2, 2) = element2.at<uchar>(2, 4) = element2.at<uchar>(2, 6) = 255;
	element2.at<uchar>(3, 1) = element2.at<uchar>(3, 3) = element2.at<uchar>(3, 5) = 255;
	element2.at<uchar>(4, 0) = element2.at<uchar>(4, 2) = element2.at<uchar>(4, 4) = 255;
	element2.at<uchar>(5, 1) = element2.at<uchar>(5, 3) = 255;
	element2.at<uchar>(6, 2) = 255;

	// 形态学构造元素# 4 赋值
	element4.at<uchar>(0, 2) = 255;
	element4.at<uchar>(1, 1) = element4.at<uchar>(1, 3) = 255;
	element4.at<uchar>(2, 0) = element4.at<uchar>(2, 2) = element4.at<uchar>(2, 4) = 255;
	element4.at<uchar>(3, 1) = element4.at<uchar>(3, 3) = element4.at<uchar>(3, 5) = 255;
	element4.at<uchar>(4, 2) = element4.at<uchar>(4, 4) = element4.at<uchar>(4, 6) = 255;
	element4.at<uchar>(5, 3) = element4.at<uchar>(5, 5) = 255;
	element4.at<uchar>(6, 4) = 255;

	// 对源灰度图像做闭操作：先做膨胀，再做腐蚀
	//构造体# 1
	dilate(src, dilatedImage1, element1);	// 对元灰度图做膨胀
	erode(dilatedImage1, closedImage1, element1);	// 对膨胀图做腐蚀，得到闭图像
													//构造体# 2
	dilate(src, dilatedImage2, element2);	// 对元灰度图做膨胀
	erode(dilatedImage2, closedImage2, element2);	// 对膨胀图做腐蚀，得到闭图像
													//构造体# 3
	dilate(src, dilatedImage3, element3);	// 对元灰度图做膨胀
	erode(dilatedImage3, closedImage3, element3);	// 对膨胀图做腐蚀，得到闭图像
													//构造体# 4
	dilate(src, dilatedImage4, element4);	// 对元灰度图做膨胀
	erode(dilatedImage4, closedImage4, element4);	// 对膨胀图做腐蚀，得到闭图像

													// 寻找4张图像中的像素 Max
	Mat maxImage(src.size(), CV_8UC1, Scalar::all(0));
	MatIterator_<uchar> itBegin1, itEnd1, itBegin2, itBegin3, itBegin4;
	for (itBegin1 = closedImage1.begin<uchar>(), itEnd1 = closedImage1.end<uchar>(), itBegin2 = closedImage2.begin<uchar>(), itBegin3 = closedImage3.begin<uchar>(), itBegin4 = closedImage4.begin<uchar>(), itDst = maxImage.begin<uchar>();
		itBegin1 != itEnd1; itBegin1++, itBegin2++, itBegin3++, itBegin4++, itDst++)
	{
		int maxValue = (*itBegin1);
		if ((*itBegin2) > maxValue)
		{
			maxValue = (*itBegin2);
		};
		if ((*itBegin3) > maxValue)
		{
			maxValue = (*itBegin3);
		};
		if ((*itBegin4) > maxValue)
		{
			maxValue = (*itBegin4);
		}
		(*itDst) = maxValue;
	}
	// imshow("maxImage", maxImage);

	// 用得到的闭图像减去原灰度图，得到标定盘方框轮廓
	src.copyTo(srcImage);
	for (itUcharBegin = maxImage.begin<uchar>(), itUcharEnd = maxImage.end<uchar>(), itDst = srcImage.begin<uchar>(), itSrc = src.begin<uchar>(); itUcharBegin != itUcharEnd; itUcharBegin++, itDst++, itSrc++)
	{
		(*itDst) = (*itUcharBegin) - (*itSrc);
	}
	imshow("Method # 2", srcImage);
	threshold(srcImage, srcImage, 100, 255, CV_THRESH_OTSU);	// Otsu阈值分割
	imshow("Otsu # 2", srcImage);
	// 轮廓查找方法二：结束
}