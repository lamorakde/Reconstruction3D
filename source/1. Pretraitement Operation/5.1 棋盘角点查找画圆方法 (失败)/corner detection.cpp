#include "opencv2/opencv.hpp"
#include <iostream>
#include <windows.h>
#include <fstream>
#include "czh_binary_CV.h"

using namespace std;
using namespace cv;

enum {ONE, TWO};

int radius = 10;	// ��Բ�㷨�뾶

int main()
{
	// ���������Ϣ
	string programInformation = "�ó�����Զ�Ѱ�����̵ȱ궨ͼ�еĽǵ�.";
	string fileType = ".pgm";
	czh_helpInformation(programInformation);

	// �õ�����ͼ��������Ϣ�����
	cout << "Enter the input image name without " << fileType << ": ";
	string fileName, srcFileName;

	fileName = "real image 3"; 

	// getline(cin, fileName);				// ��������ļ���
	srcFileName = fileName + fileType;	// ȷ������ͼƬ�ļ�����
	Mat src = imread(srcFileName,0);	// ��ȡԴͼ��
	czh_imageOpenDetect(src, fileName, fileType);	// ���Դͼ���Ƿ�ɹ���
	// ��ʾԴͼ����Ϣ
	cout << "\nInput image name:\t" << srcFileName << endl
		<< "Image size:\t\t" << src.cols << "*" << src.rows << endl
		<< "Image pixels:\t\t" << src.cols*src.rows << endl
		<< "Image type:\t\t" << src.type() << endl;

	// ����ֵ�ָ��ͼ��ת��Ϊ��ֵͼ��
	// imshow("Gray scale image", src);	// ��ʾԭʼ�Ҷ�ͼ��
	Mat srcImage;
	threshold(src, srcImage, 150, 255, 0);	// ��ֵ�ָ�
	// imshow("Binary Image", srcImage);	// ��ʾ��ֵͼ��
	czh_imwrite(srcImage, "binary");	// �����ֵͼ��

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


// 	// test begins
// 		Mat t_Image_bw(srcImage.size(), CV_8UC3, Scalar::all(0)); 
// 		Mat t_Image_or(srcImage.size(), CV_8UC3, Scalar::all(0));
// 		for (int i = 0; i < t_Image_bw.rows; i++)
// 		{
// 			for (int j = 0; j < t_Image_bw.cols; j++)
// 			{
// 				// ��Դͼ��ֵͼ���ֵ�������Զ�ֵͼ����ͨ����
// 				t_Image_bw.at<Vec3b>(i, j)[0] = srcImage.at<uchar>(i, j);
// 				t_Image_bw.at<Vec3b>(i, j)[1] = srcImage.at<uchar>(i, j);
// 				t_Image_bw.at<Vec3b>(i, j)[2] = srcImage.at<uchar>(i, j);
// 				// ��Դͼ�Ҷ�ͼ���ֵ�������ԻҶ�ͼ����ͨ����
// 				t_Image_or.at<Vec3b>(i, j)[0] = src.at<uchar>(i, j);
// 				t_Image_or.at<Vec3b>(i, j)[1] = src.at<uchar>(i, j);
// 				t_Image_or.at<Vec3b>(i, j)[2] = src.at<uchar>(i, j);
// 			}
// 		}
// 		vector<Point> pointsTest;	// ���ڴ�������ص�ΪԲ�ĵ�Բ���ص�ʸ������������
// 		vector<int> t_value;
// 		vector<int> t_regionPointNum;
// 		czh_BresenhamCircle(203, 199, radius, pointsTest);	// ������Գ�ʼ��ΪԲ������õ�Բ���꣺ԪԲ����
// 	
// 		for (int it = 0; it < pointsTest.size(); it++)
// 		{
// 			
// 			// �ڲ��Զ�ֵͼ���ϻ�Բ
// 			t_Image_bw.at<Vec3b>(pointsTest[it])[2] = 255;
// 			t_Image_bw.at<Vec3b>(pointsTest[it])[1] = 0;
// 			t_Image_bw.at<Vec3b>(pointsTest[it])[0] = 0;
// 			// �ڲ��ԻҶ�ͼ���ϻ�Բ
// 			t_Image_or.at<Vec3b>(pointsTest[it].y, pointsTest[it].x)[2] = 255;
// 			t_Image_or.at<Vec3b>(pointsTest[it].y, pointsTest[it].x)[1] = 0;
// 			t_Image_or.at<Vec3b>(pointsTest[it].y, pointsTest[it].x)[0] = 0;
// 
// 			cout << "point #" << it + 1 << " (" << pointsTest[it].x << " , " << pointsTest[it].y << ") : "
// 				<< (int)srcImage.at<uchar>(pointsTest[it]) << endl;
// 
// 			t_value.push_back((int)srcImage.at<uchar>(pointsTest[it]));
// 		}
// 		t_value.push_back((int)srcImage.at<uchar>(pointsTest[0]));	// �������һ��Ԫ�أ������һ�����صĻҶ���ͬ
// 		
// 		int dif = 0;	// �л�������ʼ��Ϊ0
// 		t_regionPointNum.clear();
// 		t_regionPointNum.push_back(1);// ��ʼ����һ��������������ֵ����Ϊ1����Ϊ�±ߵ�ѭ��ֱ�Ӵӵڶ����㿪ʼ
// 	
// 		for (int i = 1; i < t_value.size(); i++)
// 		{	// ����Բ���ػҶ�ֵʸ����ÿһ����
// 	
// 			// �����ǰ���ǰһ��ҶȲ�Ϊ255������Ϊ��ת��������
// 			if (abs(t_value[i] - t_value[i - 1]) == 255)
// 			{
// 				dif++;	// �л���������
// 				t_regionPointNum.push_back(0);	// ������������
// 			}
// 			// �� dif ���������ص�������
// 			t_regionPointNum[dif]++;
// 		}
// 	
// 		for (int i = 0; i < t_regionPointNum.size(); i++)
// 		{
// 			cout << "region # " << i << ": " << t_regionPointNum[i] << endl;
// 		}
// 		cout << "dif : " << dif << endl;
// 		cout << "abs(regionPointNum[0] + regionPointNum[2] + regionPointNum[4] - regionPointNum[1] - regionPointNum[3]) : "
// 			<< abs(t_regionPointNum[0] + t_regionPointNum[2] + t_regionPointNum[4] - t_regionPointNum[1] - t_regionPointNum[3]) << endl;
// 		cout << "abs(regionPointNum[1] - regionPointNum[3]) : " << abs(t_regionPointNum[1] - t_regionPointNum[3]) << endl;
// 		cout << "abs(regionPointNum[0] + regionPointNum[4] - regionPointNum[2])" 
// 			 << abs(t_regionPointNum[0] + t_regionPointNum[4] - t_regionPointNum[2]) << endl;
// // 		imshow("test_gray", t_Image_or);
// // 		imshow("test_bw", t_Image_bw);
// 		czh_imwrite(t_Image_bw, "test_bw");
// 		czh_imwrite(t_Image_or, "test_or");
// 		//waitKey(0);
// 		//////////////////////////////////////////////////////////
// 		//////////////////////////////////////////////////////////
// 		// test end


	//...............................................//
	//												 //
	//				     STEP.1                      //
	//			 Ѱ�����п����ǽǵ������			 //
	//												 //
	//...............................................//

	Mat pointsImage(srcImage.size(), CV_8UC1, Scalar::all(0));
	
	// ����Ըĵ�ΪԲ�ģ�radiusΪ�뾶��Բ�ϵ����ص�
	// int radius = 9;	// ��Բ�㷨�뾶
	vector<Point> pointsOfCircleOriginal;	// ���ڴ�������ص�ΪԲ�ĵ�Բ���ص�ʸ����Ԫ����
	vector<Point> pointsOfCircle;	// ���ڴ�������ص�ΪԲ�ĵ�Բ���ص�ʸ������������
	vector<Point> pointsOfCircleOriginal_2;	// ���ڴ�������ص�ΪԲ�ĵ�Բ���ص�ʸ����Ԫ����
	vector<Point> pointsOfCircle_2;	// ���ڴ�������ص�ΪԲ�ĵ�Բ���ص�ʸ������������
	czh_BresenhamCircle(radius, radius, radius, pointsOfCircleOriginal);	// ������Գ�ʼ��ΪԲ������õ�Բ���꣺ԪԲ����
	czh_BresenhamCircle(radius, radius, radius, pointsOfCircle);	// ������Գ�ʼ��ΪԲ������õ�Բ���꣺��ǰԲ����
	czh_BresenhamCircle(radius, radius, radius - 4, pointsOfCircleOriginal_2);
	czh_BresenhamCircle(radius, radius, radius - 4, pointsOfCircle_2);
	vector<int> valuesOfPoints;
	vector<int> regionPointNum;	// ��¼ÿһ�����������������ֵ����

	for (int i = radius; i < srcImage.rows - radius; i++)
	{	// ����������ɨ�裬Ѱ��������ΪԲ�ģ�radiusΪ�뾶��Բ�Ϻڰ�������ȵ������أ�ͨ�����㵱ǰ����ͳ�ʼ�����λ���������㵱ǰԲ����
		// ��¼Բ�����ػҶ�ֵ�ڰ��л�������ֻ������4���л��ģ�˵���õ��ڽ��㸽��
		// ͬʱ�������Բ�ϵİ�ɫ���ص��ں�ɫ���أ�˵������ǽ���
		for (int j = radius; j < srcImage.cols - radius; j++)
		{
			// ����Ըõ�ΪԲ�ģ�radiusΪ�뾶��Բ�ϵ����ص�
			int offset_x = j - radius;	// x λ����
			int offset_y = i - radius;	// y λ����

			valuesOfPoints.clear();	// ���Բ���ػҶ�ֵʸ��
			for (int p = 0; p < pointsOfCircle.size(); p++)
			{	// ͨ�����㵱ǰ����ͳ�ʼ�����λ���������㵱ǰԲ�ϵ���������
				pointsOfCircle[p].x = pointsOfCircleOriginal[p].x + offset_x;
				pointsOfCircle[p].y = pointsOfCircleOriginal[p].y + offset_y;
				// ��Բ�ϵ����صĻҶ�ֵ���� valuesOfPoints ʸ��֮��
				valuesOfPoints.push_back((int)srcImage.at<uchar>(pointsOfCircle[p]));
			}
			valuesOfPoints.push_back((int)srcImage.at<uchar>(pointsOfCircle[0]));	// �������һ��Ԫ�أ������һ�����صĻҶ���ͬ

			int dif = 0;	// �л�������ʼ��Ϊ0
			regionPointNum.clear();
			regionPointNum.push_back(1);// ��ʼ����һ��������������ֵ����Ϊ1����Ϊ�±ߵ�ѭ��ֱ�Ӵӵڶ����㿪ʼ

			for (int i = 1; i < valuesOfPoints.size(); i++)
			{	// ����Բ���ػҶ�ֵʸ����ÿһ����

				// �����ǰ���ǰһ��ҶȲ�Ϊ255������Ϊ��ת��������
				if (abs(valuesOfPoints[i] - valuesOfPoints[i - 1]) == 255)
				{
					dif++;	// �л���������
					regionPointNum.push_back(0);	// ������������
				}

				// �� dif ���������ص�������
				regionPointNum[dif]++;
			}

			if (dif == 4)	// ����л�����Ϊ4�Σ�˵���õ����ڲ����㸽��
			{
				regionPointNum[4] -= 1;	// �����ʼ������ӵ���һ��Ԫ�أ����Ԫ�����һ�����ػҶ�ֵ��ͬ�����ظ�����һ��
				
				if ( abs( regionPointNum[0] + regionPointNum[2] + regionPointNum[4] - regionPointNum[1] - regionPointNum[3] ) < 9 )
				{	// ����õ�Բ�Ϻڰ�������ȣ���ô���뽻��ͷǳ���
					if (abs(regionPointNum[1] - regionPointNum[3]) < 9)
					{
						if( (abs(regionPointNum[0] + regionPointNum[4] - regionPointNum[2]) < 9) )
						{
							pointsImage.at<uchar>(i, j) = 255;
						}
					}
				}
			}

			if (dif == 2)	// ����л�����Ϊ���Σ������ڰױ����ǲ��ǽӽ�1:3
			{
				float black, white;
				regionPointNum[2] -= 1;	// �����ʼ������ӵ���һ��Ԫ�أ����Ԫ�����һ�����ػҶ�ֵ��ͬ�����ظ�����һ��
				if (valuesOfPoints[0] == 0)	// ����õ��һ��Բ�����Ǻ�ɫ
				{
					black = regionPointNum[0] + regionPointNum[2];
					white = regionPointNum[1];
				}
				else // ����õ��һ��Բ�����ǰ�ɫ
				{
					black = regionPointNum[1]; 
					white = regionPointNum[0] + regionPointNum[2];
				};

				if ((white / black > 2.9) && (white / black < 3.1))	// ����׺ڱ����ӽ�3������Ϊ�����Ǻ�ɫ�ǵ㣬���С�뾶���¼��Բ����
				{
					valuesOfPoints.clear();	// ���Բ���ػҶ�ֵʸ��
					for (int p = 0; p < pointsOfCircle_2.size(); p++)
					{	// ͨ�����㵱ǰ����ͳ�ʼ�����λ���������㵱ǰԲ�ϵ���������
						pointsOfCircle_2[p].x = pointsOfCircleOriginal_2[p].x + offset_x;
						pointsOfCircle_2[p].y = pointsOfCircleOriginal_2[p].y + offset_y;
						// ��Բ�ϵ����صĻҶ�ֵ���� valuesOfPoints ʸ��֮��
						valuesOfPoints.push_back((int)srcImage.at<uchar>(pointsOfCircle_2[p]));
					}
					valuesOfPoints.push_back((int)srcImage.at<uchar>(pointsOfCircle_2[0]));	// �������һ��Ԫ�أ������һ�����صĻҶ���ͬ

					int dif = 0;	// �л�������ʼ��Ϊ0
					regionPointNum.clear();
					regionPointNum.push_back(1);// ��ʼ����һ��������������ֵ����Ϊ1����Ϊ�±ߵ�ѭ��ֱ�Ӵӵڶ����㿪ʼ

					for (int i = 1; i < valuesOfPoints.size(); i++)
					{	// ����Բ���ػҶ�ֵʸ����ÿһ����
						// �����ǰ���ǰһ��ҶȲ�Ϊ255������Ϊ��ת��������
						if (abs(valuesOfPoints[i] - valuesOfPoints[i - 1]) == 255)
						{
							dif++;	// �л���������
							regionPointNum.push_back(0);	// ������������
						}
						// �� dif ���������ص�������
						regionPointNum[dif]++;
					}

					if (dif == 2)	// ���ת������Ϊ2�Σ�������Ǻ�ɫ�ǵ�
					{
						regionPointNum[2] -= 1;	// �����ʼ������ӵ���һ��Ԫ�أ����Ԫ�����һ�����ػҶ�ֵ��ͬ�����ظ�����һ��
						if (valuesOfPoints[0] == 0)	// ����õ��һ��Բ�����Ǻ�ɫ
						{
							black = regionPointNum[0] + regionPointNum[2];
							white = regionPointNum[1];
						}
						else // ����õ��һ��Բ�����ǰ�ɫ
						{
							black = regionPointNum[1];
							white = regionPointNum[0] + regionPointNum[2];
						};

						if ((white / black > 2.9) && (white / black < 3.1))
						{
							pointsImage.at<uchar>(i, j) = 255;
						}
					}
				}
			}
		}
	}
	
	// �����Ͳ��������Ӷϵ�
	Mat dilatedImage;
	dilate(pointsImage, dilatedImage, getStructuringElement(MORPH_RECT, Size(3, 3), Point(-1, -1)), Point(-1, -1), 3);

	time = ((double)getTickCount() - time) / getTickFrequency();
	cout << "\nProgram Step #1 ���ǵ㣬���� ���� :\t" << time << " second used.\n" << endl;
	imshow("Step # 1 ԭͼ", pointsImage);	// ��ʾ��һ��Ч��ͼ
	imshow("Step # 1 ����", dilatedImage);	// ��ʾ��һ������֮��Ч��ͼ


	//...............................................//
	//												 //
	//				     STEP.2                      //
	//				����ͨ���ǲ���				 //
	//												 //
	//...............................................//

	time = static_cast<double>(getTickCount());	

	Mat labelledImage;
	czh_labeling(dilatedImage, labelledImage);	// ���ú���������ͨ���ǲ���

	time = ((double)getTickCount() - time) / getTickFrequency();
	cout << "Program Step #2 ��ͨ���� ���� :\t" << time << " second used.\n" << endl;
	imshow("Step # 2 Label", labelledImage);	// ��ʾ�ڶ���Ч��ͼ

	//...............................................//
	//												 //
	//				     STEP.3                      //
	//					Ѱ������					 //
	//												 //
	//...............................................//

	time = static_cast<double>(getTickCount());

	Mat dstImage(srcImage.size(), CV_8UC1, Scalar::all(0));
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
		dstImage.at<uchar>(y, x) = 255;	// ������ͼ�� Mat �����аѽǵ���Ϊ��ɫ
	}

	time = ((double)getTickCount() - time) / getTickFrequency();
	cout << "Program Step #3 Ѱ������ ���� :\t" << time << " second used.\n" << endl;
	imshow("Step # 3 Final Result", dstImage);	// ��ʾ������Ч��ͼ


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
	for (int i = 0; i < cornerCenterPointSet.size(); i++)
	{
		ffout << cornerCenterPointSet[i].x << ",\t" << cornerCenterPointSet[i].y << endl;
	}
	ffout.close();

	//////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////////////

	// ���������Ϣ�������������ʱ��
	timeTotal = ((double)getTickCount() - timeTotal) / getTickFrequency();
	cout << "Program finished :\t" << timeTotal << " second used.\n" << endl;

	// ��������
// 	imshow("srcImage", srcImage);
//	imshow("dstImage", dstImage);
// 	czh_imwrite(pointsImage, "points");
// 	czh_imwrite(dilatedImage, "dilated");
// 	czh_imwrite(labelledImage, "label");
//  	czh_imwrite(dstImage, "corner");

	waitKey();
	system("pause");
}


