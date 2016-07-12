#include "opencv2/opencv.hpp"
#include <iostream>
#include <windows.h>
#include <fstream>
#include "czh_binary_CV.h"

using namespace std;
using namespace cv;

int main()
{
	// 程序帮助信息
	string programInformation = "该程序会使用霍夫线变换方法自动寻找棋盘等标定图中的角点.";
	string fileType = ".ppm";
	czh_helpInformation(programInformation);

	// 得到输入图像名称信息并输出
	cout << "Enter the input image name without " << fileType << ": ";
	string fileName, srcFileName;
	fileName = "contours";
	// fileName = "image tilt 1";
	// getline(cin, fileName);				// 获得输入文件名
	srcFileName = fileName + fileType;	// 确定输入图片文件类型
	
	Mat src = imread(srcFileName, 0);	// 读取源图像
	czh_imageOpenDetect(src, fileName, fileType);	// 检测源图像是否成功打开
	
	// 显示源图像信息
	cout << "\nInput image name:\t" << srcFileName << endl
		<< "Image size:\t\t" << src.cols << "*" << src.rows << endl
		<< "Image pixels:\t\t" << src.cols*src.rows << endl
		<< "Image type:\t\t" << src.type() << endl;

	// 做阈值分割，将图像转换为二值图像
	Mat srcImage;
	threshold(src, srcImage, 150, 255, 0);	// 阈值分割
											
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
	
	//...............................................//
	//												 //
	//				       STEP.1                    //
	//			      霍夫变换检测直线	     	   	 //
	//												 //
	//...............................................//

	Mat edgeImage;	// 存储边缘图像
	Canny(srcImage, edgeImage, 50, 200, 3);	// 使用Canny算子寻找边缘
	imshow("canny", edgeImage);
	// 将得到的边缘图像进行霍夫线变换，将得到的线参数放置于 lineVector 向量之中
	vector<Vec2f> lineVector;
	czh_myHoughLines(edgeImage, lineVector, 1, CV_PI / 180, 120, 20, 20);

	// 在 lineImage 之中绘制侦测到的线
	Mat lineImage(srcImage.size(), CV_8UC1, Scalar::all(0));	// 后续操作需要的线图像
	Mat lineShowImage(srcImage.size(), CV_8UC3, Scalar::all(0));// 直观观察的线图像+二值图像

	// 将二值图像的值赋给一个三通道图像 lineShowImage，以便于绘制红色线条
	MatIterator_<Vec3b> itVec3bBegin, itVec3bEnd;	MatIterator_<uchar> itSrc;
	for (itVec3bBegin = lineShowImage.begin<Vec3b>(), itVec3bEnd = lineShowImage.end<Vec3b>(), itSrc = srcImage.begin<uchar>(); itVec3bBegin != itVec3bEnd; itVec3bBegin++,itSrc++)
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
		line(lineImage, pt1, pt2, Scalar(255, 255, 255), 1, 8); // 在线条图像中绘制侦测到的线条（白色）
		line(lineShowImage, pt1, pt2, Scalar(0, 0, 255), 1, CV_AA);	// 在二值图像中绘制侦测到的线条（红色）
	}

	time = ((double)getTickCount() - time) / getTickFrequency();
	cout << "STEP.1 霍夫线变换 finished :\t" << time << " second used.\n" << endl;
	
	//...............................................//
	//												 //
	//				       STEP.2                    //
	//			    根据直线图像检测交点	   	   	 //
	//												 //
	//...............................................//
	time = static_cast<double>(getTickCount());

	Mat potentialPointImage(srcImage.size(), CV_8UC1, Scalar::all(0));

	uchar *rowCur, *rowTop, *rowBot, *ptrDst;	// 当前行，上一行，下一行的行指针
	for (int i = 1; i < lineImage.rows - 1; i++)
	{	// 遍历得到的线图像每个像素，寻找可能是交点的点
		rowTop = lineImage.ptr<uchar>(i-1);
		rowCur = lineImage.ptr<uchar>(i);
		rowBot = lineImage.ptr<uchar>(i+1);
		ptrDst = potentialPointImage.ptr<uchar>(i);
		for (int j = 1; j < lineImage.cols - 1; j++)	// 遍历 lineImage 中每一个像素
		{
			if (rowCur[j] == 255)	// 如果当前像素是前景像素
			{	// 如果它8个邻域中有且只有4个非零像素，则可能是交点
				if((int)(rowCur[j - 1]+ rowCur[j + 1]+ rowTop[j - 1] + rowTop[j] + rowTop[j + 1] + rowBot[j - 1] + rowBot[j] + rowBot[j + 1]) == 1020)
					ptrDst[j] = 255;
			}
		}
	}

	// 对已经找到的交点图像做连通域标记，以便于下一步的重心查找
	Mat labelledPointImage;
	czh_labeling(potentialPointImage, labelledPointImage);

	time = ((double)getTickCount() - time) / getTickFrequency();
	cout << "STEP.2 交点初步判定+连通域标记 finished :\t" << time << " second used.\n" << endl;

	//...............................................//
	//												 //
	//				     STEP.3                      //
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
	//				     STEP.4                      //
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
	cout << "Program finished :\t" << timeTotal << " second used.\n" << endl;

	// 测试区域
//  	imshow("Gray scale image", src);	// 显示原始灰度图像
	imshow("【二值图像以及线条】", lineShowImage);	// 显示二值图像
	imshow("【线检测效果】", lineImage);	// 显示线检测结果
    imshow("【潜在交点检测效果】", potentialPointImage);			// 显示最终结果图像
	imshow("【最终交点检测效果】", dstImage);	// 显示第三步效果图
// 		czh_imwrite(srcImage, "binary");	// 输出二值图像
  	czh_imwrite(lineImage, "Lines");
	czh_imwrite(potentialPointImage, "potentialPointImage");	// 输出点图像
	czh_imwrite(dstImage, "Final Point Image");	// 输出最终点图像

	waitKey();
	system("pause");
}

