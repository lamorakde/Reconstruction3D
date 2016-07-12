#include "opencv2/opencv.hpp"
using namespace cv;
using namespace std;

static enum { RED, GREEN, BLUE, WHITE, BLACK } color;

void czh_inverseBinary(Mat & src_bw_Image, Mat & dst_bw_Image)
{
	// ʹ�� LUT ��תͼ���кڰ�����
	Mat tmpImage;
	tmpImage.create(src_bw_Image.size(), src_bw_Image.type());
	tmpImage = src_bw_Image;
	uchar inverseBinaryTable[256];	// ��ʼ����ת��
	inverseBinaryTable[0] = 255;	// ��ʼ����ת��
	inverseBinaryTable[255] = 0;	// ��ʼ����ת��

	Mat lookUpTable(1, 256, CV_8U);	// ���� LUT ���ҵı�
	uchar* p = lookUpTable.data;
	for (int i = 0; i < 256; ++i)
	{
		p[i] = inverseBinaryTable[i];
	}
	LUT(tmpImage, lookUpTable, dst_bw_Image);
}

void czh_endPoint(Mat & src_skeleton_Image, Mat & dst_endPoint_Image)
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

void czh_writeWhitePixel(Mat & srcImage, vector<Point2f> & dstWhitePoints)
{
	// �ú�������һ����ֵͼ�������ҳ���ͼ���еİ�ɫ����
	// ���ҽ���Щ������������������ʸ�� vector<Point2f> & dstWhitePoints ֮��
	Mat src = srcImage;

	// ����������ʸ����ԭ�е����ݲ��ͷſռ�
	vector<Point2f>().swap(dstWhitePoints);

	Point2f curPoint;	//	���ڱ���ͼ��ĵ�ǰ��

	for (int i = 0; i < src.rows; i++)
	{
		for (int j = 0; j < src.cols; j++)
		{
			// ����õ��ǰ�ɫ��������� x,y ������ǰ�����ݣ������õ��ƽ�ʸ����
			if (src.at<uchar>(i, j) == 255)
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
		cout << "\t#" << i + 1 << ":\t" << dstWhitePoints[i].x << ",\t" << dstWhitePoints[i].y << endl;
	}
}

void czh_skeleton(Mat & srcImage, Mat & dstImage, int iterations = 10)
{
	// �ú�������һ����ֵͼ����߻Ҷ�ͼ�������ͼ��Ĺ���
	if (srcImage.type() != CV_8UC1)
	{
		cerr << "ֻ�ܴ����ֵ��Ҷ�ͼ��.\n";
		cerr << "��ȡͼ����: imread() ���ϲ��� \"0\" ��������޸�������.\n";
		return;
	}

	Mat tmpImage;
	int p2, p3, p4, p5, p6, p7, p8, p9;
	uchar * ptrTop, *ptrCur, *ptrBot; // 8��������һ�У���ǰ�к���һ�е���ָ��
	// ��Ϊ�� 8 ������������Կ�͸߸���1����ָֹ�����
	const int height = srcImage.rows - 1;	
	const int width = srcImage.cols - 1;
	bool isFinished = false;

	srcImage.copyTo(dstImage);

	for (int it = 0; it < iterations; it++)
	{
		// ÿ�ε����������� tmpImage �ϵĵ㣬�޸� dstImage �ϵ�����
		// ��һ�εĵ�����ʼʱ���� dstImage �ϵ����ݸ��Ƶ� tmpImage��
		dstImage.copyTo(tmpImage);
		isFinished = false;

		// ����ɨ�����һ:��ʼ
		for (int i = 1; i < height; i++)
		{
			// ��3*3������ÿһ����ָ�븳ֵ
			ptrTop = tmpImage.ptr<uchar>(i - 1);
			ptrCur = tmpImage.ptr<uchar>(i);
			ptrBot = tmpImage.ptr<uchar>(i + 1);

			for (int j = 1; j < width; j++)	
			{	
				if (ptrCur[j] > 0)	// �����ǰ����ΪĿ�����صĻ�
				{
					int AP = 0;
					p2 = (ptrTop[j] > 0);
					p3 = (ptrTop[j + 1] > 0);
					p4 = (ptrCur[j + 1] > 0);
					p5 = (ptrBot[j + 1] > 0);
					p6 = (ptrBot[j] > 0);
					p7 = (ptrBot[j - 1] > 0);
					p8 = (ptrCur[j - 1] > 0);
					p9 = (ptrTop[j - 1] > 0);
					if (p2 == 0 && p3 == 1) AP++;
					if (p3 == 0 && p4 == 1) AP++;
					if (p4 == 0 && p5 == 1) AP++;
					if (p5 == 0 && p6 == 1) AP++;
					if (p6 == 0 && p7 == 1) AP++;
					if (p7 == 0 && p8 == 1) AP++;
					if (p8 == 0 && p9 == 1) AP++;
					if (p9 == 0 && p2 == 1) AP++;

					// ����ĸ�����������Ļ�����ɾ�� dstImage �е�ǰ��
					if (((p2 + p3 + p4 + p5 + p6 + p7 + p8 + p9) >= 2) && ((p2 + p3 + p4 + p5 + p6 + p7 + p8 + p9) <= 6))
					{
						if (AP == 1)
						{
							if (p2*p4*p6 == 0)
							{
								if (p4*p6*p8 == 0)
								{
									dstImage.ptr<uchar>(i)[j] = 0;
									isFinished = true;
								}
							}
						}
					}
				}
			}
		} // ����ɨ�����һ:����

		  // ����ɨ����̶�:��ʼ
		dstImage.copyTo(tmpImage);
		for (int i = 1; i < height; i++)
		{
			ptrTop = tmpImage.ptr<uchar>(i - 1);
			ptrCur = tmpImage.ptr<uchar>(i);
			ptrBot = tmpImage.ptr<uchar>(i + 1);

			for (int j = 1; j < width; j++)
			{
				if (ptrCur[j] > 0)
				{
					int AP = 0;
					p2 = (ptrTop[j] > 0);
					p3 = (ptrTop[j + 1] > 0);
					p4 = (ptrCur[j + 1] > 0);
					p5 = (ptrBot[j + 1] > 0);
					p6 = (ptrBot[j] > 0);
					p7 = (ptrBot[j - 1] > 0);
					p8 = (ptrCur[j - 1] > 0);
					p9 = (ptrTop[j - 1] > 0);
					if (p2 == 0 && p3 == 1) AP++;
					if (p3 == 0 && p4 == 1) AP++;
					if (p4 == 0 && p5 == 1) AP++;
					if (p5 == 0 && p6 == 1) AP++;
					if (p6 == 0 && p7 == 1) AP++;
					if (p7 == 0 && p8 == 1) AP++;
					if (p8 == 0 && p9 == 1) AP++;
					if (p9 == 0 && p2 == 1) AP++;

					if (((p2 + p3 + p4 + p5 + p6 + p7 + p8 + p9) >= 2) && ((p2 + p3 + p4 + p5 + p6 + p7 + p8 + p9) <= 6))
					{
						if (AP == 1)
						{
							if (p2*p4*p8 == 0)
							{
								if (p2*p6*p8 == 0)
								{
									dstImage.ptr<uchar>(i)[j] = 0;
									isFinished = true;
								}
							}
						}
					}

				}
			}
		} // ����ɨ����̶�:����

		//�����������ɨ������Ѿ�û�п���ϸ���������ˣ����˳�����
		if (!isFinished) break;
	}
}

void czh_extractColor(Mat &srcImage, Mat & dstImage, int color = RED)
{
	// ���������Mat & srcImage ��һ����ͨ�� Mat����
	// ���������Mat & dstImage ��һ���� srcImage ��ͬ�ߴ�ĵ�ͨ��Mat���󣬲������ dstImage ������
	// ���������int color ����Ҫ��ȡ����ɫ��Ŀǰֻ֧�ֺ�ɫ��RED��
	// �ú�������һ����ͨ��RGBͼ�񣬳�ȡ��ͼ���е� color ��ɫ�����أ�����Ϊ��ɫ���أ�����������Ϊ��ɫ����������� dstImage ������

	if (!srcImage.data)
	{
		cerr << "��ͼ��ʧ��." << endl;
		return;
	}
	if (srcImage.type() != CV_8UC3)
	{
		cerr << "�ú���ֻ֧����ͨ��ͼ��." << endl;
		return;
	}
	if (dstImage.type() != CV_8UC1)
	{
		cerr << "�ú������ͼ��������Ϊһ����ͨ������." << endl;
		return;
	}

	Mat temp; srcImage.copyTo(temp); // ��Դͼ���Ƶ� temp ��ʱ����֮��
	Mat dst(srcImage.size(), CV_8UC1);	// Ŀ��ͼ���ʼ��ΪԴͼ��ߴ磬��ͨ��
	MatIterator_<Vec3b> itBegin, itEnd;	// Դͼ�����������
	MatIterator_<uchar> itDst;	// Ŀ��ͼ�����������
	int meanRed, meanGreen, distance, distanceThreshold;;		// ��ֵ�㷨��Ҫ�Ĳ���: ��ɫ��ֵ����ɫ��ֵ�����ؾ�ֵ֮���ֵ
	
	switch (color)	// ���ݲ�ͬ��ɫ��ѡ��ͬ�Ĳ���
	{
		case RED:
			meanRed = 200;
			meanGreen = 23;
			distanceThreshold = 70;
			break;
		case BLUE:
			break;
		case GREEN:
			break;
	}

	for (itBegin = temp.begin<Vec3b>(), itEnd = temp.end<Vec3b>(), itDst = dst.begin<uchar>(); itBegin != itEnd; itBegin++, itDst++)
	{
		// (*itBegin)[1]  Green pixels
		// (*itBegin)[2]  Red pixels
		distance = fabs((*itBegin)[1] - meanGreen) + fabs((*itBegin)[2] - meanRed);
		if (distance < distanceThreshold)
		{
			(*itDst) = 255;
		}
		else
		{
			(*itDst) = 0;
		}
	}
	dst.copyTo(dstImage);	// ����ʱĿ����󸳸�����Ŀ�����
}

void czh_imwrite(Mat &dstImage,const string imageName)
{
	// �ú����Զ��ж����ͼ��Mat��������ͣ��Ծ������ͼ���ǻҶ�ͼ���������ͨ��ͼ��
	string dstFileName;
	switch (dstImage.type())
	{
	case CV_8UC1:
		dstFileName = "result " + imageName + ".pgm";
		break;
	default:
		dstFileName = "result " + imageName + ".ppm";
		break;
	}
	cout << "Output image name:\t" << dstFileName << endl;
	imwrite(dstFileName, dstImage);
}