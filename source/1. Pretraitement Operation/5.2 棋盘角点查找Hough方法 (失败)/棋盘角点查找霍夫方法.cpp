#include "opencv2/opencv.hpp"
#include <iostream>
#include <windows.h>
#include <fstream>
#include "czh_binary_CV.h"

using namespace std;
using namespace cv;

int main()
{
	// ���������Ϣ
	string programInformation = "�ó����ʹ�û����߱任�����Զ�Ѱ�����̵ȱ궨ͼ�еĽǵ�.";
	string fileType = ".ppm";
	czh_helpInformation(programInformation);

	// �õ�����ͼ��������Ϣ�����
	cout << "Enter the input image name without " << fileType << ": ";
	string fileName, srcFileName;
	fileName = "contours";
	// fileName = "image tilt 1";
	// getline(cin, fileName);				// ��������ļ���
	srcFileName = fileName + fileType;	// ȷ������ͼƬ�ļ�����
	
	Mat src = imread(srcFileName, 0);	// ��ȡԴͼ��
	czh_imageOpenDetect(src, fileName, fileType);	// ���Դͼ���Ƿ�ɹ���
	
	// ��ʾԴͼ����Ϣ
	cout << "\nInput image name:\t" << srcFileName << endl
		<< "Image size:\t\t" << src.cols << "*" << src.rows << endl
		<< "Image pixels:\t\t" << src.cols*src.rows << endl
		<< "Image type:\t\t" << src.type() << endl;

	// ����ֵ�ָ��ͼ��ת��Ϊ��ֵͼ��
	Mat srcImage;
	threshold(src, srcImage, 150, 255, 0);	// ��ֵ�ָ�
											
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
	
	//...............................................//
	//												 //
	//				       STEP.1                    //
	//			      ����任���ֱ��	     	   	 //
	//												 //
	//...............................................//

	Mat edgeImage;	// �洢��Եͼ��
	Canny(srcImage, edgeImage, 50, 200, 3);	// ʹ��Canny����Ѱ�ұ�Ե
	imshow("canny", edgeImage);
	// ���õ��ı�Եͼ����л����߱任�����õ����߲��������� lineVector ����֮��
	vector<Vec2f> lineVector;
	czh_myHoughLines(edgeImage, lineVector, 1, CV_PI / 180, 120, 20, 20);

	// �� lineImage ֮�л�����⵽����
	Mat lineImage(srcImage.size(), CV_8UC1, Scalar::all(0));	// ����������Ҫ����ͼ��
	Mat lineShowImage(srcImage.size(), CV_8UC3, Scalar::all(0));// ֱ�۹۲����ͼ��+��ֵͼ��

	// ����ֵͼ���ֵ����һ����ͨ��ͼ�� lineShowImage���Ա��ڻ��ƺ�ɫ����
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
		line(lineImage, pt1, pt2, Scalar(255, 255, 255), 1, 8); // ������ͼ���л�����⵽����������ɫ��
		line(lineShowImage, pt1, pt2, Scalar(0, 0, 255), 1, CV_AA);	// �ڶ�ֵͼ���л�����⵽����������ɫ��
	}

	time = ((double)getTickCount() - time) / getTickFrequency();
	cout << "STEP.1 �����߱任 finished :\t" << time << " second used.\n" << endl;
	
	//...............................................//
	//												 //
	//				       STEP.2                    //
	//			    ����ֱ��ͼ���⽻��	   	   	 //
	//												 //
	//...............................................//
	time = static_cast<double>(getTickCount());

	Mat potentialPointImage(srcImage.size(), CV_8UC1, Scalar::all(0));

	uchar *rowCur, *rowTop, *rowBot, *ptrDst;	// ��ǰ�У���һ�У���һ�е���ָ��
	for (int i = 1; i < lineImage.rows - 1; i++)
	{	// �����õ�����ͼ��ÿ�����أ�Ѱ�ҿ����ǽ���ĵ�
		rowTop = lineImage.ptr<uchar>(i-1);
		rowCur = lineImage.ptr<uchar>(i);
		rowBot = lineImage.ptr<uchar>(i+1);
		ptrDst = potentialPointImage.ptr<uchar>(i);
		for (int j = 1; j < lineImage.cols - 1; j++)	// ���� lineImage ��ÿһ������
		{
			if (rowCur[j] == 255)	// �����ǰ������ǰ������
			{	// �����8������������ֻ��4���������أ�������ǽ���
				if((int)(rowCur[j - 1]+ rowCur[j + 1]+ rowTop[j - 1] + rowTop[j] + rowTop[j + 1] + rowBot[j - 1] + rowBot[j] + rowBot[j + 1]) == 1020)
					ptrDst[j] = 255;
			}
		}
	}

	// ���Ѿ��ҵ��Ľ���ͼ������ͨ���ǣ��Ա�����һ�������Ĳ���
	Mat labelledPointImage;
	czh_labeling(potentialPointImage, labelledPointImage);

	time = ((double)getTickCount() - time) / getTickFrequency();
	cout << "STEP.2 ��������ж�+��ͨ���� finished :\t" << time << " second used.\n" << endl;

	//...............................................//
	//												 //
	//				     STEP.3                      //
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
	//				     STEP.4                      //
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
	cout << "Program finished :\t" << timeTotal << " second used.\n" << endl;

	// ��������
//  	imshow("Gray scale image", src);	// ��ʾԭʼ�Ҷ�ͼ��
	imshow("����ֵͼ���Լ�������", lineShowImage);	// ��ʾ��ֵͼ��
	imshow("���߼��Ч����", lineImage);	// ��ʾ�߼����
    imshow("��Ǳ�ڽ�����Ч����", potentialPointImage);			// ��ʾ���ս��ͼ��
	imshow("�����ս�����Ч����", dstImage);	// ��ʾ������Ч��ͼ
// 		czh_imwrite(srcImage, "binary");	// �����ֵͼ��
  	czh_imwrite(lineImage, "Lines");
	czh_imwrite(potentialPointImage, "potentialPointImage");	// �����ͼ��
	czh_imwrite(dstImage, "Final Point Image");	// ������յ�ͼ��

	waitKey();
	system("pause");
}

