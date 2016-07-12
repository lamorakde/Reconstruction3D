#include "opencv2/opencv.hpp"
#include <iostream>
#include <windows.h>
#include <fstream>
#include <math.h>
#include "czh_binary_CV.h"

using namespace std;
using namespace cv;

#define WINDOW_NAME "����任"
#define WINDOW_NAME_BW "��ֵ�ָ�"

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
	// ���õ��ı�Եͼ����л����߱任�����õ����߲��������� lineVector ����֮��
	vector<Vec2f> lineVector;
	Mat houghSrcImage = srcImage;
	czh_myHoughLines(houghSrcImage, lineVector, 1, CV_PI / 180,g_thresholdHough,g_thresholdRho,g_thresholdAngle);

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

int main()
{
	// ���������Ϣ
	string programInformation = "�ó����ʹ�û����߱任�����Զ�Ѱ����ʽ�궨ͼ�еĽǵ�.";
	string fileType = ".ppm";
	czh_helpInformation(programInformation);

	// �õ�����ͼ��������Ϣ�����
	std::cout << "Enter the input image name without " << fileType << ": ";
	string fileName, srcFileName;
	fileName = "image 2";
	// getline(cin, fileName);				// ��������ļ���
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
	src.copyTo(srcImage);
// 	imshow("gray scale image", srcImage);
// 
// 	// ��ʾ profile
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
	

	// ʱ����Ϣ����¼�����㷨ʹ��ʱ��
	double timeTotal = static_cast<double>(getTickCount());
	double time = static_cast<double>(getTickCount());

	uchar *ptr_uDst;	// uchar ����ָ��
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
	//		# 1 ģ������Լ���ز���
	///////////////////////////////////////////////////
 	int M_vertical[7][3] = { { high,low,high },{ high,low,high },{ high,low,high },{ high,low,high },{ high,low,high },{ high,low,high },{ high,low,high } };
 	
 	for (int i = 0; i < 7; i++)
 		for (int j = 0; j < 3; j++)
 		{
 			total += M_vertical[i][j];
 		}
 	float mean_M = total / 21;	// M_vertical �ľ�ֵ
 
 	total = 0;
 	for (int i = 0; i < 7; i++)
 		for (int j = 0; j < 3; j++)
 		{
 			total += (M_vertical[i][j] - mean_M)*(M_vertical[i][j] - mean_M);
 		}
 
 	float var_M = total / 20;		// M_vertical �ķ���var�������ĸ�� 21 - 1
 	float sigma_M = sqrt(var_M);	// M_vertical �ı�׼�� sigma

	/////////////////////////////////////////////////
	//		# 2 ��ԭ�Ҷ�ͼ��Ƕ��ģ����м���
	/////////////////////////////////////////////////

	// ��������ʼ�� rho ����������ͼ��ͬ���ĳߴ�
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
 			// ��ǰ I �ϵĵ�����ģ��֮�µ�ƽ��ֵ
 			float mean_I = total / 21;
 			
 			// ��ǰ I �ϵĵ�����ģ��֮�µķ���ͱ�׼��
 			total = 0;
 			for (int win_row = -3; win_row <= 3; win_row++)
 				for (int win_cols = -1; win_cols <= 1; win_cols++)
 				{
 					total += (srcImage.at<uchar>(i + win_row, j + win_cols) - mean_I)*(srcImage.at<uchar>(i + win_row, j + win_cols) - mean_I);
 				}
 			float var_I = total / 20;	// �����ĸ�� 21 - 1
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
 
 	// Ѱ�� rhoVertical �������Сֵ
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
 
 	// ��һ��
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
	//		# 1 ģ������Լ���ز���
	///////////////////////////////////////////////////

	int M_horizontal[3][7] = { { high,high,high,high,high,high,high },{ low,low,low,low,low,low,low },{ high,high,high,high,high,high,high } };

	///////////////////////////////////////////////////
	//		# 2 ��ԭ�Ҷ�ͼ��Ƕ��ģ����м���
	///////////////////////////////////////////////////

	// ��������ʼ�� rho ����������ͼ��ͬ���ĳߴ�
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
			// ��ǰ I �ϵĵ�����ģ��֮�µ�ƽ��ֵ
			float mean_I = total / 21;

			// ��ǰ I �ϵĵ�����ģ��֮�µķ���ͱ�׼��
			total = 0;
			for (int win_row = -1; win_row <= 1; win_row++)
				for (int win_cols = -3; win_cols <= 3; win_cols++)
				{
					total += (srcImage.at<uchar>(i + win_row, j + win_cols) - mean_I)*(srcImage.at<uchar>(i + win_row, j + win_cols) - mean_I);
				}
			float var_I = total / 20;	// �����ĸ�� 21 - 1
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

	// Ѱ�� rhoVertical �������Сֵ
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

	// ��һ��
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
	//				        ����		    	   	 //
	//												 //
	//...............................................//

	///////////////////////////////////////////////////
	//		# 1 ģ������Լ���ز���
	///////////////////////////////////////////////////
	int M_topRight[9][9] = { { 0,0,0,0,0,0,high,0,0 },{ 0,0,0,0,0,high,0,low,0 },{ 0,0,0,0,high,0,low,0,high },
							 { 0,0,0,high,0,low,0,high,0 },{ 0,0,high,0,low,0,high,0,0 },{ 0,high,0,low,0,high,0,0,0 },
							 { high,0,low,0,high,0,0,0,0 },{ 0,low,0,high,0,0,0,0,0 },{ 0,0,high,0,0,0,0,0,0 } };
 
	///////////////////////////////////////////////////
	//		# 2 ��ԭ�Ҷ�ͼ��Ƕ��ģ����м���
	///////////////////////////////////////////////////

	// ��������ʼ�� rho ����������ͼ��ͬ���ĳߴ�
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
			// ��ǰ I �ϵĵ�����ģ��֮�µ�ƽ��ֵ
			float mean_I = total / 21;

			// ��ǰ I �ϵĵ�����ģ��֮�µķ���ͱ�׼��
			total = 0;
			for (int win_row = -4; win_row <= 4; win_row++)
				for (int win_cols = -4; win_cols <= 4; win_cols++)
				{
					if (M_topRight[win_row + 4][win_cols + 4] != 0)
						total += (srcImage.at<uchar>(i + win_row, j + win_cols) - mean_I)*(srcImage.at<uchar>(i + win_row, j + win_cols) - mean_I);
				}
			float var_I = total / 20;	// �����ĸ�� 21 - 1
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

	// Ѱ�� rhoTopRight �������Сֵ
	// ��Ҫ�޸�
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

	// ��һ��
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
	//				        ����		    	   	 //
	//												 //
	//...............................................//

	///////////////////////////////////////////////////
	//		# 1 ģ������Լ���ز���
	///////////////////////////////////////////////////
 	int M_topLeft[9][9] = { { 0,0,high,0,0,0,0,0,0 },{ 0,low,0,high,0,0,0,0,0 },{ high,0,low,0,high,0,0,0,0 },
							{ 0,high,0,low,0,high,0,0,0 },{ 0,0,high,0,low,0,high,0,0 },{ 0,0,0,high,0,low,0,high,0 } ,
							{ 0,0,0,0,high,0,low,0,high } ,{ 0,0,0,0,0,high,0,low,0 } ,{ 0,0,0,0,0,0,high,0,0 } };

	///////////////////////////////////////////////////
	//		# 2 ��ԭ�Ҷ�ͼ��Ƕ��ģ����м���
	///////////////////////////////////////////////////

	// ��������ʼ�� rho ����������ͼ��ͬ���ĳߴ�
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
			// ��ǰ I �ϵĵ�����ģ��֮�µ�ƽ��ֵ
			float mean_I = total / 21;
			
			// ��ǰ I �ϵĵ�����ģ��֮�µķ���ͱ�׼��
			total = 0;
			for (int win_row = -4; win_row <= 4; win_row++)
				for (int win_cols = -4; win_cols <= 4; win_cols++)
				{
					if (M_topLeft[win_row + 4][win_cols + 4] != 0)
						total += (srcImage.at<uchar>(i + win_row, j + win_cols) - mean_I)*(srcImage.at<uchar>(i + win_row, j + win_cols) - mean_I);
				}
			float var_I = total / 20;	// �����ĸ�� 21 - 1
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
	
	// Ѱ�� rhoTopRight �������Сֵ
	// ��Ҫ�޸�
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
	// ��һ��
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
	//				      ����pi/8		    	   	 //
	//												 //
	//...............................................//

	///////////////////////////////////////////////////
	//		# 1 ģ������Լ���ز���
	///////////////////////////////////////////////////
	int M_topRight_225[7][7] = { { 0,0,0,0,0,high,high },{ 0,0,high,high,high,low,low },{ high,high,low,low,low,high,high },
								 { low,low,high,high,high,0,0 },{ high,high,0,0,0,0,0 },{ 0,0,0,0,0,0,0 },
								 { 0,0,0,0,0,0,0 } };
  
 	///////////////////////////////////////////////////
 	//		# 2 ��ԭ�Ҷ�ͼ��Ƕ��ģ����м���
 	///////////////////////////////////////////////////
	
	// ��������ʼ�� rho ����������ͼ��ͬ���ĳߴ�
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
			// ��ǰ I �ϵĵ�����ģ��֮�µ�ƽ��ֵ
			float mean_I = total / 21;

			// ��ǰ I �ϵĵ�����ģ��֮�µķ���ͱ�׼��
			total = 0;
			for (int win_row = -3; win_row <= 3; win_row++)
				for (int win_cols = -3; win_cols <= 3; win_cols++)
				{
					if (M_topRight_225[win_row + 3][win_cols + 3] != 0)
						total += (srcImage.at<uchar>(i + win_row, j + win_cols) - mean_I)*(srcImage.at<uchar>(i + win_row, j + win_cols) - mean_I);
				}
			float var_I = total / 20;	// �����ĸ�� 21 - 1
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

	// Ѱ�� rhoTopRight �������Сֵ
	// ��Ҫ�޸�
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

	// ��һ��
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
	//				      ����pi/8		    	   	 //
	//												 //
	//...............................................//

	///////////////////////////////////////////////////
	//		# 1 ģ������Լ���ز���
	///////////////////////////////////////////////////
	int M_topLeft_225[7][7] = { { high,high,0,0,0,0,0 },{ low,low,high,high,high,0,0 },{ high,high,low,low,low,high,high },
								 { 0,0,high,high,high,low,low },{ 0,0,0,0,0,high,high },
								 { 0,0,0,0,0,0,0 },{ 0,0,0,0,0,0,0 } };

	///////////////////////////////////////////////////
	//		# 2 ��ԭ�Ҷ�ͼ��Ƕ��ģ����м���
	///////////////////////////////////////////////////

	// ��������ʼ�� rho ����������ͼ��ͬ���ĳߴ�
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
		// ��ǰ I �ϵĵ�����ģ��֮�µ�ƽ��ֵ
		float mean_I = total / 21;

		// ��ǰ I �ϵĵ�����ģ��֮�µķ���ͱ�׼��
		total = 0;
		for (int win_row = -3; win_row <= 3; win_row++)
			for (int win_cols = -3; win_cols <= 3; win_cols++)
			{
				if (M_topLeft_225[win_row + 3][win_cols + 3] != 0)
					total += (srcImage.at<uchar>(i + win_row, j + win_cols) - mean_I)*(srcImage.at<uchar>(i + win_row, j + win_cols) - mean_I);
			}
		float var_I = total / 20;	// �����ĸ�� 21 - 1
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

	// Ѱ�� rhoTopRight �������Сֵ
	// ��Ҫ�޸�
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

	// ��һ��
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
	//				     ���� 3pi/8		    	   	 //
	//												 //
	//...............................................//

	///////////////////////////////////////////////////
	//		# 1 ģ������Լ���ز���
	///////////////////////////////////////////////////
	int M_topRight_675[7][7] = { { 0,0,high,low,high,0,0 },{ 0,0,high,low,high,0,0 },{ 0,high,low,high,0,0,0 },
								 { 0,high,low,high,0,0,0 },{ 0,high,low,high,0,0,0 },
								 { high,low,high,0,0,0,0 },{ high,low,high,0,0,0,0 } };

	///////////////////////////////////////////////////
	//		# 2 ��ԭ�Ҷ�ͼ��Ƕ��ģ����м���
	///////////////////////////////////////////////////

	// ��������ʼ�� rho ����������ͼ��ͬ���ĳߴ�
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
			// ��ǰ I �ϵĵ�����ģ��֮�µ�ƽ��ֵ
			float mean_I = total / 21;

			// ��ǰ I �ϵĵ�����ģ��֮�µķ���ͱ�׼��
			total = 0;
			for (int win_row = -3; win_row <= 3; win_row++)
				for (int win_cols = -3; win_cols <= 3; win_cols++)
				{
					if (M_topRight_675[win_row + 3][win_cols + 3] != 0)
						total += (srcImage.at<uchar>(i + win_row, j + win_cols) - mean_I)*(srcImage.at<uchar>(i + win_row, j + win_cols) - mean_I);
				}
			float var_I = total / 20;	// �����ĸ�� 21 - 1
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

	// Ѱ�� rhoTopRight �������Сֵ
	// ��Ҫ�޸�
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

	// ��һ��
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
	//				     ���� 3pi/8		    	   	 //
	//												 //
	//...............................................//

	///////////////////////////////////////////////////
	//		# 1 ģ������Լ���ز���
	///////////////////////////////////////////////////
	int M_topLeft_675[7][7] = { { 0,0,high,low,high,0,0 },{ 0,0,high,low,high,0,0 },
								{ 0,0,0,high,low,high,0 },{ 0,0,0,high,low,high,0 },{ 0,0,0,high,low,high,0 },
								{ 0,0,0,0,high,low,high},{ 0,0,0,0,high,low,high } };

	///////////////////////////////////////////////////
	//		# 2 ��ԭ�Ҷ�ͼ��Ƕ��ģ����м���
	///////////////////////////////////////////////////
	// ��������ʼ�� rho ����������ͼ��ͬ���ĳߴ�
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
			// ��ǰ I �ϵĵ�����ģ��֮�µ�ƽ��ֵ
			float mean_I = total / 21;

			// ��ǰ I �ϵĵ�����ģ��֮�µķ���ͱ�׼��
			total = 0;
			for (int win_row = -3; win_row <= 3; win_row++)
				for (int win_cols = -3; win_cols <= 3; win_cols++)
				{
					if (M_topLeft_675[win_row + 3][win_cols + 3] != 0)
						total += (srcImage.at<uchar>(i + win_row, j + win_cols) - mean_I)*(srcImage.at<uchar>(i + win_row, j + win_cols) - mean_I);
				}
			float var_I = total / 20;	// �����ĸ�� 21 - 1
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

	// Ѱ�� rhoTopRight �������Сֵ
	// ��Ҫ�޸�
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
	// ��һ��
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

	// ��ʾ profile
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

	// ��ȥ�����
	Mat denoisedImage;
	czh_bwAreaOpen(bwImage, denoisedImage, 50, 100, 5);
	imshow("ȥ��ͼ��", denoisedImage);



	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//																																				  //
	//															��ʼ�㷨����																		  //
	//																																				  //
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//...............................................//
	//												 //
	//				       STEP.1                    //
	//			      ����任���ֱ��	     	   	 //
	//												 //
	//...............................................//

// 	// ������ѡ����ѻ�������ֵ����ʼ
// 	srcImage = denoisedImage;
// 	namedWindow(WINDOW_NAME);
// 	g_thresholdHough = 130;
// 	g_thresholdAngle = 0;
// 	g_thresholdRho = 0;
// 	createTrackbar("Hough Threshold", WINDOW_NAME, &g_thresholdHough, g_thresholdHoughMax, on_Trackbar_Hough);
// 	createTrackbar("Angle", WINDOW_NAME, &g_thresholdAngle, g_thresholdAngleMax, on_Trackbar_Hough);
// 	createTrackbar("Rho", WINDOW_NAME, &g_thresholdRho, g_thresholdRhoMax, on_Trackbar_Hough);
// 	// ������ѡ����ѻ�������ֵ������
// 
// 	// ��������������±ߵĲ���
// 	waitKey(0);	
// 	imshow("lineImage", lineImage);
	 
	//...............................................//
	//												 //
	//				       STEP.2                    //
	//			    ����ֱ��ͼ���⽻��	   	   	 //
	//												 //
	//...............................................//
	
// 	time = static_cast<double>(getTickCount());
// 	
// 	Mat potentialPointImage(srcImage.size(), CV_8UC1, Scalar::all(0));
// 	
// 	uchar *rowCur, *rowTop, *rowBot, *ptrDst;	// ��ǰ�У���һ�У���һ�е���ָ��
// 	for (int i = 1; i < lineImage.rows - 1; i++)
// 	{	// �����õ�����ͼ��ÿ�����أ�Ѱ�ҿ����ǽ���ĵ�
// 		rowTop = lineImage.ptr<uchar>(i - 1);
// 		rowCur = lineImage.ptr<uchar>(i);
// 		rowBot = lineImage.ptr<uchar>(i + 1);
// 		ptrDst = potentialPointImage.ptr<uchar>(i);
// 		for (int j = 1; j < lineImage.cols - 1; j++)	// ���� lineImage ��ÿһ������
// 		{
// 			if (rowCur[j] == 255)	// �����ǰ������ǰ������
// 			{	// �����8������������ֻ��4���������أ�������ǽ���
// 				if ((int)(rowCur[j - 1] + rowCur[j + 1] + rowTop[j - 1] + rowTop[j] + rowTop[j + 1] + rowBot[j - 1] + rowBot[j] + rowBot[j + 1]) == 1020)
// 					ptrDst[j] = 255;
// 			}
// 		}
// 	}
// 	// ���Ѿ��ҵ��Ľ���ͼ������ͨ���ǣ��Ա�����һ�������Ĳ���
// 	Mat labelledPointImage;
// 	czh_labeling(potentialPointImage, labelledPointImage);
// 	
// 	time = ((double)getTickCount() - time) / getTickFrequency();
// 	cout << "STEP.2 ��������ж�+��ͨ���� finished :\t" << time << " second used.\n" << endl;

// 	//...............................................//
// 	//												 //
// 	//				     STEP.3                      //
// 	//					Ѱ������					 //
// 	//												 //
// 	//...............................................//
// 	
// 	time = static_cast<double>(getTickCount());
// 	
// 	Mat dstImage(srcImage.size(), CV_8UC1, Scalar::all(0));
// 	vector<Point> centerPoints;
// 	czh_centerOfGravity(labelledPointImage, dstImage, centerPoints);	// Ѱ������
// 	
// 	time = ((double)getTickCount() - time) / getTickFrequency();
// 	cout << "STEP.2 ��������ж�+��ͨ���� finished :\t" << time << " second used.\n" << endl;
// 	
// 	
// 	//...............................................//
// 	//												 //
// 	//				     STEP.4                      //
// 	//				   ���������					 //
// 	//												 //
// 	//...............................................//
// 	
// 	// �ѽǵ��������Ϊ txt �ļ�
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

	// ���������Ϣ�������������ʱ��
	timeTotal = ((double)getTickCount() - timeTotal) / getTickFrequency();
	std::cout << "Program finished :\t" << timeTotal << " second used.\n" << endl;

	// ��������
//	imshow("Gray scale image", src);	// ��ʾԭʼ�Ҷ�ͼ��
//	imshow("����ֵͼ���Լ�������", lineShowImage);	// ��ʾ��ֵͼ��
//	imshow("���߼��Ч����", lineImage);	// ��ʾ�߼����
// 	imshow("��Ǳ�ڽ�����Ч����", potentialPointImage);			// ��ʾ���ս��ͼ��
// 	imshow("�����ս�����Ч����", dstImage);	// ��ʾ������Ч��ͼ

	cv::waitKey();
	std::system("pause");
}

