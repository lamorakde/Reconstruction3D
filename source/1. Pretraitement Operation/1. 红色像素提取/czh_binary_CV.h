#ifndef CHENG_IMAGE_FUNCTION_H_
#define CHENG_IMAGE_FUNCTION_H_
#include "opencv2/opencv.hpp"
#include <vector>

using namespace std;
using namespace cv;
static enum { RED, GREEN, BLUE, WHITE, BLACK } color;

void czh_inverseBinary(Mat & src_bw_Image, Mat & dst_bw_Image);	// 使用 LUT 反转图像中黑白像素
void czh_endPoint(Mat & src_skeleton_Image, Mat & dst_endPoint_Image);	// 程序将会寻找该骨骼图像的端点
void czh_writeWhitePixel(Mat & srcImage, vector<Point2f> & dstWhitePoints);// 找出该图像中的白色像素并将像素坐标存于输入参数矢量
void czh_skeleton(Mat & srcImage, Mat & dstImage, int iterations = 10);	// 该函数输入一个二值图像或者灰度图像，输出该图像的骨骼
void czh_extractColor(Mat &srcImage, Mat & dstImage, int color = RED);	// 该函数输入一个三通道RGB图像，抽取该图像中的 color 颜色的像素，保存为白色像素

void czh_imwrite(Mat &dstImage, const string imageName);// 该函数自动判断输出图像Mat对象的类型，以决定输出图像是灰度图像(pgm)或者是三通道图像(ppm)
#endif
