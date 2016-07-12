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
void czh_thin_parallel(Mat & srcImage, Mat & dstImage, int iterations = 10);	// �ú���ʹ��zhang�Ĳ����㷨����һ����ֵͼ����߻Ҷ�ͼ�������ͼ��Ĺ���
void czh_thin_LUT(Mat & srcImage, Mat & dstImage, int iterations = 10);	// �ú���ʹ�ò������һ����ֵͼ����߻Ҷ�ͼ�������ͼ��Ĺ���
void czh_extractColor(Mat &srcImage, Mat & dstImage, int color = RED);	// �ú�������һ����ͨ��RGBͼ�񣬳�ȡ��ͼ���е� color ��ɫ�����أ�����Ϊ��ɫ����
void czh_minFilter(Mat &srcImage, Mat & dstImage, int size);	// �ú�������һ����ͨ��ͼ������Сֵ�˲������ڴ�С size * size

void czh_helpInformation(string const &functionInfo);	// ������Ϣ��������
void czh_imageOpenDetect(Mat & srcImage, string & fileName, string & fileType);// ���ͼ������Ƿ��Ѿ��ɹ���

void czh_middlePointCircle(int x0, int y0, int radius, vector<Point> &pointsOfCircle); // �ú��������Ե� (x0, y0) ΪԲ�ģ� radiusΪ�뾶��Բ����Բ�ϵĵ�����굼�� vector<Point>ʸ�� pointsOfCircle ֮��
void czh_BresenhamCircle(int x0, int y0, int radius, vector<Point> &pointsOfCircle);	// Bresenham �Ż���Բ�㷨
void czh_Circle_Profile(const Mat & srcImage, const int x0, const int y0, const int radius); // չʾ��ѡ����ΪԲ�ģ�radiusΪ�뾶��Բ�ĻҶȱ仯
void czh_imwrite(Mat &dstImage, const string imageName);// �ú����Զ��ж����ͼ��Mat��������ͣ��Ծ������ͼ���ǻҶ�ͼ��(pgm)��������ͨ��ͼ��(ppm)
void czh_labeling(Mat & src, Mat & dst);	// �ú�����ʹ�� two - pass �㷨��Զ�ֵͼ������ͨ����
void czh_labeling_backup(Mat & src, Mat & dst);	// �ú�����ʹ�� two - pass �㷨��Զ�ֵͼ������ͨ����,���Ϻ������ݰ�
#endif
