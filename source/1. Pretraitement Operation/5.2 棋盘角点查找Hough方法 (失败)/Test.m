clc;clear;
% I=imread('image.ppm');
% co=importdata('result image.txt',',');

I=imread('image tilt 1.ppm');
co=importdata('result image tilt 1.txt',',');

% ��Ϊһ��ͼ������ӣ�0,0����ʼ�� Matlab �ӣ�1,1����ʼ��������Ҫ+1
for i=1:size(co,1)
    co(i,1)=co(i,1)+1;
    co(i,2)=co(i,2)+1;
end
hold on;
% ��ʾԴͼ��ͽǵ�
imshow(I);
hold on;
for i=1:size(co,1)
    plot(co(i,1),co(i,2),'r+');
end
