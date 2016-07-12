#include "opencv2/opencv.hpp"
#include <windows.h>

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
	// �ú�������һ����ͨ��ͼ������Сֵ�˲������ڴ�С size * size
// 	if ( (srcImage.type() != CV_32FC1) || (srcImage.type() != CV_8UC1))
// 	{
// 		cerr << "����ͼ�����Ϊ��ͨ��ͼ��.\n";
// 		return;
// 	}
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

// 	while (x < y)
// 	{
// 		pointsDirection[0].push_back(Point(x0 + x, y0 + y));
// 		pointsDirection[1].push_back(Point(x0 + y, y0 + x));// ˳����ȷ
// 		pointsDirection[6].push_back(Point(x0 - y, y0 + x));
// 		pointsDirection[7].push_back(Point(x0 - x, y0 + y));// ˳����ȷ
// 		pointsDirection[4].push_back(Point(x0 - x, y0 - y));
// 		pointsDirection[5].push_back(Point(x0 - y, y0 - x));// ˳����ȷ
// 		pointsDirection[2].push_back(Point(x0 + y, y0 - x));
// 		pointsDirection[3].push_back(Point(x0 + x, y0 - y));// ˳����ȷ
// 
// 		// �ж���һ�εĵ�����
// 		if (distance < 0)
// 		{
// 			distance += 4 * x + 6;
// 		}
// 		else
// 		{
// 			distance += 4 * (x - y) + 10;
// 			y--;
// 		}
// 		x++;
// 	}


	pointsDirection[0].push_back(Point(x0 + x, y0 + y));
	pointsDirection[1].push_back(Point(x0 + y, y0 + x));
	pointsDirection[6].push_back(Point(x0 - y, y0 + x));
	pointsDirection[7].push_back(Point(x0 - x, y0 + y));
	pointsDirection[4].push_back(Point(x0 - x, y0 - y));
	pointsDirection[5].push_back(Point(x0 - y, y0 - x));
	pointsDirection[2].push_back(Point(x0 + y, y0 - x));
	pointsDirection[3].push_back(Point(x0 + x, y0 - y));

	x++;

	while (x < y)
	{
		if (distance < 0)
		{
			distance += 4 * x + 6;
		}
		else
		{
			distance += 4 * (x - y) + 10;
			y--;
		}
		pointsDirection[0].push_back(Point(x0 + x, y0 + y));	
		pointsDirection[1].push_back(Point(x0 + y, y0 + x));// ˳����ȷ
		pointsDirection[6].push_back(Point(x0 - y, y0 + x));	
		pointsDirection[7].push_back(Point(x0 - x, y0 + y));// ˳����ȷ
		pointsDirection[4].push_back(Point(x0 - x, y0 - y));	
		pointsDirection[5].push_back(Point(x0 - y, y0 - x));// ˳����ȷ
		pointsDirection[2].push_back(Point(x0 + y, y0 - x));	
		pointsDirection[3].push_back(Point(x0 + x, y0 - y));// ˳����ȷ

		x++;
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
		for (int j = 0; j < pointsDirection[i].size(); j++)
		{
			pointsOfCircle.push_back(pointsDirection[i][j]);
		}
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
				while (preLabel != curLabel) {
					curLabel = preLabel;
					preLabel = labelSet[preLabel];
				}
				labelSet[i] = curLabel;
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

