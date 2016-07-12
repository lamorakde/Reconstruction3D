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

Mat srcImage, bwSrcImage, bwResultImage;
int g_bwThreshold;
int g_bwThresholdMax = 255;

void on_Trackbar_Threshold(int, void*)
{
	threshold(bwSrcImage, bwResultImage, g_bwThreshold, 255, 0);
	imshow(WINDOW_NAME_BW, bwResultImage);
}

struct sort_X {
	bool operator() (cv::Point pt1, cv::Point pt2) { return (pt1.x < pt2.x); }
} sort_x;

struct sort_Y {
	bool operator() (cv::Point pt1, cv::Point pt2) { return (pt1.y < pt2.y); }
} sort_y;

int main()
{
	// 程序帮助信息
	string programInformation = "该程序会使用模板自相关方法自动寻找立式标定图中的角点.";
	string fileType = ".ppm";
	czh_helpInformation(programInformation);

	// 得到输入图像名称信息并输出
	std::cout << "Enter the input image name without " << fileType << ": ";
	string fileName, srcFileName;
	fileName = "30";
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
	Mat grayImage;
	src.copyTo(grayImage);

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

	// 显示 profile
	bool showProfile = false;
	if (showProfile == true)
	{
		Point pt1(0, 162);
		Point pt2(blurredImage.cols - 1, 162);
		Mat blurProfile = czh_Line_Profile(blurredImage, pt1, pt2, true);
		cv::imshow("srcImage", grayImage);
		waitKey();
	}
	
	vector<Point> inputPoints;
	inputPoints.push_back(Point(991, 317));
	inputPoints.push_back(Point(707, 753));
	inputPoints.push_back(Point(846, 756));
	inputPoints.push_back(Point(1126, 761));
	inputPoints.push_back(Point(1270, 765));

	vector<Point> cornerPts;	// 用于存放交点的点向量
	int cornerPts_EveryLine = 5;
	// 选择两个区域的两个不同的点作为模板
	for (int it = 0; it < inputPoints.size(); it++)
	{
		float total = 0;
		int x, y, M_radius;
		x = inputPoints[it].x;
		y = inputPoints[it].y;
		M_radius = 10;
// 		cout << "输入点坐标 X:\t";
// 		cin >> x;
// 		cout << "输入点坐标 Y:\t";
// 		cin >> y;
// 		cout << "输入自相关半径:\t";
// 		cin >> M_radius;
		int M_size = M_radius * 2 + 1;
		int ptsInMask = M_size*M_size;

		// image 2: (509, 285) radius = 5
		// image 3: (484, 271) radius = 6
		// image 4: (538, 360) radius = 6
		// kinect1: (982, 338) radius = 10; (1209, 701) radius = 5
		// kinect4: (1089, 486) radius = 10

		//...............................................//
		//												 //
		//				     STEP. 1                     //
		//			      灰度图像自相关	     	   	 //
		//												 //
		//...............................................//

		///////////////////////////////////////////////////
		//		# 1 模板矩阵以及相关参数
		///////////////////////////////////////////////////
		Mat correlationSrcImage;
		blurredImage.copyTo(correlationSrcImage);

		// 创建模板
		int **M = new int*[M_size];
		for (int i = 0; i < M_size; i++)
			M[i] = new int[M_size];

		// 初始化模板
		for (int i = -M_radius; i <= M_radius; i++)
			for (int j = -M_radius; j <= M_radius; j++)
			{
				M[i + M_radius][j + M_radius] = correlationSrcImage.at<uchar>(y + i, x + j);
				total += M[i + M_radius][j + M_radius];
			}

		float mean_M = total / ptsInMask;	// M 的均值

		total = 0;
		for (int i = 0; i < M_size; i++)
			for (int j = 0; j < M_size; j++)
				total += (M[i][j] - mean_M)*(M[i][j] - mean_M);

		float var_M = total / (ptsInMask - 1);		// M 的方差var，这里分母是 ptsReal - 1
		float sigma_M = sqrt(var_M);	// M 的标准差 sigma

		cout << "Mean  : " << mean_M << endl
			<< "Var   : " << var_M << endl
			<< "Sigma : " << sigma_M << endl;
		/////////////////////////////////////////////////
		//		# 2 在原灰度图中嵌套模板进行计算
		/////////////////////////////////////////////////

		// 创建并初始化 rho 向量，它与图像同样的尺寸
		double **rho = new double *[correlationSrcImage.rows];
		for (int i = 0; i < correlationSrcImage.rows; i++)
			rho[i] = new double[correlationSrcImage.cols];

		for (int i = 0; i < correlationSrcImage.rows; i++)
			for (int j = 0; j < correlationSrcImage.cols; j++)
				rho[i][j] = 0;

		float mean_I, var_I, sigma_I;
		uchar * ptr;

		for (int i = M_radius; i < correlationSrcImage.rows - M_radius; i++)
			for (int j = M_radius; j < correlationSrcImage.cols - M_radius; j++)
			{
				total = 0;
				for (int win_row = -M_radius; win_row <= M_radius; win_row++)
				{
					ptr = correlationSrcImage.ptr(i + win_row);

					for (int win_cols = -M_radius; win_cols <= M_radius; win_cols++)
						total += ptr[j + win_cols];
				}

				// 当前 I 上的点套用模板之下的平均值
				mean_I = total / ptsInMask;

				// 当前 I 上的点套用模板之下的方差和标准差
				total = 0;
				for (int win_row = -M_radius; win_row <= M_radius; win_row++)
				{
					ptr = correlationSrcImage.ptr(i + win_row);

					for (int win_cols = -M_radius; win_cols <= M_radius; win_cols++)
						total += (ptr[j + win_cols] - mean_I)*(ptr[j + win_cols] - mean_I);
				}

				var_I = total / (ptsInMask - 1);	// 这里分母是 ptsReal - 1
				sigma_I = sqrt(var_I);

				if (sigma_I == 0)
					continue;

				total = 0;
				for (int win_row = -M_radius; win_row <= M_radius; win_row++)
				{
					ptr = correlationSrcImage.ptr(i + win_row);

					for (int win_cols = -M_radius; win_cols <= M_radius; win_cols++)
						total += (ptr[j + win_cols] - mean_I)*(M[win_row + M_radius][win_cols + M_radius] - mean_M);
				}
				rho[i][j] = total / (sigma_I*sigma_M);
			}

		// 寻找 rhoVertical 中最大最小值
		float maxValue = rho[M_radius][M_radius];
		float minValue = rho[M_radius][M_radius];
		for (int i = M_radius; i < correlationSrcImage.rows - M_radius; i++)
			for (int j = M_radius; j < correlationSrcImage.cols - M_radius; j++)
			{
				if (rho[i][j] > maxValue)
					maxValue = rho[i][j];
				if (rho[i][j] < minValue)
					minValue = rho[i][j];
			}

		// 归一化
		uchar *ptr_uSrc, *ptr_uDst;
		Mat corImage(correlationSrcImage.size(), CV_8UC1, Scalar::all(0));
		for (int i = M_radius; i < correlationSrcImage.rows - M_radius; i++)
		{
			ptr_uDst = corImage.ptr(i);
			for (int j = M_radius; j < correlationSrcImage.cols - M_radius; j++)
			{
				double cur = (rho[i][j] - minValue) / (maxValue - minValue);
				if (cur < 0)
					cur = 0;
				ptr_uDst[j] = cvRound(255 * cur);
			}
		}

		// 局部最大
		Mat localMaxImage(correlationSrcImage.size(), CV_8UC1, Scalar::all(0));
		int localValue, localMaxWindowRadius = 7;
		bool jumpThisPixel = false;
		uchar *ptr_uCurLine;

		for (int i = M_radius + localMaxWindowRadius; i < corImage.rows - M_radius - localMaxWindowRadius; i++)
		{
			ptr_uDst = localMaxImage.ptr(i);	// 目标图像行指针
			ptr_uSrc = corImage.ptr(i);	// 原图像行指针
			for (int j = M_radius + localMaxWindowRadius; j < corImage.cols - M_radius - localMaxWindowRadius; j++)	// 遍历每一个像素
			{
				jumpThisPixel = false;	// 跳过判别符初始为 false
				localValue = ptr_uSrc[j];	// 当前像素值
				for (int row = -localMaxWindowRadius; row <= localMaxWindowRadius; row++)	// 在小窗口中遍历
				{
					ptr_uCurLine = corImage.ptr(i + row);	// 原图像行指针

					if (jumpThisPixel == true)	// 如果跳过判别符为真，则跳出这一次循环
						break;

					for (int col = -localMaxWindowRadius; col <= localMaxWindowRadius; col++)
						if (ptr_uCurLine[j + col] > localValue)	// 如果在小窗口中有比当前像素大的值，则跳出这一次循环
						{										// 并置判别符为真
							jumpThisPixel = true;
							break;
						}
				}
				if (jumpThisPixel == false)
					ptr_uDst[j] = localValue;
			}
		}

		// 阈值分割，筛选出期望点
		localMaxImage.copyTo(bwSrcImage);
		cv::namedWindow(WINDOW_NAME_BW);
		cv::createTrackbar("Threshold", WINDOW_NAME_BW, &g_bwThreshold, g_bwThresholdMax, on_Trackbar_Threshold);
		cv::waitKey();

		// 对已经找到的交点图像做连通域标记，以便于下一步的重心查找
		Mat dilatedImage;
		dilate(bwResultImage, dilatedImage, getStructuringElement(MORPH_RECT, Size(3, 3), Point(-1, -1)));
		Mat labelledPointImage;
		czh_labeling(dilatedImage, labelledPointImage);

		//...............................................//
		//												 //
		//				     STEP.2                      //
		//					寻找重心					 //
		//												 //
		//...............................................//

		Mat dstImage(srcImage.size(), CV_8UC1, Scalar::all(0));
		vector<Point> centerPts;
		czh_centerOfGravity(labelledPointImage, dstImage, centerPts);	// 寻找重心

		for (int i = 0; i < centerPts.size(); i++)
			cornerPts.push_back(centerPts[i]);

// 		//...............................................//
// 		//												 //
// 		//				     STEP.3                      //
// 		//				   对交点排序					 //
// 		//												 //
// 		//...............................................//
// 		// 最终图像中的点置入tempPts向量中
// 		vector<Point> tempPts;
// 		int ptsNum = 0;
// 		for (int i = 0; i < dstImage.rows; i++)
// 		{
// 			ptr = dstImage.ptr(i);
// 			for (int j = 0; j < dstImage.cols; j++)
// 				if (ptr[j] != 0)
// 				{
// 					tempPts.push_back(Point(j, i));
// 					ptsNum++;
// 				}
// 		}
// 
// 		// 先把所有点按照 y 坐标从小到大排序
// 		std::sort(tempPts.begin(), tempPts.end(), order_Y);
// 
// 		int cornerPtsLineNum = ptsNum / cornerPts_EveryLine;
// 		vector<vector<Point>> tempPtsForX(cornerPtsLineNum);
// 		int ptr_in_total = 0;
// 
// 		for (int i = 0; i < cornerPtsLineNum; i++)
// 			for (int j = 0; j < cornerPts_EveryLine; j++)
// 			{
// 				tempPtsForX[i].push_back(tempPts[ptr_in_total++]);
// 			}
// 
// 		for (int i = 0; i < cornerPtsLineNum; i++)
// 		{
// 			// 先把所有点按照 y 坐标从小到大排序
// 			std::sort(tempPtsForX[i].begin(), tempPtsForX[i].end(), order_X);
// 		}
// 
// 		for (int i = 0; i < cornerPtsLineNum; i++)
// 			for (int j = 0; j < cornerPts_EveryLine; j++)
// 			{
// 				cornerPts.push_back(tempPtsForX[i][j]);
// 			}
// 
// 		for (int i = 0; i < cornerPts.size(); i++)
// 			cout << cornerPts[i].x << ", " << cornerPts[i].y << endl;

		//////////////////////////////////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////////////////////////////////

		// 程序结束信息：报告程序所花时间
		destroyWindow(WINDOW_NAME_BW);
		timeTotal = ((double)getTickCount() - timeTotal) / getTickFrequency();
		std::cout << "Program finished :\t" << timeTotal << " second used.\n" << endl;

		// 测试区域
// 		cv::imshow("Gray Scale Image", grayImage);
// 		cv::imshow("Blured Image : Gaussian", blurredImage);
// 		cv::imshow("Correlation Image", corImage);
// 		cv::imshow("Local Max Image", localMaxImage);
// 		cv::imshow("Final Result", dstImage);

	}

	//...............................................//
	//												 //
	//				     STEP.3                      //
	//					角点排序					 //
	//												 //
	//...............................................//
	// 首先对 Y 坐标从小到大排序
	std::sort(cornerPts.begin(), cornerPts.end(), sort_y);
	// 把原角点向量拆分为多个行向量
	int lineNum = cornerPts.size() / cornerPts_EveryLine;
	vector<vector<Point>> cornerPts_inLine(lineNum);
	int ptr = 0;

	for (int i = 0; i < lineNum; i++)
		for (int j = 0; j < cornerPts_EveryLine; j++)
			cornerPts_inLine[i].push_back(cornerPts[ptr++]);

	// 对每个行向量上的角点按照 X 坐标从小到大排序
	for (int i = 0; i < lineNum; i++)
		std::sort(cornerPts_inLine[i].begin(), cornerPts_inLine[i].end(), sort_x);

	cornerPts.clear();	// 清楚原交点向量
	// 把排序之后的点重新压进交点向量
	for (int i = 0; i < lineNum; i++)
		for (int j = 0; j < cornerPts_EveryLine; j++)
			cornerPts.push_back(cornerPts_inLine[i][j]);
	//...............................................//
	//												 //
	//				     STEP.4                      //
	//					输出坐标					 //
	//												 //
	//...............................................//
	ofstream outFile; // 创建一个ofstream对象
	string coordinate = "coordinates ";
	string txt = ".txt";
	string coordinatesFile = coordinate + fileName + txt;
	outFile.open(coordinatesFile);

	for (int i = 0; i < cornerPts.size(); i++)
		outFile << cornerPts[i].x << ",\t" << cornerPts[i].y << endl;

	outFile.close();

	Mat finalShow(srcImage.size(), srcImage.type(), Scalar::all(0));
	for (int i = 0; i < cornerPts.size(); i++)
		finalShow.at<uchar>(cornerPts[i].y, cornerPts[i].x)=255;

//	imshow("Final", finalShow);
	cv::waitKey();
	std::system("pause");
}