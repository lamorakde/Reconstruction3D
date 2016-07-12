#include "opencv2/opencv.hpp"
#include <windows.h>
#include "czh_cv.h"

using namespace cv;
using namespace std;

static enum { RED, GREEN, BLUE, WHITE, BLACK } color;
#define WORD_IN_RED SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY |	FOREGROUND_RED)
#define WORD_IN_WHITE SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);

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
	cv::LUT(tmpImage, lookUpTable, dst_bw_Image);
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

void czh_thin_parallel(Mat & srcImage, Mat & dstImage, int iterations)
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

void czh_thin_LUT(Mat & srcImage, Mat & dstImage, int iterations)
{
	// �ú�������һ����ֵͼ����߻Ҷ�ͼ�������ͼ��Ĺ���
	if (srcImage.type() != CV_8UC1)
	{
		cerr << "ֻ�ܴ����ֵ��Ҷ�ͼ��.\n";
		cerr << "��ȡͼ����: imread() ���ϲ��� \"0\" ��������޸�������.\n";
		return;
	}

	// ���ڲ���ı�
	int array[256] = {	0,0,1,1,0,0,1,1,1,1,0,1,1,1,0,1,
						1,1,0,0,1,1,1,1,0,0,0,0,0,0,0,1,
						0,0,1,1,0,0,1,1,1,1,0,1,1,1,0,1,
						1,1,0,0,1,1,1,1,0,0,0,0,0,0,0,1,
						1,1,0,0,1,1,0,0,0,0,0,0,0,0,0,0,
						0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
						1,1,0,0,1,1,0,0,1,1,0,1,1,1,0,1,
						0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
						0,0,1,1,0,0,1,1,1,1,0,1,1,1,0,1,
						1,1,0,0,1,1,1,1,0,0,0,0,0,0,0,1,
						0,0,1,1,0,0,1,1,1,1,0,1,1,1,0,1,
						1,1,0,0,1,1,1,1,0,0,0,0,0,0,0,0,
						1,1,0,0,1,1,0,0,0,0,0,0,0,0,0,0,
						1,1,0,0,1,1,1,1,0,0,0,0,0,0,0,0,
						1,1,0,0,1,1,0,0,1,1,0,1,1,1,0,0,
						1,1,0,0,1,1,1,0,1,1,0,0,1,0,0,0 };

	// ׼����ʱ Mat ����
	Mat tempImage;
	srcImage.copyTo(dstImage);
	srcImage.copyTo(tempImage);

	// Ϊ�˱�֤Ч�����������Ͳ�����ʹ�ϵ���������
	dilate(srcImage, dstImage, getStructuringElement(MORPH_RECT, Size(3, 3), Point(-1, -1)), Point(-1, -1), 3);
	
	// ���㷨�����ں�ɫ���أ������ȷ�תͼ����г�ȡ�Ǽܲ����������ת����ȥ
	MatIterator_<uchar> itBegin, itEnd;
	for (itBegin = dstImage.begin<uchar>(), itEnd = dstImage.end<uchar>(); itBegin != itEnd; itBegin++)
	{
		(*itBegin) = abs((*itBegin) - 255);
	}

	// ��ʼ����
	int pointValue;

	uchar * ptrTop, *ptrCur, *ptrBot; // 8��������һ�У���ǰ�к���һ�е���ָ��
	uchar * dstPtr;	// Ŀ��ͼ��ǰ��ָ��

	// ��Ϊ�� 8 ������������Կ�͸߸���1����ָֹ�����
	const int height = srcImage.rows - 1;
	const int width = srcImage.cols - 1;

	for (int loop = 0; loop < iterations; loop++)
	{
		bool changed = false;	// �жϸôε����Ƿ��޸���������ɫ

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//																														   //
		//													����ɨ�����һ:��ʼ                                                    //
		//																														   //
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		bool next = true;			// ������Ծ���ӷ�
		dstImage.copyTo(tempImage);	// ÿ�ε����������޸Ĺ����Ŀ��ͼ���Ƹ���ʱͼ����м�鴦��

		for (int i = 1; i < height; i++)
		{
			// ��3*3������ÿһ����ָ�븳ֵ
			ptrTop = tempImage.ptr<uchar>(i - 1);
			ptrCur = tempImage.ptr<uchar>(i);
			ptrBot = tempImage.ptr<uchar>(i + 1);
			dstPtr = dstImage.ptr<uchar>(i);

			for (int j = 1; j < width; j++)
			{
				if (((int)ptrCur[j - 1] + (int)ptrCur[j] + (int)ptrCur[j + 1]) == 0)	// ����õ������ھӶ��Ǻ�ɫ���������õ㲻������
				{
					dstPtr[j] = ptrCur[j];
					continue;
				};

				if (next == false)	// ����޸���ĳ����( next �ᱻ��Ϊ false)����������һ���㲻������
				{
					next = true;
					dstPtr[j] = ptrCur[j];
					continue;
				};

				if ((int)ptrCur[j] == 0)	// ���ĳ����Ϊ��ɫ�������ж�
				{
					pointValue = 0;

					// ��һ��
					if ((int)(ptrTop[j - 1]) == 255)
					{
						pointValue += 1;
					}
					if ((int)(ptrTop[j]) == 255)
					{
						pointValue += 2;
					}
					if ((int)(ptrTop[j + 1]) == 255)
					{
						pointValue += 4;
					}

					// �ڶ���
					if ((int)(ptrCur[j - 1]) == 255)
					{
						pointValue += 8;
					}
					if ((int)(ptrCur[j]) == 255)
					{
						pointValue += 0;
					}
					if ((int)(ptrCur[j + 1]) == 255)
					{
						pointValue += 16;
					}

					// ������
					if ((int)(ptrBot[j - 1]) == 255)
					{
						pointValue += 32;
					}
					if ((int)(ptrBot[j]) == 255)
					{
						pointValue += 64;
					}
					if ((int)(ptrBot[j + 1]) == 255)
					{
						pointValue += 128;
					}

					dstPtr[j] = 255 * array[pointValue];

					if (dstPtr[j] == 255)	
					{
						next = false;	// ����޸��˵�ǰ���أ��Ӻڱ�Ϊ�ף�����������һ����
						changed = true;	// ���Ұ� changed ����Ϊtrue
					}
					else dstPtr[j] = ptrCur[j];
				}
			}
		}

		// �����������У��������б�Ϊ��ɫ
		ptrTop = dstImage.ptr<uchar>(0);
		ptrBot = dstImage.ptr<uchar>(dstImage.rows - 1);

		for (int i = 0; i < dstImage.cols; i++)
		{
			ptrTop[i] = 255;
			ptrBot[i] = 255;
		}
		for (int i = 0; i < dstImage.rows; i++)
		{
			dstImage.at<uchar>(i, 0) = 255;
			dstImage.at<uchar>(i, dstImage.cols - 1) = 255;
		}

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//																														   //
		//													����ɨ����̶�:��ʼ                                                    //
		//																														   //
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		next = true;			// ������Ծ���ӷ�
		dstImage.copyTo(tempImage);	// ÿ�ε����������޸Ĺ����Ŀ��ͼ���Ƹ���ʱͼ����м�鴦��

		for (int j = 1; j < width; j++)
		{
			for (int i = 1; i < height; i++)
			{
				// ��3*3������ÿһ����ָ�븳ֵ
				ptrTop = tempImage.ptr<uchar>(i - 1);
				ptrCur = tempImage.ptr<uchar>(i);
				ptrBot = tempImage.ptr<uchar>(i + 1);
				dstPtr = dstImage.ptr<uchar>(i);

				if (((int)ptrTop[j] + (int)ptrCur[j] + (int)ptrBot[j]) == 0)	// ����õ������ھӶ��Ǻ�ɫ���������õ㲻������
				{
					dstPtr[j] = ptrCur[j];
					continue;
				};

				if (next == false)	// ����޸���ĳ����( next �ᱻ��Ϊ false)����������һ���㲻������
				{
					next = true;
					dstPtr[j] = ptrCur[j];
					continue;
				};

				if ((int)ptrCur[j] == 0)	// ���ĳ����Ϊ��ɫ�������ж�
				{
					pointValue = 0;

					// ��һ��
					if ((int)(ptrTop[j - 1]) == 255)
					{
						pointValue += 1;
					}
					if ((int)(ptrTop[j]) == 255)
					{
						pointValue += 2;
					}
					if ((int)(ptrTop[j + 1]) == 255)
					{
						pointValue += 4;
					}

					// �ڶ���
					if ((int)(ptrCur[j - 1]) == 255)
					{
						pointValue += 8;
					}
					if ((int)(ptrCur[j]) == 255)
					{
						pointValue += 0;
					}
					if ((int)(ptrCur[j + 1]) == 255)
					{
						pointValue += 16;
					}

					// ������
					if ((int)(ptrBot[j - 1]) == 255)
					{
						pointValue += 32;
					}
					if ((int)(ptrBot[j]) == 255)
					{
						pointValue += 64;
					}
					if ((int)(ptrBot[j + 1]) == 255)
					{
						pointValue += 128;
					}

					dstPtr[j] = 255 * array[pointValue];

					if (dstPtr[j] == 255)	// ����޸��˵�ǰ���أ��Ӻڱ�Ϊ�ף�����������һ����
					{
						next = false;	// ����޸��˵�ǰ���أ��Ӻڱ�Ϊ�ף�����������һ����
						changed = true;	// ���Ұ� changed ����Ϊtrue
					}
					else dstPtr[j] = ptrCur[j];
				}
			}
		}

		// �����������У��������б�Ϊ��ɫ
		ptrTop = dstImage.ptr<uchar>(0);
		ptrBot = dstImage.ptr<uchar>(dstImage.rows - 1);
		for (int i = 0; i < dstImage.cols; i++)
		{
			ptrTop[i] = 255;
			ptrBot[i] = 255;
		}
		for (int i = 0; i < dstImage.rows; i++)
		{
			dstImage.at<uchar>(i, 0) = 255;
			dstImage.at<uchar>(i, dstImage.cols - 1) = 255;
		}

		if (changed == false)
		{
			break;	// ���һ�ε���û��ɾ���κ����أ�����Ϊ�Ѿ�����ϸ������ֱ���˳�
		}
	}

	// ����ٷ�ת�ڰ�
	for (itBegin = dstImage.begin<uchar>(), itEnd = dstImage.end<uchar>(); itBegin != itEnd; itBegin++)
	{
		(*itBegin) = abs((*itBegin) - 255);
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

void czh_minFilter(Mat &srcImage, Mat & dstImage, int radius)
{
	Mat src;
	srcImage.copyTo(src);
	Mat dst(srcImage.size(), srcImage.type(), Scalar::all(0));
	switch (srcImage.type())
	{
	case CV_8UC1:
	{
		int minValue;
		for (int i = radius; i < src.rows - radius; i++)	// ����ÿ������
		{
			for (int j = radius; j < src.cols - radius; j++)
			{
				minValue = src.at<uchar>(i, j);
				// ���ÿ�����أ���һ������֮�б���Ѱ����Сֵ
				for (int windows_rows_number = - radius; windows_rows_number <= radius; windows_rows_number++)
				{
					for (int windows_cols_number = - radius; windows_cols_number <= radius; windows_cols_number++)
					{
						if (src.at<uchar>(i + windows_rows_number, j + windows_cols_number) < minValue)
						{
							minValue = src.at<uchar>(i + windows_rows_number, j + windows_cols_number);
						}
					}
				}
				dst.at<uchar>(i, j) = minValue;
			}
		}
		
		for (int i = radius; i < src.rows - radius; i++)
		{
			for (int j = radius - 1; j >= 0; j--)
			{
				dst.at<uchar>(i, j) = dst.at<uchar>(i, j + 1);
			}

			for (int j = src.cols - radius; j < src.cols; j++)
			{
				dst.at<uchar>(i, j) = dst.at<uchar>(i, j - 1);
			}
		}

		for (int j = 0; j < src.cols; j++)
		{
			for (int i = radius - 1; i >= 0; i--)
			{
				dst.at<uchar>(i, j) = dst.at<uchar>(i + 1, j);
			}

			for (int i = src.rows - radius; i < src.rows; i++)
			{
				dst.at<uchar>(i, j) = dst.at<uchar>(i - 1, j);
			}
		}

		break;
	}
		
	case CV_32FC1:
	{
		float minValue;
		for (int i = radius; i < src.rows - radius; i++)	// ����ÿ������
		{
			for (int j = radius; j < src.cols - radius; j++)
			{
				minValue = src.at<float>(i, j);
				// ���ÿ�����أ���һ������֮�б���Ѱ����Сֵ
				for (int windows_rows_number = -radius; windows_rows_number <= radius; windows_rows_number++)
				{
					for (int windows_cols_number = -radius; windows_cols_number <= radius; windows_cols_number++)
					{
						if (src.at<float>(i + windows_rows_number, j + windows_cols_number) < minValue)
						{
							minValue = src.at<float>(i + windows_rows_number, j + windows_cols_number);
						}
					}
				}
				dst.at<float>(i, j) = minValue;
			}
		}

		for (int i = radius; i < src.rows - radius; i++)
		{
			for (int j = radius - 1; j >= 0; j--)
			{
				dst.at<float>(i, j) = dst.at<float>(i, j + 1);
			}

			for (int j = src.cols - radius; j < src.cols; j++)
			{
				dst.at<float>(i, j) = dst.at<float>(i, j - 1);
			}
		}

		for (int j = 0; j < src.cols; j++)
		{
			for (int i = radius - 1; i >= 0; i--)
			{
				dst.at<float>(i, j) = dst.at<float>(i + 1, j);
			}

			for (int i = src.rows - radius; i < src.rows; i++)
			{
				dst.at<float>(i, j) = dst.at<float>(i - 1, j);
			}
		}
		break;
	}
	} // switch finishes
	
	dstImage = dst;
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

void czh_helpInformation(string const &functionInfo)
{
	// ������Ϣ��������
	WORD_IN_WHITE;
	cout << "************************************************************************************************************************\n";
	cout << "\t\t\t\t\tProgram information:\n";
	cout << "\t\t\t\t" << functionInfo << "\n\n";
	cout << "************************************************************************************************************************\n\n";
}

void czh_imageOpenDetect(Mat & srcImage, string & fileName, string & fileType)
{	// ���ͼ������Ƿ��Ѿ��ɹ���
	string srcFileName = fileName + fileType;
	if (srcImage.data == nullptr)
	{
		WORD_IN_RED;
		cerr << "����: ��ͼ���ļ�: " << srcFileName << " ʧ�ܣ���ȷ���ļ������ļ�������ȷ.\n\n";
		WORD_IN_WHITE;
		cout << "Enter the input image name without " << fileType << ": ";
		fileName.clear(); srcFileName.clear();
		getline(cin, fileName);				// ��������ļ���
		srcFileName = fileName + fileType;	// ȷ������ͼƬ�ļ�����
		srcImage = imread(srcFileName);	// ��ȡԴͼ��
		czh_imageOpenDetect(srcImage, fileName, srcFileName);
	}
	return;
}

void czh_middlePointCircle(int x0, int y0, int radius, vector<Point> &pointsOfCircle)
{
	// �ú��������Ե� (x0, y0) ΪԲ�ģ� radiusΪ�뾶��Բ
	// ����Բ�ϵĵ�����굼�� vector<Point>ʸ�� pointsOfCircle ֮��
	// �е㻭Բ��������һ���Ż�����
	int x = radius;
	int y = 0;
	int error = 0;

	vector<Point>().swap(pointsOfCircle);	// ���Ŀ��ʸ��
	vector<vector<Point>> pointsDirection(8);	// ��ʱ����ʸ��

	while (x >= y)
	{
		pointsDirection[0].push_back(Point(x0 + x, y0 + y));
		pointsDirection[1].push_back(Point(x0 + y, y0 + x));
		pointsDirection[6].push_back(Point(x0 - y, y0 + x));
		pointsDirection[7].push_back(Point(x0 - x, y0 + y));
		pointsDirection[4].push_back(Point(x0 - x, y0 - y));
		pointsDirection[5].push_back(Point(x0 - y, y0 - x));
		pointsDirection[2].push_back(Point(x0 + y, y0 - x));
		pointsDirection[3].push_back(Point(x0 + x, y0 - y));
		
		error += 1 + 2 * y;
		y += 1;
		if (2 * (error - x) + 1 > 0)
		{
			error += 1 - 2 * x;
			x -= 1;
		}
	}

	// ��Ϊ�Գ��ԣ���4����ĵ���ת˳�򲢲���ȷ����Ҫ����˳��
	for (int i = 0; i < pointsDirection[1].size() / 2; i++)
	{
		swap(pointsDirection[1][i], pointsDirection[1][pointsDirection[1].size() - 1 - i]);
		swap(pointsDirection[3][i], pointsDirection[3][pointsDirection[3].size() - 1 - i]);
		swap(pointsDirection[5][i], pointsDirection[5][pointsDirection[5].size() - 1 - i]);
		swap(pointsDirection[7][i], pointsDirection[7][pointsDirection[7].size() - 1 - i]);
	}

	for (int i = 0; i < 8; i++)
	{
		for (int j = 0; j < pointsDirection[0].size(); j++)
		{
			pointsOfCircle.push_back(pointsDirection[i][j]);
		}
	}
}

void czh_BresenhamCircle(int x0, int y0, int radius, vector<Point> &pointsOfCircle)
{
	// �ú��������Ե� (x0, y0) ΪԲ�ģ� radiusΪ�뾶��Բ
	// ����Բ�ϵĵ��������12���ӷ���ʼ˳ʱ�뷽�����ε��� vector<Point>ʸ�� pointsOfCircle ֮��
	// ref : algorithm of Bresenham
	vector<Point>().swap(pointsOfCircle);	// ���Ŀ��ʸ��
	vector<vector<Point>> pointsDirection(8);	// ��ʱ����ʸ��
	int x, y, distance;
	x = 0;
	y = radius;
	distance = 3 - 2 * radius;

	while (x < y)
	{
		pointsDirection[0].push_back(Point(x0 + x, y0 + y));
		pointsDirection[1].push_back(Point(x0 + y, y0 + x));// ˳����ȷ
		pointsDirection[6].push_back(Point(x0 - y, y0 + x));
		pointsDirection[7].push_back(Point(x0 - x, y0 + y));// ˳����ȷ
		pointsDirection[4].push_back(Point(x0 - x, y0 - y));
		pointsDirection[5].push_back(Point(x0 - y, y0 - x));// ˳����ȷ
		pointsDirection[2].push_back(Point(x0 + y, y0 - x));
		pointsDirection[3].push_back(Point(x0 + x, y0 - y));// ˳����ȷ

		// �ж���һ�εĵ�����
		if (distance < 0)
		{
			distance += 4 * x + 6;
		}
		else
		{
			distance += 4 * (x - y) + 10;
			y--;
		}
		x++;
	}


// 	pointsDirection[0].push_back(Point(x0 + x, y0 + y));
// 	pointsDirection[1].push_back(Point(x0 + y, y0 + x));
// 	pointsDirection[6].push_back(Point(x0 - y, y0 + x));
// 	pointsDirection[7].push_back(Point(x0 - x, y0 + y));
// 	pointsDirection[4].push_back(Point(x0 - x, y0 - y));
// 	pointsDirection[5].push_back(Point(x0 - y, y0 - x));
// 	pointsDirection[2].push_back(Point(x0 + y, y0 - x));
// 	pointsDirection[3].push_back(Point(x0 + x, y0 - y));
// 
// 	x++;
// 
// 	while (x < y)
// 	{
// 		if (distance < 0)
// 		{
// 			distance += 4 * x + 6;
// 		}
// 		else
// 		{
// 			distance += 4 * (x - y) + 10;
// 			y--;
// 		}
// 		pointsDirection[0].push_back(Point(x0 + x, y0 + y));	
// 		pointsDirection[1].push_back(Point(x0 + y, y0 + x));// ˳����ȷ
// 		pointsDirection[6].push_back(Point(x0 - y, y0 + x));	
// 		pointsDirection[7].push_back(Point(x0 - x, y0 + y));// ˳����ȷ
// 		pointsDirection[4].push_back(Point(x0 - x, y0 - y));	
// 		pointsDirection[5].push_back(Point(x0 - y, y0 - x));// ˳����ȷ
// 		pointsDirection[2].push_back(Point(x0 + y, y0 - x));	
// 		pointsDirection[3].push_back(Point(x0 + x, y0 - y));// ˳����ȷ
// 
// 		x++;
// 	}

	// ��Ϊ�Գ��ԣ���4����ĵ���ת˳�򲢲���ȷ����Ҫ����˳��
	for (int i = 0; i < pointsDirection[1].size() / 2; i++)
	{
		swap(pointsDirection[1][i], pointsDirection[1][pointsDirection[1].size() - 1 - i]);
		swap(pointsDirection[3][i], pointsDirection[3][pointsDirection[3].size() - 1 - i]);
		swap(pointsDirection[5][i], pointsDirection[5][pointsDirection[5].size() - 1 - i]);
		swap(pointsDirection[7][i], pointsDirection[7][pointsDirection[7].size() - 1 - i]);
	}

	for (int i = 0; i < 8; i++)
	{
		for (int j = 0; j < pointsDirection[i].size(); j++)
		{
			pointsOfCircle.push_back(pointsDirection[i][j]);
		}
	}
}

void czh_BresenhamLine(Point pt0, Point pt1, vector<Point> &linePoints)
{
	// �ú��������Ե� pt0 Ϊ��㣬 �� pt1 Ϊ�յ��ֱ�ߣ�����ֱ�ߵ���� linePoints ��
	linePoints.clear();	// ������ڴ�ŵ������
	
	int x0 = pt0.x;
	int y0 = pt0.y;
	int x1 = pt1.x;
	int y1 = pt1.y;

	bool directionRightToLeft = false;	// �Ƿ����ŵ�

	bool steep = (abs(y1 - y0) > abs(x1 - x0));
	if (steep)	// ���б�ʴ���1�����б�ʱ�Ϊ  1/ԭ����б�� �����㣬�ŵ��ʱ��ߵ� x �� y����
	{
		swap(x0, y0);
		swap(x1, y1);
	}
	if (x0 > x1)	// �����x������յ�x�������ӷ������ŵ�
	{
		directionRightToLeft = true;
		swap(x0, x1);
		swap(y0, y1);
	}
	int deltax = x1 - x0;
	int deltay = abs(y1 - y0);
	int error = deltax / 2;
	int ystep;
	int y = y0;

	if (y0 < y1)	// �� y ���������ϻ������£���ȡ��һ���Ĳ�������
		ystep = 1;
	else
		ystep = -1;

	for (int x = x0; x <= x1; x++)
	{
		if (steep)	// ����ԭ��б���Ƿ�С��1����ȡ��һ�����ŵ㷽ʽ��(x,y)����(y,x)
		{
			linePoints.push_back(Point(y, x));
		}
		else
		{
			linePoints.push_back(Point(x, y));
		}
		error = error - deltay;
		if (error < 0)
		{
			y = y + ystep;
			error = error + deltax;
		}
	}

	// Ĭ���Ƶ�˳���Ǵ����Ͻ������½����ηŵ㣬�����ʼ�������½ǣ���ߵ�������˳��
	if (directionRightToLeft == true)
	{
		for (int i = 0; i < linePoints.size() / 2; i++)
			swap(linePoints[i], linePoints[linePoints.size() - 1 - i]);
	}
}

void czh_Circle_Profile(const Mat & srcImage, const int x0, const int y0, const int radius)
{
	// չʾ��ѡ����ΪԲ�ģ�radiusΪ�뾶��Բ�ĻҶȱ仯

	// ���Ա�ѡ��ĵ�ΪԲ�ģ�radiusΪ�뾶��һ��Բ��Բ�ϵĵ����걻���� pointsOfCircle ʸ��֮��
	vector<Point> pointsOfCircle;
	czh_BresenhamCircle(x0, y0, radius, pointsOfCircle);

	// ��Բ�ϵĵ�ĻҶ�ֵ���� valuesOfPoints ʸ��֮��
	vector<int> valuesOfPoints;	// ���Ŀ��ʸ��
	for (int i = 0; i < pointsOfCircle.size(); i++)
	{
		valuesOfPoints.push_back((int)srcImage.at<uchar>(pointsOfCircle[i]));
	}
	valuesOfPoints.push_back((int)srcImage.at<uchar>(pointsOfCircle[0]));	// �������һ��Ԫ�أ������һ�����صĻҶ���ͬ

	int profileHeight = 255;	// profileͼ��߶�
	int profileWidth = pointsOfCircle.size() * 6;	// profile ͼ����
	Mat profileImage(profileHeight, profileWidth, CV_8UC1, Scalar::all(0));

	for (int x = 0; x < pointsOfCircle.size(); x++)
	{
		int y = profileHeight - valuesOfPoints[x] / 2;
		for (; y < profileHeight; y++)
		{
			profileImage.at<uchar>(y, 4 * x) = 255;
			profileImage.at<uchar>(y, 4 * x + 1) = 255;
			profileImage.at<uchar>(y, 4 * x + 2) = 255;
			profileImage.at<uchar>(y, 4 * x + 3) = 255;
		}
	}
	imshow("Profile", profileImage);
}

Mat czh_Line_Profile(const Mat & srcImage, Point pt0, Point pt1, bool sameSize)
{
	// չʾ��ѡ����������Ϊ�˵��ֱ����ͼ�еĻҶȱ仯,���ر仯ͼ����
	vector<Point> linePts;	// ���ڴ��ֱ���ϵĵ�ĵ�����
	czh_BresenhamLine(pt0, pt1, linePts);	// �ҳ�ֱ���ϵĵ㣬�Ž� linePts

	// �����ϵĵ�ĻҶ�ֵ���� valuesOfPoints ʸ��֮��
	vector<int> valuesOfPoints;	// ���Ŀ��ʸ��
	for (int i = 0; i < linePts.size(); i++)
	{
		valuesOfPoints.push_back((int)srcImage.at<uchar>(linePts[i]));
	}

	int profileHeight = 255;	// profileͼ��߶�
	int profileWidth;	// profile ͼ����

	if(!sameSize)
		profileWidth = linePts.size() * 6;	
	else profileWidth = linePts.size();

	Mat profileImage(profileHeight, profileWidth, CV_8UC1, Scalar::all(0));

	for (int x = 0; x < linePts.size(); x++)
	{
		int y = profileHeight - valuesOfPoints[x] / 2;
		for (; y < profileHeight; y++)
		{
			if (!sameSize)
			{
				profileImage.at<uchar>(y, 4 * x) = 255;
				profileImage.at<uchar>(y, 4 * x + 1) = 255;
				profileImage.at<uchar>(y, 4 * x + 2) = 255;
				profileImage.at<uchar>(y, 4 * x + 3) = 255;
			}
			else
			{
				profileImage.at<uchar>(y, x) = 255;
			}
		}
	}
	// imshow("Line Profile", profileImage);
	return profileImage;
}

void czh_labeling(Mat & src, Mat & dst)
{
	// �ú�����ʹ�� two - pass �㷨��Զ�ֵͼ������ͨ����

	// ׼��������ʱ����
	Mat srcImage;
	src.copyTo(srcImage);

	///////////////////////////////////////////////////////////
	//				      first pass                         //
	///////////////////////////////////////////////////////////

	// ��Ϊ�� 8 ������������Կ�͸߸���1����ָֹ�����
	const int height = srcImage.rows - 1;
	const int width = srcImage.cols - 1;

	uchar *data_cur, *data_prev;	// ��ǰ�к���һ����ָ��
	int label = 1;
	vector<int> labelSet;
	labelSet.push_back(0);
	labelSet.push_back(1);

	///////////////////////////////////////////////////////////////////////////////////////////////////
	////////									�����һ��						    			///////
	////////									   ��ʼ							    			///////
	data_cur = srcImage.ptr<uchar>(0);

	if (data_cur[0] != 0)	// �����һ�еĵ�һ��Ԫ�ز�Ϊ0�������һ��label
	{
		labelSet.push_back(++label);	// ���� labelSet �������һ�� label
		labelSet[label] = label;		// �����λ�õ� label ��ֵ
		data_cur[0] = label;			// �޸�Դͼ���ػҶ�ֵΪ label
	}

	for (int j = 1; j < srcImage.cols; j++)
	{
		if (data_cur[j] == 0)
		{	// �����ǰ���ز���Ŀ�����أ�������������
			continue;
		}
		int left = data_cur[j - 1];	// ���Ҷ�ֵ
		int cnt = 0;		// ���ڼ��������ϵ�һ���ж��ٸ���Чֵ
		int neighborLabels[2];
		if (left != 0)		// ����������Чֵ
		{
			neighborLabels[cnt++] = left;	// �����Ҷ�ֵ���� neighborLabels ��һ��Ԫ��
		}

		if (cnt == 0)	// ������Ϊ0
		{
			labelSet.push_back(++label);	// ���� labelSet �������һ�� label
			labelSet[label] = label;		// �����λ�õ� label ��ֵ
			data_cur[j] = label;			// �޸�Դͼ���ػҶ�ֵΪ label
			continue;						// ������һ��ѭ��
		}
		int smallestLabel = neighborLabels[0];				// ��������ϵ�ֻ��һ����Чֵ����ô��ѡ�����Чֵ�� label ��ΪС��label
		data_cur[j] = smallestLabel;	// ��Դͼ��õ�Ҷ�ֵ��ֵΪС�� label

		// ������С�ȼ۱�  
		for (int k = 0; k < cnt; k++)
		{
			int tempLabel = neighborLabels[k];
			int & oldSmallestLabel = labelSet[tempLabel];
			if (oldSmallestLabel > smallestLabel)
			{
				labelSet[oldSmallestLabel] = smallestLabel;
			}
			else if (oldSmallestLabel < smallestLabel)
			{
				labelSet[smallestLabel] = oldSmallestLabel;
			}
		}
	}
	////////									�����һ��						    			///////
	////////									   ����							    			///////
	///////////////////////////////////////////////////////////////////////////////////////////////////

	///////////////////////////////////////////////////////////////////////////////////////////////////
	////////									�����м�						    			///////
	////////									  ��ʼ							    			///////
	for (int i = 1; i < height; i++)
	{
		data_cur = srcImage.ptr<uchar>(i);		// ��ǰ����ָ��
		data_prev = srcImage.ptr<uchar>(i - 1);	// ��һ����ָ��

		// ������е�һ��Ԫ��
		if (data_cur[0] != 0)	// �����һ�еĵ�һ��Ԫ�ز�Ϊ0��������Ķ���Ԫ�أ��������Ԫ��δ����ǣ������һ��label
		{
			int up = data_prev[0];		// �ϵ�Ҷ�ֵ
			int neighborLabels[2];		// ���ڴ洢�����ϵ�ĻҶ�ֵ
			int cnt = 0;		// ���ڼ��������ϵ�һ���ж��ٸ���Чֵ
			if (up != 0)		// ����ϵ�����Чֵ
			{
				neighborLabels[cnt++] = up;		// ���ϵ�Ҷ�ֵ���� neighborLabels ����һ��Ԫ��
			};
			if (cnt == 0)	// ����ϵ�Ϊ0������ǰ��һ�� label
			{
				labelSet.push_back(++label);	// ���� labelSet �������һ�� label
				labelSet[label] = label;		// �����λ�õ� label ��ֵ
				data_cur[0] = label;			// �޸�Դͼ���ػҶ�ֵΪ label
			}
			else
			{
				int smallestLabel = neighborLabels[0];				// ��������ϵ�ֻ��һ����Чֵ����ô��ѡ�����Чֵ�� label ��ΪС��label
				data_cur[0] = smallestLabel;	// ��Դͼ��õ�Ҷ�ֵ��ֵΪС�� label

				// ������С�ȼ۱�  
				for (int k = 0; k < cnt; k++)
				{
					int tempLabel = neighborLabels[k];
					int & oldSmallestLabel = labelSet[tempLabel];
					if (oldSmallestLabel > smallestLabel)
					{
						labelSet[oldSmallestLabel] = smallestLabel;
					}
					else if (oldSmallestLabel < smallestLabel)
					{
						labelSet[smallestLabel] = oldSmallestLabel;
					}
				}
				// ÿ����һ��Ŀ��㣬���������ϵ��б�ǵĻ����͸�����ͨ���Ч��С label 
				// ����Ч��ͨ���е� label �滻Ϊ��ͨ������С label 
				// ����ÿ�α���һ���µ㣬��ͨ���еĸ��� label ���Ǹ���ͨ������С�� label
				for (size_t i = 2; i < labelSet.size(); i++)
				{
					int curLabel = labelSet[i];
					int preLabel = labelSet[curLabel];
					while (preLabel != curLabel)
					{
						curLabel = preLabel;
						preLabel = labelSet[preLabel];
					}
					labelSet[i] = curLabel;
				}
			}
		}

		// ����ÿһ�еĺ����Ԫ��
		for (int j = 1; j < srcImage.cols; j++)
		{	// ����ͼ��ÿһ������
			if (data_cur[j] == 0)
			{	// �����ǰ���ز���Ŀ�����أ�������������
				continue;
			}
			int left = data_cur[j - 1];	// ���Ҷ�ֵ
			int up = data_prev[j];		// �ϵ�Ҷ�ֵ
			int neighborLabels[2];		// ���ڴ洢�����ϵ�ĻҶ�ֵ
			int cnt = 0;		// ���ڼ��������ϵ�һ���ж��ٸ���Чֵ
			if (left != 0)		// ����������Чֵ
			{
				neighborLabels[cnt++] = left;	// �����Ҷ�ֵ���� neighborLabels ��һ��Ԫ��
			}
			if (up != 0)		// ����ϵ�����Чֵ
			{
				neighborLabels[cnt++] = up;		// ���ϵ�Ҷ�ֵ���� neighborLabels ����һ��Ԫ��
			}
			if (cnt == 0)	// ��������ϵ㶼Ϊ0
			{
				labelSet.push_back(++label);	// ���� labelSet �������һ�� label
				labelSet[label] = label;		// �����λ�õ� label ��ֵ
				data_cur[j] = label;			// �޸�Դͼ���ػҶ�ֵΪ label
				continue;						// ������һ��ѭ��
			}
			int smallestLabel = neighborLabels[0];				// ��������ϵ�ֻ��һ����Чֵ����ô��ѡ�����Чֵ�� label ��ΪС��label
			if (cnt == 2 && neighborLabels[1] < smallestLabel)	// ��������ϵ㶼����Чֵ��ѡ��С��һ�� label
			{
				smallestLabel = neighborLabels[1];
			}
			data_cur[j] = smallestLabel;	// ��Դͼ��õ�Ҷ�ֵ��ֵΪС�� label

			// ������С�ȼ۱�  
			for (int k = 0; k < cnt; k++)
			{
				int tempLabel = neighborLabels[k];
				int & oldSmallestLabel = labelSet[tempLabel];
				if (oldSmallestLabel > smallestLabel)
				{
					// ��ԭ��ͨ����С label �ȵ�ǰ��С label ��ʱ��������ǰ����С label ��������ͨ����С label
					labelSet[oldSmallestLabel] = smallestLabel;
				}
				else if (oldSmallestLabel < smallestLabel)
				{
					labelSet[smallestLabel] = oldSmallestLabel;
				}
			}
			// ÿ����һ��Ŀ��㣬���������ϵ��б�ǵĻ����͸�����ͨ���Ч��С label 
			// ����Ч��ͨ���е� label �滻Ϊ��ͨ������С label 
			// ����ÿ�α���һ���µ㣬��ͨ���еĸ��� label ���Ǹ���ͨ������С�� label
			for (size_t i = 2; i < labelSet.size(); i++)
			{
				int curLabel = labelSet[i];
				int preLabel = labelSet[curLabel];
				while (preLabel != curLabel)
				{
					curLabel = preLabel;
					preLabel = labelSet[preLabel];
				}
				labelSet[i] = curLabel;
			}
		}
	}

	////////									�����м�						    			///////
	////////									  ����							    			///////
	///////////////////////////////////////////////////////////////////////////////////////////////////


	///////////////////////////////////////////////////////////////////////////////////////////////////
	////////								�������һ��						    			///////
	////////									��ʼ							    			///////
	data_cur = srcImage.ptr<uchar>(srcImage.rows - 1);
	data_prev = srcImage.ptr<uchar>(srcImage.rows - 2);	// ��һ����ָ��

	if (data_cur[0] != 0)	// �����һ�еĵ�һ��Ԫ�ز�Ϊ0��������Ķ���Ԫ�أ��������Ԫ��δ����ǣ������һ��label
	{
		int up = data_prev[0];		// �ϵ�Ҷ�ֵ
		int neighborLabels[2];		// ���ڴ洢�����ϵ�ĻҶ�ֵ
		int cnt = 0;		// ���ڼ��������ϵ�һ���ж��ٸ���Чֵ
		if (up != 0)		// ����ϵ�����Чֵ
		{
			neighborLabels[cnt++] = up;		// ���ϵ�Ҷ�ֵ���� neighborLabels ����һ��Ԫ��
		};
		if (cnt == 0)	// ����ϵ�Ϊ0������ǰ��һ�� label
		{
			labelSet.push_back(++label);	// ���� labelSet �������һ�� label
			labelSet[label] = label;		// �����λ�õ� label ��ֵ
			data_cur[0] = label;			// �޸�Դͼ���ػҶ�ֵΪ label
		}
		else
		{
			int smallestLabel = neighborLabels[0];				// ��������ϵ�ֻ��һ����Чֵ����ô��ѡ�����Чֵ�� label ��ΪС��label
			data_cur[0] = smallestLabel;	// ��Դͼ��õ�Ҷ�ֵ��ֵΪС�� label

			// ������С�ȼ۱�  
			for (int k = 0; k < cnt; k++)
			{
				int tempLabel = neighborLabels[k];
				int & oldSmallestLabel = labelSet[tempLabel];
				if (oldSmallestLabel > smallestLabel)
				{
					labelSet[oldSmallestLabel] = smallestLabel;
				}
				else if (oldSmallestLabel < smallestLabel)
				{
					labelSet[smallestLabel] = oldSmallestLabel;
				}
			}
			// ÿ����һ��Ŀ��㣬���������ϵ��б�ǵĻ����͸�����ͨ���Ч��С label 
			// ����Ч��ͨ���е� label �滻Ϊ��ͨ������С label 
			// ����ÿ�α���һ���µ㣬��ͨ���еĸ��� label ���Ǹ���ͨ������С�� label
			for (size_t i = 2; i < labelSet.size(); i++)
			{
				int curLabel = labelSet[i];
				int preLabel = labelSet[curLabel];
				while (preLabel != curLabel)
				{
					curLabel = preLabel;
					preLabel = labelSet[preLabel];
				}
				labelSet[i] = curLabel;
			}
		}
	}

	for (int j = 1; j < srcImage.cols; j++)
	{
		if (data_cur[j] == 0)
		{	// �����ǰ���ز���Ŀ�����أ�������������
			continue;
		}
		int left = data_cur[j - 1];	// ���Ҷ�ֵ
		int up = data_prev[j];		// �ϵ�Ҷ�ֵ
		int neighborLabels[2];		// ���ڴ洢�����ϵ�ĻҶ�ֵ
		int cnt = 0;		// ���ڼ��������ϵ�һ���ж��ٸ���Чֵ
		if (left != 0)		// ����������Чֵ
		{
			neighborLabels[cnt++] = left;	// �����Ҷ�ֵ���� neighborLabels ��һ��Ԫ��
		}
		if (up != 0)		// ����ϵ�����Чֵ
		{
			neighborLabels[cnt++] = up;		// ���ϵ�Ҷ�ֵ���� neighborLabels ����һ��Ԫ��
		}
		if (cnt == 0)	// ��������ϵ㶼Ϊ0
		{
			labelSet.push_back(++label);	// ���� labelSet �������һ�� label
			labelSet[label] = label;		// �����λ�õ� label ��ֵ
			data_cur[j] = label;			// �޸�Դͼ���ػҶ�ֵΪ label
			continue;						// ������һ��ѭ��
		}
		int smallestLabel = neighborLabels[0];				// ��������ϵ�ֻ��һ����Чֵ����ô��ѡ�����Чֵ�� label ��ΪС��label
		if (cnt == 2 && neighborLabels[1] < smallestLabel)	// ��������ϵ㶼����Чֵ��ѡ��С��һ�� label
		{
			smallestLabel = neighborLabels[1];
		}
		data_cur[j] = smallestLabel;	// ��Դͼ��õ�Ҷ�ֵ��ֵΪС�� label

		// ������С�ȼ۱�  
		for (int k = 0; k < cnt; k++)
		{
			int tempLabel = neighborLabels[k];
			int & oldSmallestLabel = labelSet[tempLabel];
			if (oldSmallestLabel > smallestLabel)
			{
				labelSet[oldSmallestLabel] = smallestLabel;
			}
			else if (oldSmallestLabel < smallestLabel)
			{
				labelSet[smallestLabel] = oldSmallestLabel;
			}
		}
		// ÿ����һ��Ŀ��㣬���������ϵ��б�ǵĻ����͸�����ͨ���Ч��С label 
		// ����Ч��ͨ���е� label �滻Ϊ��ͨ������С label 
		// ����ÿ�α���һ���µ㣬��ͨ���еĸ��� label ���Ǹ���ͨ������С�� label
		for (size_t i = 2; i < labelSet.size(); i++)
		{
			int curLabel = labelSet[i];
			int preLabel = labelSet[curLabel];
			while (preLabel != curLabel)
			{
				curLabel = preLabel;
				preLabel = labelSet[preLabel];
			}
			labelSet[i] = curLabel;
		}
	}
////////								�������һ��						    			///////
////////									����							    			///////
///////////////////////////////////////////////////////////////////////////////////////////////////

		// ���µȼ۶��б�,����С��Ÿ��ظ�����  
		for (size_t i = 2; i < labelSet.size(); i++)
		{
			int curLabel = labelSet[i];
			int preLabel = labelSet[curLabel];
			while (preLabel != curLabel) {
				curLabel = preLabel;
				preLabel = labelSet[preLabel];
			}
			labelSet[i] = curLabel;
		}

		///////////////////////////////////////////////////////////
		//				     second pass                         //
		///////////////////////////////////////////////////////////

		// ��ͼ�и������ص� label ����ͬ��ͨ����С label
		for (int i = 0; i < srcImage.rows; i++)
		{
			data_cur = srcImage.ptr<uchar>(i);		// ��ǰ����ָ��
			for (int j = 0; j < srcImage.cols; j++)
			{
				data_cur[j] = labelSet[data_cur[j]];
			}
		}

		srcImage.copyTo(dst);
}

void czh_labeling_backup(Mat & src, Mat & dst)
{
	// �ú�����ʹ�� two - pass �㷨��Զ�ֵͼ������ͨ����

	// ׼��������ʱ����
	Mat srcImage; 
	src.copyTo(srcImage);

	///////////////////////////////////////////////////////////
	//				      first pass                         //
	///////////////////////////////////////////////////////////

	// ��Ϊ�� 8 ������������Կ�͸߸���1����ָֹ�����
	const int height = srcImage.rows - 1;
	const int width = srcImage.cols - 1;

	uchar *data_cur, *data_prev;	// ��ǰ�к���һ����ָ��
	int label = 1;
	vector<int> labelSet;
	labelSet.push_back(0);
	labelSet.push_back(1);
	for (int i = 1; i < height; i++)
	{
		data_cur = srcImage.ptr<uchar>(i);		// ��ǰ����ָ��
		data_prev = srcImage.ptr<uchar>(i - 1);	// ��һ����ָ��

		for (int j = 1; j < width; j++)
		{	// ����ͼ��ÿһ������
			if (data_cur[j] == 0)
			{	// �����ǰ���ز���Ŀ�����أ�������������
				continue;
			}
			int left = data_cur[j - 1];	// ���Ҷ�ֵ
			int up = data_prev[j];		// �ϵ�Ҷ�ֵ
			int neighborLabels[2];		// ���ڴ洢�����ϵ�ĻҶ�ֵ
			int cnt = 0;		// ���ڼ��������ϵ�һ���ж��ٸ���Чֵ
			if (left != 0)		// ����������Чֵ
			{
				neighborLabels[cnt++] = left;	// �����Ҷ�ֵ���� neighborLabels ��һ��Ԫ��
			}
			if (up != 0)		// ����ϵ�����Чֵ
			{
				neighborLabels[cnt++] = up;		// ���ϵ�Ҷ�ֵ���� neighborLabels ����һ��Ԫ��
			}
			if (cnt == 0)	// ��������ϵ㶼Ϊ0
			{
				labelSet.push_back(++label);	// ���� labelSet �������һ�� label
				labelSet[label] = label;		// �����λ�õ� label ��ֵ
				data_cur[j] = label;			// �޸�Դͼ���ػҶ�ֵΪ label
				continue;						// ������һ��ѭ��
			}
			int smallestLabel = neighborLabels[0];				// ��������ϵ�ֻ��һ����Чֵ����ô��ѡ�����Чֵ�� label ��ΪС��label
			if (cnt == 2 && neighborLabels[1] < smallestLabel)	// ��������ϵ㶼����Чֵ��ѡ��С��һ�� label
			{
				smallestLabel = neighborLabels[1];
			}
			data_cur[j] = smallestLabel;	// ��Դͼ��õ�Ҷ�ֵ��ֵΪС�� label

			// ������С�ȼ۱�  
			for (int k = 0; k < cnt; k++)
			{
				int tempLabel = neighborLabels[k];
				int & oldSmallestLabel = labelSet[tempLabel];
				if (oldSmallestLabel > smallestLabel)
				{
					labelSet[oldSmallestLabel] = smallestLabel;
				}
				else if (oldSmallestLabel < smallestLabel)
				{
					labelSet[smallestLabel] = oldSmallestLabel;
				}
			}

		}
	}

	// ���µȼ۶��б�,����С��Ÿ��ظ�����  
	for (size_t i = 2; i < labelSet.size(); i++)
	{
		int curLabel = labelSet[i];
		int preLabel = labelSet[curLabel];
		while (preLabel != curLabel) {
			curLabel = preLabel;
			preLabel = labelSet[preLabel];
		}
		labelSet[i] = curLabel;
	}

	///////////////////////////////////////////////////////////
	//				     second pass                         //
	///////////////////////////////////////////////////////////

	// ��ͼ�и������ص� label ����ͬ��ͨ����С label
	for (int i = 0; i < height; i++)
	{
		data_cur = srcImage.ptr<uchar>(i);		// ��ǰ����ָ��
		for (int j = 0; j < width; j++)
		{
			data_cur[j] = labelSet[data_cur[j]];
		}
	}

	srcImage.copyTo(dst);
}


////////////////////////////////////////////////////////////////////////////////
//							����任����ṹ								////
struct LinePolar															////
{																			////
	float rho;																////
	float angle;															////
};																			////																		////
////////////////////////////////////////////////////////////////////////////////

void czh_myHoughLines(const Mat & img, std::vector<Vec2f>& lines, float rho, float theta, int thresholdTotal, int thresholdRho, int thresholdAngle)
{
	// �˺����Ǹ���OpenCV��ı�׼�����߱任�ı�ģ������㷨��δ�ı䣬�����޸����ж�����
	// # 1 �ú��������Ȱ��ջ���任��ֱ�߲��������ڵ������У��������ڻ���ƽ��֮�У�
	// # 2 �Ի���ƽ������ͨ����
	// # 3 ��ÿ����ͨ����һ�����ģ������Ŀ�� lines ������
	// # 4 Ȼ��鲢�����������ֱ��

	int i, j;
	float irho = 1 / rho;
	CV_Assert(img.type() == CV_8UC1);	// ���ͼ���Ƿ�Ϊ��ͨ��

										//���ͼ������
	const uchar* image = img.ptr();
	//����������
	int step = (int)img.step;
	int width = img.cols;
	int height = img.rows;

	//����theta��rho�������ֱ��ʣ�����houghͼ�ĸ߿�
	int numangle = cvRound(CV_PI / theta);
	int numrho = cvRound(((width + height) * 2 + 1) / rho);

	//_accum�Ǵ��houghͼ���ۻ�ֵ������Ӧhoughͼ�Ķ�ά����
	AutoBuffer<int> _accum((numangle + 2) * (numrho + 2));

	//������ɢ��sin(theta)��cos(theta)
	AutoBuffer<float> _tabSin(numangle);
	AutoBuffer<float> _tabCos(numangle);
	int *accum = _accum;
	float *tabSin = _tabSin, *tabCos = _tabCos;

	memset(accum, 0, sizeof(accum[0]) * (numangle + 2) * (numrho + 2));	//�ڴ渳ֵΪ��

	float ang = 0;
	for (int n = 0; n < numangle; ang += theta, n++)
	{
		tabSin[n] = (float)(sin((double)ang) * irho);
		tabCos[n] = (float)(cos((double)ang) * irho);
	}


	/////////////////////////////////////////////////////////////////////////////////////////////
	//																						   //
	// 	      # 1 ���Ȱ��ջ���任��ֱ�߲��������ڵ������У��������ڻ���ƽ��֮�У�	   		   //
	//																						   //
	/////////////////////////////////////////////////////////////////////////////////////////////

	for (i = 0; i < height; i++)
		for (j = 0; j < width; j++)
		{	// ����Դͼ��ÿһ������
			if (image[i * step + j] != 0)	// �����������ǰ������
				for (int n = 0; n < numangle; n++)	// ��������Ļ���任���ߣ�����������Ӧλ��
				{
					int r = cvRound(j * tabCos[n] + i * tabSin[n]);
					r += (numrho - 1) / 2;
					accum[(n + 1) * (numrho + 2) + r + 1]++;
				}
		}

	/////////////////////////////////////////////////////////////////////////////////////////////
	//																						   //
	//			                    	# 2 �Ի���ƽ������ͨ����				     		   //
	//																						   //
	/////////////////////////////////////////////////////////////////////////////////////////////
	Mat maskImage(numangle + 2, numrho + 2, CV_8UC1, Scalar::all(0));

	for (int r = 0; r < numrho; r++)
		for (int n = 0; n < numangle; n++)
		{
			int base = (n + 1) * (numrho + 2) + r + 1;
			if (accum[base] > thresholdTotal)	// ���ڻ���ƽ���ϵĲ��������ĳһ�����Ե�ֵ������ֵ������ mask ͼ������Ӧ�ĵ���Ϊ��ɫ
			{
				maskImage.at<uchar>(n + 1, r + 1) = 255; // ע������Ϊ�˺�ԭ����任��������һ�£�mask ͼ��ߴ�� accum �ߴ�һ��
			}
		}

	czh_labeling(maskImage, maskImage);

	/////////////////////////////////////////////////////////////////////////////////////////////
	//																						   //
	//				# 3 ��ÿ����ͨ����һ�����ģ������Ŀ�� lines ������						   //
	//																						   //
	/////////////////////////////////////////////////////////////////////////////////////////////
	// ά��һ����СΪ256��ʸ����ÿһ��ʸ��Ԫ�أ���ʸ������һ����ʸ�������ڴ�����ػҶ�ֵ��ͬ�ĵ�
	vector<vector<Point>> pointsSet(256);	// �������ػҶ�Ϊ 1 �ĵ㶼����� pointSet[1] ֮��

	Point curPoint;	// ��ǰ��Point����
	int curValue;	// ��ǰ��Ҷ�ֵ
	MatIterator_<uchar> itBegin, itEnd;	// ͼ�������

	for (itBegin = maskImage.begin<uchar>(), itEnd = maskImage.end<uchar>(); itBegin != itEnd; itBegin++)
	{	// �������ͼ����ÿһ�����أ�����ͬ���صĵ�Ž�һ��ʸ��
		if ((*itBegin) != 0)	// �����ǰ��Ҷ�ֵ��Ϊ��
		{
			curValue = (*itBegin);	// ��ǰ��Ҷ�ֵ
			curPoint = itBegin.pos();	// ��ǰ��Point����
			pointsSet[curValue].push_back(curPoint);	// �ѵ�ǰ��ѹ���Ӧʸ����
		}
	}

	for (int i = 0; i < pointsSet.size(); i++)
	{	// ���� pointsSet ������256����ʸ��

		if (pointsSet[i].size() == 0)
		{	// ���ĳ����ʸ����һ���㶼û�У�������
			continue;
		}

		// ��ʼ��
		int nCur, rCur;
		int total_n_values = 0, total_r_values = 0, total_values = 0;

		for (int j = 0; j < pointsSet[i].size(); j++)
		{	// ����ÿһ����ʸ���еĵ�Ԫ��,��ø���ͨ��� r ֵ�� n ֵ
			nCur = pointsSet[i][j].y - 1;
			rCur = pointsSet[i][j].x - 1;
			total_n_values += accum[(nCur + 1) * (numrho + 2) + rCur + 1] * nCur;
			total_r_values += accum[(nCur + 1) * (numrho + 2) + rCur + 1] * rCur;
			total_values += accum[(nCur + 1) * (numrho + 2) + rCur + 1];
		}

		// ��øûҶ�ֵ��Ⱥ������
		nCur = cvRound(total_n_values / total_values);
		rCur = cvRound(total_r_values / total_values);

		LinePolar line;
		line.rho = (rCur - (numrho - 1)*0.5f) * rho;
		line.angle = nCur * theta;
		lines.push_back(Vec2f(line.rho, line.angle));
	}


	/////////////////////////////////////////////////////////////////////////////////////////////
	//																						   //
	//								# 4 Ȼ��鲢�����������ֱ��				     		   //
	//																						   //
	/////////////////////////////////////////////////////////////////////////////////////////////
	// ���Ѿ���õ�ֱ�߲����������б���
	for (size_t i = 0; i < lines.size(); i++)
	{
		float rhoCur = lines[i][0];
		float angleCur = lines[i][1];

		float rhoTotal = rhoCur;
		float angleTotal = angleCur;
		int numProche = 1;

		// �����������ֱ�߲����Ժ͵�ǰֱ�߲������������������ǵ�ƽ��ֵ������ǰ�����ԣ������������������
		for (size_t j = i + 1; j < lines.size(); j++)
		{	
			if ( (abs(lines[j][0] - rhoCur) < thresholdRho) && ( abs(lines[j][1] - angleCur) / CV_PI * 180 < thresholdAngle))
			{
				rhoTotal += lines[j][0];
				angleTotal += lines[j][1];
				numProche++;
				lines[j][0] = lines[j][1] = 0;
			}
		}
		
		lines[i][0] = rhoTotal / numProche;
		lines[i][1] = angleTotal / numProche;
	}

	/////////////////////////////////////////////////////////////////////////////////////////////
	//																						   //
	//								# 4 Ȼ��鲢�����������ֱ��				     		   //
	//																						   //
	/////////////////////////////////////////////////////////////////////////////////////////////
	vector<Vec2f> finalLines;
	Vec2f tempLine;
	for (size_t i = 0; i < lines.size(); i++)
	{
		if ((lines[i][0] == 0) && (lines[i][1] == 0))	// ������Ѿ�������Ĳ����ԣ�������
			continue;
		// ���򽫸ò�������ӵ���������֮��
		tempLine[0] = lines[i][0];
		tempLine[1] = lines[i][1];
		finalLines.push_back(tempLine);
	}

	lines.clear();	// ���ԭ����
	lines = finalLines;	// ��������������ԭ����
}

void czh_centerOfGravity(Mat & labelledImage, Mat & dstImage, vector<Point> & centerPoints)
{
	// �ú�������Ѿ�����ͨ���Ǵ������ͼ��Ѱ��ÿ����ͨ������ģ������ dstImage ֮��,����������������� centerPoints ֮��

	Mat tempImage(labelledImage.size(), CV_8UC1, Scalar::all(0));
	// ά��һ����СΪ256��ʸ����ÿһ��ʸ��Ԫ�أ���ʸ������һ����ʸ�������ڴ�����ػҶ�ֵ��ͬ�ĵ�
	vector<vector<Point>> pointsSet(256);	// �������ػҶ�Ϊ 1 �ĵ㶼����� pointSet[1] ֮��

	Point curPoint;	// ��ǰ��Point����
	int curValue;	// ��ǰ��Ҷ�ֵ
	MatIterator_<uchar> itBegin, itEnd;	// ͼ�������

	for (itBegin = labelledImage.begin<uchar>(), itEnd = labelledImage.end<uchar>(); itBegin != itEnd; itBegin++)
	{	// �������ͼ����ÿһ�����أ�����ͬ���صĵ�Ž�һ��ʸ��
		if ((*itBegin) != 0)	// �����ǰ��Ҷ�ֵ��Ϊ��
		{
			curValue = (*itBegin);	// ��ǰ��Ҷ�ֵ
			curPoint = itBegin.pos();	// ��ǰ��Point����
			pointsSet[curValue].push_back(curPoint);	// �ѵ�ǰ��ѹ���Ӧʸ����
		}
	}

	vector<Point> cornerCenterPointSet;	// ׼�����յĽ����ʸ��

	for (int i = 0; i < pointsSet.size(); i++)
	{	// ���� pointsSet ������256����ʸ��
		if (pointsSet[i].size() == 0)
		{	// ���ĳ����ʸ����һ���㶼û�У�������
			continue;
		}

		// ��ʼ�� x,y ֵ�ܺ�
		int total_x = 0;
		int total_y = 0;;
		int total_points = pointsSet[i].size();// �ûҶ�ֵ�ж��ٸ���

		for (int j = 0; j < pointsSet[i].size(); j++)
		{	// ����ÿһ����ʸ���еĵ�Ԫ��,���x��ֵ��y��ֵ
			total_x += pointsSet[i][j].x;
			total_y += pointsSet[i][j].y;
		}

		// ��øûҶ�ֵ��Ⱥ������
		int x = total_x / total_points;
		int y = total_y / total_points;
		cornerCenterPointSet.push_back(Point(x, y));	// �Ѹ�����ѹ�� cornerPointSet ʸ����,�Ա��Ժ�ʹ��
		tempImage.at<uchar>(y, x) = 255;	// ������ͼ�� Mat �����аѽǵ���Ϊ��ɫ
	}

	dstImage = tempImage;
	vector<Point>().swap(centerPoints);	// ����������
	centerPoints = cornerCenterPointSet;// �����������������������
}

void czh_bwAreaOpen(Mat & srcImage, Mat & dstImage, int zoneHeight, int zoneWidth, int threshold)
{
	// 	�ú���ʵ���� Matlab �� bwareaopen �����Ĺ��ܣ���һ����ֵͼ��ȥ�룬ȥ����ͨ�����С�� threshold �ĵ�

	Mat tempImage, tempRectImage;
	srcImage.copyTo(tempImage);

	int numRows = srcImage.rows / zoneHeight;
	int numCols = srcImage.cols / zoneWidth;

	Point curPoint;	// ��ǰ��Point����
	int curValue;	// ��ǰ��Ҷ�ֵ
	MatIterator_<uchar> itBegin, itEnd;	// ͼ�������

	for (int i = 0; i < numRows; i++)
		for (int j = 0; j < numCols; j++)
		{
			// ���ÿһ���ӿ������ͨ����
			tempRectImage = srcImage(Rect(j*zoneWidth, i*zoneHeight, zoneWidth, zoneHeight));
			czh_labeling(tempRectImage, tempRectImage);

			// ά��һ����СΪ256��ʸ����ÿһ��ʸ��Ԫ�أ���ʸ������һ����ʸ�������ڴ�����ػҶ�ֵ��ͬ�ĵ�
			vector<vector<Point>> pointsSet(256);	// �������ػҶ�Ϊ 1 �ĵ㶼����� pointSet[1] ֮��

			for (itBegin = tempRectImage.begin<uchar>(), itEnd = tempRectImage.end<uchar>(); itBegin != itEnd; itBegin++)
			{	// �������ͼ����ÿһ�����أ�����ͬ���صĵ�Ž�һ��ʸ��
				if ((*itBegin) != 0)	// �����ǰ��Ҷ�ֵ��Ϊ��
				{
					curValue = (*itBegin);	// ��ǰ��Ҷ�ֵ
					curPoint = itBegin.pos();	// ��ǰ��Point����
					pointsSet[curValue].push_back(curPoint);	// �ѵ�ǰ��ѹ���Ӧʸ����
				}
			}

			for (int itPts = 0; itPts < pointsSet.size(); itPts++)
			{	// ���� pointsSet ������256����ʸ��
				if (pointsSet[itPts].size() == 0)
				{	// ���ĳ����ʸ����һ���㶼û�У�������
					continue;
				}

				if (pointsSet[itPts].size() < threshold)
				{	// ���ĳ����ͨ�����С����ֵ����Ѹ���ͨ���ϵĵ���Ϊ��ɫ
					for (int nPts = 0; nPts < pointsSet[itPts].size(); nPts++)
						tempImage.at<uchar>(pointsSet[itPts][nPts].y + i*zoneHeight, pointsSet[itPts][nPts].x + j*zoneWidth) = 0;
				}
			}
		}
	dstImage = tempImage;
}

void czh_guidedFilter(Mat & srcImage, Mat & guidanceImage, Mat & dstImage, int radius, float epsilon, int outputType)
{
	// �ú���ʵ���˺ο�����ʿ�� guided filter �����˲��㷨
	CV_Assert(radius >= 2 && epsilon > 0);
	CV_Assert(srcImage.data != NULL && srcImage.channels() == 1);
	CV_Assert(guidanceImage.channels() == 1);
	CV_Assert(srcImage.rows == guidanceImage.rows && srcImage.cols == guidanceImage.cols);

	// ��ʼ�㷨
	Mat P, I;

	// ��Դͼ�������ͼ��ת��Ϊ������: �Ա��ں�ߵĳ�������
	srcImage.convertTo(I, CV_32F, 1.0 / 255.0);	// ����ͼ�� I = guidance image
	srcImage.convertTo(P, CV_32F, 1.0 / 255.0);	// ԭͼ�� P = source image

	//	����I*p��I*I  
	Mat image_IP, image_II;
	multiply(P, I, image_IP);	// image_IP = image I * image P
	multiply(I, I, image_II);	// image_I2 = image I * image I

	// �˲����ڴ�С
	Size window_size(2 * radius + 1, 2 * radius + 1);

	//������־�ֵ  
	Mat mean_I, mean_P, corr_I, corr_IP;
	boxFilter(I, mean_I, CV_32F, window_size);	// meanI = mean of input image I
	boxFilter(P, mean_P, CV_32F, window_size);	// meanP = mean of guidance image P
	boxFilter(image_II, corr_I, CV_32F, window_size);	// corr_I = mean of image_I2
	boxFilter(image_IP, corr_IP, CV_32F, window_size);	// corr_IP = mean of image_IP

	//����Ip��Э�����I�ķ���  
	Mat var_I, cov_IP;
	var_I = corr_I - mean_I.mul(mean_I);
	cov_IP = corr_IP - mean_I.mul(mean_P);

	// �� a,b
	Mat a, b;
	var_I += epsilon;
	divide(cov_IP, var_I, a);
	b = mean_P - a.mul(mean_I);

	//�԰�������i������a��b��ƽ��  
	Mat mean_a, mean_b;
	boxFilter(a, mean_a, CV_32F, window_size);
	boxFilter(b, mean_b, CV_32F, window_size);

	// �����յļ���
	Mat tempDstImage(I.size(), CV_32F);
	tempDstImage = mean_a.mul(I) + mean_b;

	dstImage = tempDstImage;	// �����ս���� dstImage ��������
	if (outputType == CV_8UC1)
	{
		dstImage.convertTo(dstImage, CV_8UC1, 255);	// ת����ʽ
	}
	else if ((outputType == CV_32FC1) || (outputType == CV_32FC1)){}
}

void czh_findValley(Mat & srcImage, Mat & dstImage, int valleyRadius, int valleyThreshold1, int valleyThreshold2)
{
	// �ú�������ͼ���е� valley ����

	CV_Assert(srcImage.channels() == 1 && dstImage.channels() == 1);
	CV_Assert(valleyRadius >= 1);
	CV_Assert(valleyThreshold1 >= 0 && valleyThreshold2 <= 255 && valleyThreshold1 <= valleyThreshold2);

	int curValue;
	bool jumpThisPixel_Horizontal, jumpThisPixel_rVertical;
	uchar *ptr_u_src, *ptr_u_dst;
	Mat tempImage(srcImage.size(), CV_8UC1, Scalar::all(0));

	for (int i = valleyRadius; i < srcImage.rows - valleyRadius; i++)	// ����ÿһ������Χ�ڵ�����
	{
		ptr_u_src = srcImage.ptr(i);	// ��ǰԴͼ��������ָ��
		ptr_u_dst = tempImage.ptr(i);	// ��ǰĿ��ͼ��������ָ��

		for (int j = valleyRadius; j < srcImage.cols - valleyRadius; j++)// ����ÿһ������Χ�ڵ�����
		{
			if (ptr_u_src[j] < valleyThreshold1 || ptr_u_src[j] > valleyThreshold2)	// �����ǰ���ػҶ�ֵ����ֵС��������������
				continue;

			curValue = ptr_u_src[j];			// ��ǰ������ֵ
			jumpThisPixel_Horizontal = false;	// ������ˮƽ�����б��
			jumpThisPixel_rVertical = false;	// �����˴�ֱ�����б��

			////////////////////////////////////////
			//   horizontal ˮƽ������� valley   //
			////////////////////////////////////////
			// �����������һ����Χ�����أ�����бȵ�ǰ���ظ�С�����أ���ǰ���ز��� valley
			for (int ptr = -valleyRadius; ptr <= valleyRadius; ptr++)
				if (ptr_u_src[j + ptr] < curValue)
				{
					jumpThisPixel_Horizontal = true;
					break;
				}

			// ���û����������˵���õ��Ǻ���ֲ���Сֵ������Ŀ��ͼ������Ϊ��ɫ
			if (jumpThisPixel_Horizontal == false)
				ptr_u_dst[j] = 255;

			//////////////////////////////////////
			//   vertical ��ֱ������� valley   //
			//////////////////////////////////////
			// �����������һ����Χ�����أ�����бȵ�ǰ���ظ�С�����أ���ǰ���ز��� valley
			for (int ptr = -valleyRadius; ptr <= valleyRadius; ptr++)
				if (srcImage.at<uchar>(i + ptr, j) < curValue)
				{
					jumpThisPixel_rVertical = true;
					break;
				}

			// ���û����������˵���õ�������ֲ���Сֵ������Ŀ��ͼ������Ϊ��ɫ
			if (jumpThisPixel_rVertical == false)
				tempImage.at<uchar>(i, j) = 255;
		}
	}
	dstImage = tempImage;
}