#include "opencv2/opencv.hpp"
#include <iostream>
#include <windows.h>
#include <fstream>
#include <math.h>
#include "czh_cv.h"

using namespace std;
using namespace cv;

#define WINDOW_NAME "����任"
#define WINDOW_NAME_BW "��ֵ�ָ�"
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
	// ���õ��ı�Եͼ����л����߱任�����õ����߲��������� lineVector ����֮��
	vector<Vec2f> lineVector;
	Mat houghSrcImage = g_houghSrcImage;
	czh_myHoughLines(houghSrcImage, lineVector, 1, CV_PI / 180, g_thresholdHough, g_thresholdRho, g_thresholdAngle);

	// �� lineImage ֮�л�����⵽����
	Mat tempLineImage(houghSrcImage.size(), CV_8UC1, Scalar::all(0));	// ����������Ҫ����ͼ��
	Mat lineShowImage(houghSrcImage.size(), CV_8UC3, Scalar::all(0));// ֱ�۹۲����ͼ��+��ֵͼ��

																	 // ����ֵͼ���ֵ����һ����ͨ��ͼ�� lineShowImage���Ա��ڻ��ƺ�ɫ����
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
		line(tempLineImage, pt1, pt2, Scalar(255, 255, 255), 1, 8); // ������ͼ���л�����⵽����������ɫ��
		line(lineShowImage, pt1, pt2, Scalar(0, 0, 255), 1, CV_AA);	// �ڶ�ֵͼ���л�����⵽����������ɫ��
	}

	cv::imshow(WINDOW_NAME, lineShowImage);	// ��ʾ�߼����
	lineImage = tempLineImage;	// ���ں�߼������ͼ��
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
	// ���������Ϣ
	string programInformation = "�ó����ʹ�û����߱任�����Զ�Ѱ����ʽ�궨ͼ�еĽǵ�.";
	string fileType = ".ppm";
	czh_helpInformation(programInformation);

	// �õ�����ͼ��������Ϣ�����
	std::cout << "Enter the input image name without " << fileType << ": ";
	string fileName, srcFileName;
	getline(cin, fileName);				// ��������ļ���
	srcFileName = fileName + fileType;	// ȷ������ͼƬ�ļ�����

	Mat src = imread(srcFileName, 0);	// ��ȡԴͼ��

	czh_imageOpenDetect(src, fileName, fileType);	// ���Դͼ���Ƿ�ɹ���
	// ��ʾԴͼ����Ϣ
	std::cout << "\nInput image name:\t" << srcFileName << endl
		<< "Image size:\t\t" << src.cols << "*" << src.rows << endl
		<< "Image pixels:\t\t" << src.cols*src.rows << endl
		<< "Image type:\t\t" << src.type() << endl;

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//																							    										//
	//													ͼ��Ԥ����																		    //
	//																															    		//
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	Mat grayImage;
	src.copyTo(grayImage);
	imshow("gray scale image", grayImage);

	// ��ʾ profile
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

	// ʱ����Ϣ����¼�����㷨ʹ��ʱ��
	double timeTotal = static_cast<double>(getTickCount());
	double time = static_cast<double>(getTickCount());

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//																																				  //
	//															��ʼ�㷨����																		  //
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
	//			      ����任���ֱ��	     	   	 //
	//												 //
	//...............................................//

	// ������ѡ����ѻ�������ֵ����ʼ
	g_houghResultImage.copyTo(g_houghSrcImage);
	namedWindow(WINDOW_NAME);
	g_thresholdHough = 130;
	g_thresholdAngle = 0;
	g_thresholdRho = 0;
	createTrackbar("Hough Threshold", WINDOW_NAME, &g_thresholdHough, g_thresholdHoughMax, on_Trackbar_Hough);
	createTrackbar("Angle", WINDOW_NAME, &g_thresholdAngle, g_thresholdAngleMax, on_Trackbar_Hough);
	createTrackbar("Rho", WINDOW_NAME, &g_thresholdRho, g_thresholdRhoMax, on_Trackbar_Hough);
	// ������ѡ����ѻ�������ֵ������

	// ��������������±ߵĲ���
	waitKey(0);

	//...............................................//
	//												 //
	//				       STEP.3                    //
	//			    ����ֱ��ͼ���⽻��	   	   	 //
	//												 //
	//...............................................//

	time = static_cast<double>(getTickCount());
	Mat potentialPointImage(srcImage.size(), CV_8UC1, Scalar::all(0));

	uchar *rowCur, *rowTop, *rowBot, *ptrDst;	// ��ǰ�У���һ�У���һ�е���ָ��
	for (int i = 1; i < lineImage.rows - 1; i++)
	{	// �����õ�����ͼ��ÿ�����أ�Ѱ�ҿ����ǽ���ĵ�
		rowTop = lineImage.ptr<uchar>(i - 1);
		rowCur = lineImage.ptr<uchar>(i);
		rowBot = lineImage.ptr<uchar>(i + 1);
		ptrDst = potentialPointImage.ptr<uchar>(i);
		for (int j = 1; j < lineImage.cols - 1; j++)	// ���� lineImage ��ÿһ������
			if (rowCur[j] == 255)	// �����ǰ������ǰ������
			{	// �����8������������ֻ��4���������أ�������ǽ���
				if ((int)(rowCur[j - 1] + rowCur[j + 1] + rowTop[j - 1] + rowTop[j] + rowTop[j + 1] + rowBot[j - 1] + rowBot[j] + rowBot[j + 1]) == 1020)
					ptrDst[j] = 255;
			}
	}
	// ���Ѿ��ҵ��Ľ���ͼ������ͨ���ǣ��Ա�����һ�������Ĳ���
	Mat labelledPointImage;
	czh_labeling(potentialPointImage, labelledPointImage);

	time = ((double)getTickCount() - time) / getTickFrequency();
	cout << "STEP.2 ��������ж�+��ͨ���� finished :\t" << time << " second used.\n" << endl;

	//...............................................//
	//												 //
	//				     STEP.4                      //
	//					Ѱ������					 //
	//												 //
	//...............................................//

	time = static_cast<double>(getTickCount());

	Mat dstImage(srcImage.size(), CV_8UC1, Scalar::all(0));
	vector<Point> centerPoints;
	czh_centerOfGravity(labelledPointImage, dstImage, centerPoints);	// Ѱ������

	time = ((double)getTickCount() - time) / getTickFrequency();
	cout << "STEP.2 ��������ж�+��ͨ���� finished :\t" << time << " second used.\n" << endl;

	//...............................................//
	//												 //
	//				     STEP.5                      //
	//				   ���������					 //
	//												 //
	//...............................................//

	// �ѽǵ��������Ϊ txt �ļ�
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

	// ���������Ϣ�������������ʱ��
	timeTotal = ((double)getTickCount() - timeTotal) / getTickFrequency();
	std::cout << "Program finished :\t" << timeTotal << " second used.\n" << endl;

	// ��������
	imshow("blur image", blurredImage);
	imshow("lineImage", lineImage);

	cv::waitKey();
	std::system("pause");
}