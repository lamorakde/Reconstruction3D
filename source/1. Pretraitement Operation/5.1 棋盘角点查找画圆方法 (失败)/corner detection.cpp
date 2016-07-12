#include "opencv2/opencv.hpp"
#include <iostream>
#include <windows.h>
#include <fstream>
#include "czh_binary_CV.h"

using namespace std;
using namespace cv;

enum {ONE, TWO};

int radius = 10;	// 画圆算法半径

int main()
{
	// 程序帮助信息
	string programInformation = "该程序会自动寻找棋盘等标定图中的角点.";
	string fileType = ".pgm";
	czh_helpInformation(programInformation);

	// 得到输入图像名称信息并输出
	cout << "Enter the input image name without " << fileType << ": ";
	string fileName, srcFileName;

	fileName = "real image 3"; 

	// getline(cin, fileName);				// 获得输入文件名
	srcFileName = fileName + fileType;	// 确定输入图片文件类型
	Mat src = imread(srcFileName,0);	// 读取源图像
	czh_imageOpenDetect(src, fileName, fileType);	// 检测源图像是否成功打开
	// 显示源图像信息
	cout << "\nInput image name:\t" << srcFileName << endl
		<< "Image size:\t\t" << src.cols << "*" << src.rows << endl
		<< "Image pixels:\t\t" << src.cols*src.rows << endl
		<< "Image type:\t\t" << src.type() << endl;

	// 做阈值分割，将图像转换为二值图像
	// imshow("Gray scale image", src);	// 显示原始灰度图像
	Mat srcImage;
	threshold(src, srcImage, 150, 255, 0);	// 阈值分割
	// imshow("Binary Image", srcImage);	// 显示二值图像
	czh_imwrite(srcImage, "binary");	// 输出二值图像

	// 时钟信息，记录程序算法使用时间
	double timeTotal = static_cast<double>(getTickCount());
	double time = static_cast<double>(getTickCount());


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//																																				  //
//															开始算法运算																		  //
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
// 				// 将源图二值图像的值赋给测试二值图像（三通道）
// 				t_Image_bw.at<Vec3b>(i, j)[0] = srcImage.at<uchar>(i, j);
// 				t_Image_bw.at<Vec3b>(i, j)[1] = srcImage.at<uchar>(i, j);
// 				t_Image_bw.at<Vec3b>(i, j)[2] = srcImage.at<uchar>(i, j);
// 				// 将源图灰度图像的值赋给测试灰度图像（三通道）
// 				t_Image_or.at<Vec3b>(i, j)[0] = src.at<uchar>(i, j);
// 				t_Image_or.at<Vec3b>(i, j)[1] = src.at<uchar>(i, j);
// 				t_Image_or.at<Vec3b>(i, j)[2] = src.at<uchar>(i, j);
// 			}
// 		}
// 		vector<Point> pointsTest;	// 用于存放以像素点为圆心的圆像素的矢量：迭代坐标
// 		vector<int> t_value;
// 		vector<int> t_regionPointNum;
// 		czh_BresenhamCircle(203, 199, radius, pointsTest);	// 这个是以初始点为圆心所求得的圆坐标：元圆坐标
// 	
// 		for (int it = 0; it < pointsTest.size(); it++)
// 		{
// 			
// 			// 在测试二值图像上画圆
// 			t_Image_bw.at<Vec3b>(pointsTest[it])[2] = 255;
// 			t_Image_bw.at<Vec3b>(pointsTest[it])[1] = 0;
// 			t_Image_bw.at<Vec3b>(pointsTest[it])[0] = 0;
// 			// 在测试灰度图像上画圆
// 			t_Image_or.at<Vec3b>(pointsTest[it].y, pointsTest[it].x)[2] = 255;
// 			t_Image_or.at<Vec3b>(pointsTest[it].y, pointsTest[it].x)[1] = 0;
// 			t_Image_or.at<Vec3b>(pointsTest[it].y, pointsTest[it].x)[0] = 0;
// 
// 			cout << "point #" << it + 1 << " (" << pointsTest[it].x << " , " << pointsTest[it].y << ") : "
// 				<< (int)srcImage.at<uchar>(pointsTest[it]) << endl;
// 
// 			t_value.push_back((int)srcImage.at<uchar>(pointsTest[it]));
// 		}
// 		t_value.push_back((int)srcImage.at<uchar>(pointsTest[0]));	// 额外添加一个元素，它与第一个像素的灰度相同
// 		
// 		int dif = 0;	// 切换次数初始化为0
// 		t_regionPointNum.clear();
// 		t_regionPointNum.push_back(1);// 初始化第一个连续区域像素值个数为1，因为下边的循环直接从第二个点开始
// 	
// 		for (int i = 1; i < t_value.size(); i++)
// 		{	// 遍历圆像素灰度值矢量中每一个点
// 	
// 			// 如果当前点和前一点灰度差为255，则认为是转换了区域
// 			if (abs(t_value[i] - t_value[i - 1]) == 255)
// 			{
// 				dif++;	// 切换次数递增
// 				t_regionPointNum.push_back(0);	// 新增连续区域
// 			}
// 			// 第 dif 个区域像素点数递增
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
	//			 寻找最有可能是角点的像素			 //
	//												 //
	//...............................................//

	Mat pointsImage(srcImage.size(), CV_8UC1, Scalar::all(0));
	
	// 求得以改点为圆心，radius为半径的圆上的像素点
	// int radius = 9;	// 画圆算法半径
	vector<Point> pointsOfCircleOriginal;	// 用于存放以像素点为圆心的圆像素的矢量：元坐标
	vector<Point> pointsOfCircle;	// 用于存放以像素点为圆心的圆像素的矢量：迭代坐标
	vector<Point> pointsOfCircleOriginal_2;	// 用于存放以像素点为圆心的圆像素的矢量：元坐标
	vector<Point> pointsOfCircle_2;	// 用于存放以像素点为圆心的圆像素的矢量：迭代坐标
	czh_BresenhamCircle(radius, radius, radius, pointsOfCircleOriginal);	// 这个是以初始点为圆心所求得的圆坐标：元圆坐标
	czh_BresenhamCircle(radius, radius, radius, pointsOfCircle);	// 这个是以初始点为圆心所求得的圆坐标：当前圆坐标
	czh_BresenhamCircle(radius, radius, radius - 4, pointsOfCircleOriginal_2);
	czh_BresenhamCircle(radius, radius, radius - 4, pointsOfCircle_2);
	vector<int> valuesOfPoints;
	vector<int> regionPointNum;	// 记录每一个连续区域里的像素值个数

	for (int i = radius; i < srcImage.rows - radius; i++)
	{	// 对所有像素扫描，寻找以像素为圆心，radius为半径，圆上黑白像素相等的列像素：通过计算当前坐标和初始坐标的位移量来计算当前圆坐标
		// 记录圆上像素灰度值黑白切换次数，只有满足4次切换的，说明该店在交点附近
		// 同时如果它的圆上的白色像素等于黑色像素，说明这就是交点
		for (int j = radius; j < srcImage.cols - radius; j++)
		{
			// 求得以该点为圆心，radius为半径的圆上的像素点
			int offset_x = j - radius;	// x 位移量
			int offset_y = i - radius;	// y 位移量

			valuesOfPoints.clear();	// 清空圆像素灰度值矢量
			for (int p = 0; p < pointsOfCircle.size(); p++)
			{	// 通过计算当前坐标和初始坐标的位移量来计算当前圆上点像素坐标
				pointsOfCircle[p].x = pointsOfCircleOriginal[p].x + offset_x;
				pointsOfCircle[p].y = pointsOfCircleOriginal[p].y + offset_y;
				// 将圆上点像素的灰度值存在 valuesOfPoints 矢量之中
				valuesOfPoints.push_back((int)srcImage.at<uchar>(pointsOfCircle[p]));
			}
			valuesOfPoints.push_back((int)srcImage.at<uchar>(pointsOfCircle[0]));	// 额外添加一个元素，它与第一个像素的灰度相同

			int dif = 0;	// 切换次数初始化为0
			regionPointNum.clear();
			regionPointNum.push_back(1);// 初始化第一个连续区域像素值个数为1，因为下边的循环直接从第二个点开始

			for (int i = 1; i < valuesOfPoints.size(); i++)
			{	// 遍历圆像素灰度值矢量中每一个点

				// 如果当前点和前一点灰度差为255，则认为是转换了区域
				if (abs(valuesOfPoints[i] - valuesOfPoints[i - 1]) == 255)
				{
					dif++;	// 切换次数递增
					regionPointNum.push_back(0);	// 新增连续区域
				}

				// 第 dif 个区域像素点数递增
				regionPointNum[dif]++;
			}

			if (dif == 4)	// 如果切换次数为4次，说明该点在内部交点附近
			{
				regionPointNum[4] -= 1;	// 剪掉最开始额外添加的那一个元素，这个元素与第一个像素灰度值相同，它重复算了一次
				
				if ( abs( regionPointNum[0] + regionPointNum[2] + regionPointNum[4] - regionPointNum[1] - regionPointNum[3] ) < 9 )
				{	// 如果该点圆上黑白像素相等，那么它离交点就非常近
					if (abs(regionPointNum[1] - regionPointNum[3]) < 9)
					{
						if( (abs(regionPointNum[0] + regionPointNum[4] - regionPointNum[2]) < 9) )
						{
							pointsImage.at<uchar>(i, j) = 255;
						}
					}
				}
			}

			if (dif == 2)	// 如果切换次数为两次，则看它黑白比例是不是接近1:3
			{
				float black, white;
				regionPointNum[2] -= 1;	// 剪掉最开始额外添加的那一个元素，这个元素与第一个像素灰度值相同，它重复算了一次
				if (valuesOfPoints[0] == 0)	// 如果该点第一个圆像素是黑色
				{
					black = regionPointNum[0] + regionPointNum[2];
					white = regionPointNum[1];
				}
				else // 如果该点第一个圆像素是白色
				{
					black = regionPointNum[1]; 
					white = regionPointNum[0] + regionPointNum[2];
				};

				if ((white / black > 2.9) && (white / black < 3.1))	// 如果白黑比例接近3，则认为可能是黑色角点，则减小半径重新检测圆像素
				{
					valuesOfPoints.clear();	// 清空圆像素灰度值矢量
					for (int p = 0; p < pointsOfCircle_2.size(); p++)
					{	// 通过计算当前坐标和初始坐标的位移量来计算当前圆上点像素坐标
						pointsOfCircle_2[p].x = pointsOfCircleOriginal_2[p].x + offset_x;
						pointsOfCircle_2[p].y = pointsOfCircleOriginal_2[p].y + offset_y;
						// 将圆上点像素的灰度值存在 valuesOfPoints 矢量之中
						valuesOfPoints.push_back((int)srcImage.at<uchar>(pointsOfCircle_2[p]));
					}
					valuesOfPoints.push_back((int)srcImage.at<uchar>(pointsOfCircle_2[0]));	// 额外添加一个元素，它与第一个像素的灰度相同

					int dif = 0;	// 切换次数初始化为0
					regionPointNum.clear();
					regionPointNum.push_back(1);// 初始化第一个连续区域像素值个数为1，因为下边的循环直接从第二个点开始

					for (int i = 1; i < valuesOfPoints.size(); i++)
					{	// 遍历圆像素灰度值矢量中每一个点
						// 如果当前点和前一点灰度差为255，则认为是转换了区域
						if (abs(valuesOfPoints[i] - valuesOfPoints[i - 1]) == 255)
						{
							dif++;	// 切换次数递增
							regionPointNum.push_back(0);	// 新增连续区域
						}
						// 第 dif 个区域像素点数递增
						regionPointNum[dif]++;
					}

					if (dif == 2)	// 如果转换次数为2次，则可能是黑色角点
					{
						regionPointNum[2] -= 1;	// 剪掉最开始额外添加的那一个元素，这个元素与第一个像素灰度值相同，它重复算了一次
						if (valuesOfPoints[0] == 0)	// 如果该点第一个圆像素是黑色
						{
							black = regionPointNum[0] + regionPointNum[2];
							white = regionPointNum[1];
						}
						else // 如果该点第一个圆像素是白色
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
	
	// 做膨胀操作，连接断点
	Mat dilatedImage;
	dilate(pointsImage, dilatedImage, getStructuringElement(MORPH_RECT, Size(3, 3), Point(-1, -1)), Point(-1, -1), 3);

	time = ((double)getTickCount() - time) / getTickFrequency();
	cout << "\nProgram Step #1 检测角点，膨胀 结束 :\t" << time << " second used.\n" << endl;
	imshow("Step # 1 原图", pointsImage);	// 显示第一步效果图
	imshow("Step # 1 膨胀", dilatedImage);	// 显示第一步膨胀之后效果图


	//...............................................//
	//												 //
	//				     STEP.2                      //
	//				做连通域标记操作				 //
	//												 //
	//...............................................//

	time = static_cast<double>(getTickCount());	

	Mat labelledImage;
	czh_labeling(dilatedImage, labelledImage);	// 调用函数，做连通域标记操作

	time = ((double)getTickCount() - time) / getTickFrequency();
	cout << "Program Step #2 连通域标记 结束 :\t" << time << " second used.\n" << endl;
	imshow("Step # 2 Label", labelledImage);	// 显示第二步效果图

	//...............................................//
	//												 //
	//				     STEP.3                      //
	//					寻找重心					 //
	//												 //
	//...............................................//

	time = static_cast<double>(getTickCount());

	Mat dstImage(srcImage.size(), CV_8UC1, Scalar::all(0));
	// 维持一个大小为256的矢量，每一个矢量元素（子矢量）是一个点矢量，用于存放像素灰度值相同的点
	vector<vector<Point>> pointsSet(256);	// 比如像素灰度为 1 的点都存放于 pointSet[1] 之中
	
	Point curPoint;	// 当前点Point对象
	int curValue;	// 当前点灰度值
	MatIterator_<uchar> itBegin, itEnd;	// 图像迭代器

	for (itBegin = labelledImage.begin<uchar>(), itEnd = labelledImage.end<uchar>(); itBegin != itEnd; itBegin++)
	{	// 遍历标记图像中每一个像素，将相同像素的点放进一个矢量
		if ((*itBegin) != 0)	// 如果当前点灰度值不为零
		{
			curValue = (*itBegin);	// 当前点灰度值
			curPoint = itBegin.pos();	// 当前点Point对象
			pointsSet[curValue].push_back(curPoint);	// 把当前点压入对应矢量中
		}
	}

	vector<Point> cornerCenterPointSet;	// 准备最终的交点点矢量

	for (int i = 0; i < pointsSet.size(); i++)
	{	// 遍历 pointsSet 的所有256个子矢量

		if (pointsSet[i].size() == 0)
		{	// 如果某个子矢量中一个点都没有，则跳过
			continue;
		}

		// 初始化 x,y 值总和
		int total_x = 0;
		int total_y = 0;;	
		int total_points = pointsSet[i].size();// 该灰度值有多少个点

		for (int j = 0; j < pointsSet[i].size(); j++)
		{	// 遍历每一个子矢量中的点元素,求得x总值和y总值
			total_x += pointsSet[i][j].x;
			total_y += pointsSet[i][j].y;
		}

		// 求得该灰度值点群的重心
		int x = total_x / total_points;
		int y = total_y / total_points;
		cornerCenterPointSet.push_back(Point(x, y));	// 把该重心压入 cornerPointSet 矢量中,以备以后使用
		dstImage.at<uchar>(y, x) = 255;	// 把最终图像 Mat 对象中把角点置为白色
	}

	time = ((double)getTickCount() - time) / getTickFrequency();
	cout << "Program Step #3 寻找重心 结束 :\t" << time << " second used.\n" << endl;
	imshow("Step # 3 Final Result", dstImage);	// 显示第三步效果图


	//...............................................//
	//												 //
	//				     STEP.4                      //
	//				   输出点坐标					 //
	//												 //
	//...............................................//
	
	// 把角点坐标输出为 txt 文件
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

	// 程序结束信息：报告程序所花时间
	timeTotal = ((double)getTickCount() - timeTotal) / getTickFrequency();
	cout << "Program finished :\t" << timeTotal << " second used.\n" << endl;

	// 测试区域
// 	imshow("srcImage", srcImage);
//	imshow("dstImage", dstImage);
// 	czh_imwrite(pointsImage, "points");
// 	czh_imwrite(dilatedImage, "dilated");
// 	czh_imwrite(labelledImage, "label");
//  	czh_imwrite(dstImage, "corner");

	waitKey();
	system("pause");
}


