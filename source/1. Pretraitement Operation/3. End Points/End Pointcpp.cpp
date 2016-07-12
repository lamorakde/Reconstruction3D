#include <iostream>
#include <fstream>
#include "opencv2/opencv.hpp"

using namespace std;
using namespace cv;

void CZH_endPoint(Mat & src_skeleton_Image, Mat & dst_endPoint_Image);
void CZH_writeWhitePixel(Mat & srcImage, vector<Point2f> & dstWhitePoints);

int main()
{
	// ��ȡ�����ļ���
	cout << "This program is going to extract the end points of a skeleton image\n\n"
		<< "Please enter the input image name without .pgm:  ";
	string fileName, srcFileName, outputImageName;
	getline(cin, fileName);
	outputImageName = fileName + "_endPoints.pgm";
	srcFileName = fileName + ".pgm";
	
	// ��ȡԴͼ�񣬲����ж˵�Ѱ��
	Mat srcImage = imread(srcFileName, 0);
	Mat dstImage(srcImage.size(), srcImage.type(), Scalar(0));
	CZH_endPoint(srcImage, dstImage);

	// ���ͼ��
	imwrite(outputImageName, dstImage);

	// �����ɫ�˵���������
	vector<Point2f> whitePoints;				// ���ڴ�Ű�ɫ���ص�ʸ��
	CZH_writeWhitePixel(dstImage, whitePoints);	// ���ú��������ذ�ɫ�������겢����ʸ����
	string outputTextName = fileName + ".txt";	// ׼�������������������ı��ļ�
	ofstream outFile;
	outFile.open(outputTextName);
	for (int i = 0; i < whitePoints.size(); i++)// ������ݵ� txt �ļ�
	{
		outFile << whitePoints[i].x << ",\t" << whitePoints[i].y << endl;
	}
	outFile.close();

	waitKey();
	system("pause");
}

void CZH_endPoint(Mat & src_skeleton_Image, Mat & dst_endPoint_Image)
{
	// �ó�������һ��Ŀ��Ԫ��Ϊ��ɫ���صĵ� Mat �����ͼ��
	// ���򽫻�Ѱ�Ҹù���ͼ��Ķ˵�
	// ��������ͼ���ɫ����ռ����������޸ĵڶ�������ͼ��
	Mat src_image = src_skeleton_Image;
	Mat dst_image(src_image.size(), src_image.type(), Scalar(0));

	// �����ɫ��ɫ���ظ���
	Mat_<uchar>::iterator itBegin = src_image.begin<uchar>();
	Mat_<uchar>::iterator itEnd = src_image.end<uchar>();
	int blackPoints = 0;
	int whitePoints = 0;
	for (; itBegin != itEnd; itBegin++)
	{
		if (*itBegin == 0)
			blackPoints++;
		else whitePoints++;
	}

	if (whitePoints < blackPoints)
	{
		for (int i = 1; i < src_image.rows - 1; i++)
		{
			for (int j = 1; j < src_image.cols - 1; j++)
			{
				if (src_image.at<uchar>(i, j) == 255)
				{
					int sumWhite = 0;
					for (int ii = -1; ii <= 1; ii++)
					{
						for (int jj = -1; jj <= 1; jj++)
						{
							if (src_image.at<uchar>(i + ii, j + jj) == 255)
							{
								sumWhite++;
							}
						}
					}
					if (sumWhite == 2)
					{
						dst_image.at<uchar>(i, j) = 255;
					}
				}
			}
		}
		dst_endPoint_Image = dst_image;
	}
	else cerr << "�ú��������ɫ�����ṹ�Ķ˵㣬����ͼ���ǰ�ɫ���ع���ͼ��\n";
}

void CZH_writeWhitePixel(Mat & srcImage, vector<Point2f> & dstWhitePoints)
{
	// �ú�������һ����ֵͼ�������ҳ���ͼ���еİ�ɫ����
	// ���ҽ���Щ������������������ʸ�� vector<Point2f> & dstWhitePoints ֮��
	Mat src = srcImage;

	// ����������ʸ����ԭ�е����ݲ��ͷſռ�
	vector<Point2f>().swap(dstWhitePoints);

	Point2f curPoint;	//	���ڱ���ͼ��ĵ�ǰ��
	
	for (int i = 0; i < src.rows; i++)
	{
		for (int j = 0; j < src.cols ; j++)
		{
			// ����õ��ǰ�ɫ��������� x,y ������ǰ�����ݣ������õ��ƽ�ʸ����
			if (src.at<uchar>(i,j) == 255)
			{
				curPoint.x = j;
				curPoint.y = i;
				dstWhitePoints.push_back(curPoint);
			}
		}
	}

	// ������а�ɫ������Ϣ
	cout << "White pixels coordinates:\n";
	for (int i = 0; i < dstWhitePoints.size(); i++)
	{
		cout << "\t#" << i+1 << ":\t" << dstWhitePoints[i].x << ",\t" << dstWhitePoints[i].y << endl;
	}
}