// Back up 
/*
void backUp_method_one()
{
	// �������ҷ���һ����ʼ
	Mat dilatedImage, closedImage;
	Mat element = getStructuringElement(MORPH_RECT, Size(3, 3));	// ��̬ѧ����Ԫ��
																	// ��Դ�Ҷ�ͼ�����ղ������������ͣ�������ʴ
	dilate(src, dilatedImage, element);	// ��Ԫ�Ҷ�ͼ������
	erode(dilatedImage, closedImage, element);	// ������ͼ����ʴ���õ���ͼ��

												// �õõ��ı�ͼ���ȥԭ�Ҷ�ͼ���õ��궨�̷�������
	src.copyTo(srcImage);
	imshow("ԭʼ�Ҷ�ͼ", srcImage);
	MatIterator_<uchar> itUcharBegin, itUcharEnd, itDst, itSrc;
	for (itUcharBegin = closedImage.begin<uchar>(), itUcharEnd = closedImage.end<uchar>(), itDst = srcImage.begin<uchar>(), itSrc = src.begin<uchar>(); itUcharBegin != itUcharEnd; itUcharBegin++, itDst++, itSrc++)
	{
		(*itDst) = (*itUcharBegin) - (*itSrc);
	}
	imshow("Method # 1", srcImage);
	threshold(srcImage, srcImage, 100, 255, CV_THRESH_OTSU);	// Otsu��ֵ�ָ�
	imshow("Otsu # 1", srcImage);
	// �������ҷ���һ������
}

void backUp_method_two()
{
	// �������ҷ���������ʼ
	Mat dilatedImage1, dilatedImage2, dilatedImage3, dilatedImage4;
	Mat closedImage1, closedImage2, closedImage3, closedImage4;
	Mat element1(3, 5, CV_8UC1, Scalar::all(255));	// ��̬ѧ����Ԫ��# 1
	Mat element2(7, 7, CV_8UC1, Scalar::all(0));	// ��̬ѧ����Ԫ��# 2
	Mat element3(5, 3, CV_8UC1, Scalar::all(255));	// ��̬ѧ����Ԫ��# 3
	Mat element4(7, 7, CV_8UC1, Scalar::all(0));	// ��̬ѧ����Ԫ��# 4

													// ��̬ѧ����Ԫ��# 2 ��ֵ
	element2.at<uchar>(0, 4) = 255;
	element2.at<uchar>(1, 3) = element2.at<uchar>(1, 5) = 255;
	element2.at<uchar>(2, 2) = element2.at<uchar>(2, 4) = element2.at<uchar>(2, 6) = 255;
	element2.at<uchar>(3, 1) = element2.at<uchar>(3, 3) = element2.at<uchar>(3, 5) = 255;
	element2.at<uchar>(4, 0) = element2.at<uchar>(4, 2) = element2.at<uchar>(4, 4) = 255;
	element2.at<uchar>(5, 1) = element2.at<uchar>(5, 3) = 255;
	element2.at<uchar>(6, 2) = 255;

	// ��̬ѧ����Ԫ��# 4 ��ֵ
	element4.at<uchar>(0, 2) = 255;
	element4.at<uchar>(1, 1) = element4.at<uchar>(1, 3) = 255;
	element4.at<uchar>(2, 0) = element4.at<uchar>(2, 2) = element4.at<uchar>(2, 4) = 255;
	element4.at<uchar>(3, 1) = element4.at<uchar>(3, 3) = element4.at<uchar>(3, 5) = 255;
	element4.at<uchar>(4, 2) = element4.at<uchar>(4, 4) = element4.at<uchar>(4, 6) = 255;
	element4.at<uchar>(5, 3) = element4.at<uchar>(5, 5) = 255;
	element4.at<uchar>(6, 4) = 255;

	// ��Դ�Ҷ�ͼ�����ղ������������ͣ�������ʴ
	//������# 1
	dilate(src, dilatedImage1, element1);	// ��Ԫ�Ҷ�ͼ������
	erode(dilatedImage1, closedImage1, element1);	// ������ͼ����ʴ���õ���ͼ��
													//������# 2
	dilate(src, dilatedImage2, element2);	// ��Ԫ�Ҷ�ͼ������
	erode(dilatedImage2, closedImage2, element2);	// ������ͼ����ʴ���õ���ͼ��
													//������# 3
	dilate(src, dilatedImage3, element3);	// ��Ԫ�Ҷ�ͼ������
	erode(dilatedImage3, closedImage3, element3);	// ������ͼ����ʴ���õ���ͼ��
													//������# 4
	dilate(src, dilatedImage4, element4);	// ��Ԫ�Ҷ�ͼ������
	erode(dilatedImage4, closedImage4, element4);	// ������ͼ����ʴ���õ���ͼ��

													// Ѱ��4��ͼ���е����� Max
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

	// �õõ��ı�ͼ���ȥԭ�Ҷ�ͼ���õ��궨�̷�������
	src.copyTo(srcImage);
	for (itUcharBegin = maxImage.begin<uchar>(), itUcharEnd = maxImage.end<uchar>(), itDst = srcImage.begin<uchar>(), itSrc = src.begin<uchar>(); itUcharBegin != itUcharEnd; itUcharBegin++, itDst++, itSrc++)
	{
		(*itDst) = (*itUcharBegin) - (*itSrc);
	}
	imshow("Method # 2", srcImage);
	threshold(srcImage, srcImage, 100, 255, CV_THRESH_OTSU);	// Otsu��ֵ�ָ�
	imshow("Otsu # 2", srcImage);
	// �������ҷ�����������
}