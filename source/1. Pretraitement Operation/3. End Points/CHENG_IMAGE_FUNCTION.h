#ifndef CHENG_IMAGE_FUNCTION_H_
#define CHENG_IMAGE_FUNCTION_H_
#include "opencv2/opencv.hpp"
using namespace cv;

void CZH_inverseBinary(Mat & src_bw_Image, Mat & dst_bw_Image);
Mat CZH_skeleton(Mat src_bw_image);
void CZH_endPoint(Mat & src_skeleton_Image, Mat & dst_endPoint_Image);

#endif
