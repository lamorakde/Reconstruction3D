clc;clear;
% I=imread('image.ppm');
% co=importdata('result image.txt',',');

I=imread('image tilt 1.ppm');
co=importdata('result image tilt 1.txt',',');

% 因为一般图像坐标从（0,0）开始而 Matlab 从（1,1）开始，所以需要+1
for i=1:size(co,1)
    co(i,1)=co(i,1)+1;
    co(i,2)=co(i,2)+1;
end
hold on;
% 显示源图像和角点
imshow(I);
hold on;
for i=1:size(co,1)
    plot(co(i,1),co(i,2),'r+');
end
