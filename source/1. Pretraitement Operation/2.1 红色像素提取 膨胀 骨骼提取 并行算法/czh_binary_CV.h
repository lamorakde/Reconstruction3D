#ifndef CHENG_IMAGE_FUNCTION_H_
#define CHENG_IMAGE_FUNCTION_H_
#include "opencv2/opencv.hpp"
#include <vector>

using namespace std;
using namespace cv;
static enum { RED, GREEN, BLUE, WHITE, BLACK } color;

void czh_inverseBinary(Mat & src_bw_Image, Mat & dst_bw_Image);	// ʹ�� LUT ��תͼ���кڰ�����
void czh_endPoint(Mat & src_skeleton_Image, Mat & dst_endPoint_Image);	// ���򽫻�Ѱ�Ҹù���ͼ��Ķ˵�
void czh_writeWhitePixel(Mat & srcImage, vector<Point2f> & dstWhitePoints);// �ҳ���ͼ���еİ�ɫ���ز���������������������ʸ��
void czh_skeleton(Mat & srcImage, Mat & dstImage, int iterations = 10);	// �ú�������һ����ֵͼ����߻Ҷ�ͼ�������ͼ��Ĺ���
void czh_extractColor(Mat &srcImage, Mat & dstImage, int color = RED);	// �ú�������һ����ͨ��RGBͼ�񣬳�ȡ��ͼ���е� color ��ɫ�����أ�����Ϊ��ɫ����

void czh_imwrite(Mat &dstImage, const string imageName);// �ú����Զ��ж����ͼ��Mat��������ͣ��Ծ������ͼ���ǻҶ�ͼ��(pgm)��������ͨ��ͼ��(ppm)
#endif
