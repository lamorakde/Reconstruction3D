% �ó�������������궨���ҳ��궨����M��Ȼ�����ͨ��M��������ͼ�������������ʵ��������
% This program is used to find the calibration matrix M
clc;
clear;
format long e;

%% �궨����Ŀ
pointNumber=50;

%% ����Ŀ��ͼ��
% read the input image
im = imread('01.ppm');

%% ��������ʵ�����������²����õ��ĸ����ؼ��������
% import the key points' coordinate of the real space
xyz = importdata('xyz.txt',',');

%% ������ͼ���и�����Ӧ�����������

% import the key points' coordinate of the image
uv(:,:,1)  =    importdata('coordinates 01.txt',',');
uv(:,:,2)  =    importdata('coordinates 02.txt',',');
uv(:,:,3)  =    importdata('coordinates 03.txt',',');
uv(:,:,4)  =    importdata('coordinates 04.txt',',');
uv(:,:,5)  =    importdata('coordinates 05.txt',',');
uv(:,:,6)  =    importdata('coordinates 06.txt',',');
uv(:,:,7)  =    importdata('coordinates 07.txt',',');
uv(:,:,8)  =    importdata('coordinates 08.txt',',');
uv(:,:,9)  =    importdata('coordinates 09.txt',',');
uv(:,:,10) =    importdata('coordinates 10.txt',',');
uv(:,:,11)  =   importdata('coordinates 11.txt',',');
uv(:,:,12)  =   importdata('coordinates 12.txt',',');
uv(:,:,13)  =   importdata('coordinates 13.txt',',');
uv(:,:,14)  =   importdata('coordinates 14.txt',',');
uv(:,:,15)  =   importdata('coordinates 15.txt',',');
uv(:,:,16)  =   importdata('coordinates 16.txt',',');
uv(:,:,17)  =   importdata('coordinates 17.txt',',');
uv(:,:,18)  =   importdata('coordinates 18.txt',',');
uv(:,:,19)  =   importdata('coordinates 19.txt',',');
uv(:,:,20)  =   importdata('coordinates 20.txt',',');
uv(:,:,21)  =   importdata('coordinates 21.txt',',');
uv(:,:,22)  =   importdata('coordinates 22.txt',',');
uv(:,:,23)  =   importdata('coordinates 23.txt',',');
uv(:,:,24)  =   importdata('coordinates 24.txt',',');
uv(:,:,25)  =   importdata('coordinates 25.txt',',');
uv(:,:,26)  =   importdata('coordinates 26.txt',',');
uv(:,:,27)  =   importdata('coordinates 27.txt',',');
uv(:,:,28)  =   importdata('coordinates 28.txt',',');
uv(:,:,29)  =   importdata('coordinates 29.txt',',');
uv(:,:,30)  =   importdata('coordinates 30.txt',',');

%% ����궨����M
for i=1:30
    M_set(:,:,i) = zeros(3,4);
end;

for it = 1:30
    % ����һ����СΪ [���ؼ������*2��*11 ]  �ľ��󣬸���ֵ��0
    C = zeros(pointNumber*2,12);

    % �� C���� �������Ӧֵ
    for i = 1:pointNumber
        C(2*i-1,:) = [xyz(i,:) 1 0 0 0 0 -uv(i,1,it)*xyz(i,1) -uv(i,1,it)*xyz(i,2) -uv(i,1,it)*xyz(i,3) -uv(i,1,it)];
        C(2*i,:)   = [0 0 0 0 xyz(i,:) 1 -uv(i,2,it)*xyz(i,1) -uv(i,2,it)*xyz(i,2) -uv(i,2,it)*xyz(i,3) -uv(i,2,it)];
    end;

    % ���� Y = C'C
    Y = C'*C;

    % ����� Y���� �� valeur propre �� vecteur propre
    [eigenVectors, eigenValues] = eig(Y);

    % Ѱ����С���Ҳ�Ϊ��� valeur propre
    minIndex=1;
    for i=1:12
        eigVal = eigenValues(i,i);
        if (eigVal<=eigenValues(minIndex,minIndex) && not(eigVal==0))
            minIndex=i;
        end;
    end;

    % ���ǽ���С�� valeur propre ��Ӧ�� vecteur propre ��Ϊ������ҪѰ�ҵı궨���� M����
    M_temp = eigenVectors(:,minIndex);

    % �� M���� �ĳߴ��Ϊ 3*4
    M_temp=(reshape(M_temp,4,3))';
    M_set(:,:,it) = M_temp;
end

%% ��ʾ30��ͼ��õ���M�����ĳһ���ض��ľ���Ԫ�ر仯����
% x_plot = [1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30];
% y_plot = zeros(1,30);
% for i = 1:30
%     y_plot(i) = M(3,3,i);
% end
% plot(x_plot,y_plot,'+-');

%% ��30��ͼƬ�õ���M����ȡ��ֵ
M = zeros(3,4);

for i = 1:3
    for j = 1:4
        total = 0;
        for it = 1:30
            total = total + M_set(i,j,it);
        end
        M(i,j) = total /30;
    end 
end

%% ��ʾ���¼���õ������꣬�ͼ������
% ��ʾԭʼ���ݹؼ�������ͼ���õ��Ĺؼ�������
uv_distance=zeros(pointNumber,3);
imshow(im);
hold on;

for i=1:pointNumber 
    xyz_of_point = [xyz(i,1);xyz(i,2);xyz(i,3);1];
    
    % ����ǵ��ͼ�����꣬����һ��
    uv_calculated = M * xyz_of_point;
    uv_calculated = uv_calculated / uv_calculated(3,1);
    
    % �������
    uv_distance(i,:) = (uv_calculated)';
    uv_distance(i,1) = uv_distance(i,1)-uv(i,1);
    uv_distance(i,2) = uv_distance(i,2)-uv(i,2);
    
    % �������õ��ĵ��ͼ������
    plot(uv_calculated(1,1),uv_calculated(2,1),'ro');
    % �������ԭ�������ͼ�����ص�
    plot(uv(i,1),uv(i,2),'b+');
end