#include "czh_binary_CV.h"
#include "opencv2/opencv.hpp"
#include <iostream>
#include <vector>

using namespace std;
using namespace cv;

int main()
{
	// ���������Ϣ
	string programInformation = "�ó����Ϊ��ֵͼ������ͨ���ǲ���: two - pass method.";
	string fileType = ".ppm";
	czh_helpInformation(programInformation);

	// �õ�����ͼ��������Ϣ�����
	cout << "Enter the input image name without " << fileType << ": ";
	string fileName, srcFileName;
	getline(cin, fileName);				// ��������ļ���
	srcFileName = fileName + fileType;	// ȷ������ͼƬ�ļ�����
	Mat srcImage = imread(srcFileName, 0);	// ��ȡԴͼ��
	czh_imageOpenDetect(srcImage, fileName, fileType);	// ���Դͼ���Ƿ�ɹ���
	// ��ʾԴͼ����Ϣ
	cout << "\nInput image name:\t" << srcFileName << endl
		<< "Image size:\t\t" << srcImage.cols << "*" << srcImage.rows << endl
		<< "Image pixels:\t\t" << srcImage.cols*srcImage.rows << endl
		<< "Image type:\t\t" << srcImage.type() << endl;

	// ʱ����Ϣ����¼�����㷨ʹ��ʱ��
	double time = static_cast<double>(getTickCount());

	//////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////////////
	// ��ʼ�㷨����
	
	
	imshow("bn", srcImage);


	Mat dstImage(srcImage.size(), CV_8UC1);
	czh_labeling(srcImage, dstImage);


	
	//////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////////////

	// ���������Ϣ�������������ʱ��
	time = ((double)getTickCount() - time) / getTickFrequency();
	cout << "Program finished first:\t" << time << " second used.\n" << endl;

	// ��������
	imshow("srcImage", srcImage);
	imshow("dstImage", dstImage);
	czh_imwrite(dstImage, "labeling");

	waitKey();
	system("pause");
}