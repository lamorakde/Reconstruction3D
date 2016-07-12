#include "czh_binary_CV.h"
#include "opencv2/opencv.hpp"
#include <iostream>

using namespace std;
using namespace cv;

int main()
{
	// ���������Ϣ
	string programInformation = "����һ����ͨ��ͼ�񣬸ó�����ȳ�ȡ��ɫ���ص㣬Ȼ���ͼ���������Ա�֤�ֲ��ϵ㣬Ȼ���ȡ����ͼ��ĹǼ�.";
	string fileType = ".ppm";
	czh_helpInformation(programInformation);

	// �õ�����ͼ��������Ϣ�����
	cout << "Enter the input image name without " << fileType << ": ";
	string fileName, srcFileName;
	getline(cin, fileName);				// ��������ļ���
	srcFileName = fileName + fileType;	// ȷ������ͼƬ�ļ�����
	Mat srcImage = imread(srcFileName);	// ��ȡԴͼ��
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
	// ׼������ Mat ����
	Mat dstImage(srcImage.size(), CV_8UC1);
	Mat redPixelsImage(srcImage.size(), CV_8UC1);
	Mat skeletonImage(srcImage.size(), CV_8UC1);
	// ��ȡ��ɫ���ص�
	czh_extractColor(srcImage, redPixelsImage, RED);
	// ���ڳ�ȡ�����ĺ�ɫ���صĶ�ֵͼ����������ȡ
	czh_thin_LUT(redPixelsImage, skeletonImage);
	// ���ƹ�����ȡͼ��Ŀ�����ͼ�� Mat ����
	skeletonImage.copyTo(dstImage);
	// ���ͼ���ļ�
	czh_imwrite(dstImage, fileName);


	//////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////////////
	// ���������Ϣ�������������ʱ��
	time = ((double)getTickCount() - time) / getTickFrequency();
	cout << "Program finished first:\t" << time << " second used.\n" << endl;

	// ��������
	imshow("Դͼ", srcImage);
	imshow("Ч��ͼ", dstImage);

	waitKey();
	system("pause");
}
