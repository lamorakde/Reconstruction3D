#include "czh_binary_CV.h"
#include <opencv2/core.hpp>
#include <iostream>
#include <fstream>
using namespace std;
using namespace cv;

int main()
{
	// ������Ϣ��������
	cout << "************************************************************************************************************************\n";
	cout << "\t\t\tProgram information:\n";
	cout << "\t\t����һ����ͨ��ͼ�񣬸ó�����ȳ�ȡ��ɫ���ص㣬Ȼ���ͼ���������Ա�֤�ֲ��ϵ㣬Ȼ���ȡ����ͼ��ĹǼ�\n\n";
	cout << "************************************************************************************************************************\n\n";
	// �õ�����ͼ��������Ϣ�����
	cout << "Enter the input image name:";
	string fileName, srcFileName;
	getline(cin, fileName);
	srcFileName = fileName + ".ppm";
	Mat srcImage = imread(srcFileName);
	cout << "Input image name:\t" << srcFileName << endl
		 << "Image size:\t\t" << srcImage.cols << "*" << srcImage.rows << endl
		 << "Image pixels:\t\t" << srcImage.cols*srcImage.rows << endl
		 << "Image type:\t\t" << srcImage.type() << endl;
	// ʱ����Ϣ����¼�����㷨ʹ��ʱ��
	double time = static_cast<double>(getTickCount());

	// ��ʼ�㷨����
	// ׼������ Mat ����
	Mat dst(srcImage.size(), CV_8UC1);
	Mat redPixelsImage(srcImage.size(), CV_8UC1); 
	Mat dilatedImage(srcImage.size(), CV_8UC1);
	Mat skeletonImage(srcImage.size(), CV_8UC1);
	// ��ȡ��ɫ���ص�
	czh_extractColor(srcImage, redPixelsImage, RED);
	// ���ڳ�ȡ�����ĺ�ɫ����ͼ�������ͣ����ֲ��ϵ�
	dilate(redPixelsImage, dilatedImage, getStructuringElement(MORPH_RECT, Size(3, 3), Point(-1, -1)), Point(-1, -1),3);
	// ��������֮��Ķ�ֵͼ����������ȡ
	czh_skeleton(dilatedImage, skeletonImage);
	// ���ƹ�����ȡͼ��Ŀ�����ͼ�� Mat ����
	skeletonImage.copyTo(dst);
	// ���ͼ���ļ�
	czh_imwrite(dst, fileName);
	
	// ���������Ϣ�������������ʱ��
	time = ((double)getTickCount() - time) / getTickFrequency();
	cout << "Program finished:\t" << time << " second used.\n" << endl;
	waitKey();
	system("pause");
}