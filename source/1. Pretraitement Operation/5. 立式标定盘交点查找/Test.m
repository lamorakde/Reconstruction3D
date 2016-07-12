clc;clear;

I=imread('01.ppm');

co01=importdata('coordinates 01.txt',',');
co02=importdata('coordinates 02.txt',',');
co03=importdata('coordinates 03.txt',',');
co04=importdata('coordinates 04.txt',',');
co05=importdata('coordinates 05.txt',',');
co06=importdata('coordinates 06.txt',',');
co07=importdata('coordinates 07.txt',',');
co08=importdata('coordinates 08.txt',',');
co09=importdata('coordinates 09.txt',',');
co10=importdata('coordinates 10.txt',',');

co11=importdata('coordinates 11.txt',',');
co12=importdata('coordinates 12.txt',',');
co13=importdata('coordinates 13.txt',',');
co14=importdata('coordinates 14.txt',',');
co15=importdata('coordinates 15.txt',',');
co16=importdata('coordinates 16.txt',',');
co17=importdata('coordinates 17.txt',',');
co18=importdata('coordinates 18.txt',',');
co19=importdata('coordinates 19.txt',',');
co20=importdata('coordinates 20.txt',',');

co21=importdata('coordinates 21.txt',',');
co22=importdata('coordinates 22.txt',',');
co23=importdata('coordinates 23.txt',',');
co24=importdata('coordinates 24.txt',',');
co25=importdata('coordinates 25.txt',',');
co26=importdata('coordinates 26.txt',',');
co27=importdata('coordinates 27.txt',',');
co28=importdata('coordinates 28.txt',',');
co29=importdata('coordinates 29.txt',',');
co30=importdata('coordinates 30.txt',',');

imshow(I);
hold on;
for i=1:size(co01,1)
    plot(co01(i,1)+1,co01(i,2)+1,'r+');
    plot(co02(i,1)+1,co02(i,2)+1,'r+');
    plot(co03(i,1)+1,co03(i,2)+1,'r+');
    plot(co04(i,1)+1,co04(i,2)+1,'r+');
    plot(co05(i,1)+1,co05(i,2)+1,'r+');
    plot(co06(i,1)+1,co06(i,2)+1,'r+');
    plot(co07(i,1)+1,co07(i,2)+1,'r+');
    plot(co08(i,1)+1,co08(i,2)+1,'r+');
    plot(co09(i,1)+1,co09(i,2)+1,'r+');
    plot(co10(i,1)+1,co10(i,2)+1,'r+');
    
    plot(co11(i,1)+1,co11(i,2)+1,'b+');
    plot(co12(i,1)+1,co12(i,2)+1,'b+');
    plot(co13(i,1)+1,co13(i,2)+1,'b+');
    plot(co14(i,1)+1,co14(i,2)+1,'b+');
    plot(co15(i,1)+1,co15(i,2)+1,'b+');
    plot(co16(i,1)+1,co16(i,2)+1,'b+');
    plot(co17(i,1)+1,co17(i,2)+1,'b+');
    plot(co18(i,1)+1,co18(i,2)+1,'b+');
    plot(co19(i,1)+1,co19(i,2)+1,'b+');
    plot(co20(i,1)+1,co20(i,2)+1,'b+');
    
    plot(co21(i,1)+1,co21(i,2)+1,'y+');
    plot(co22(i,1)+1,co22(i,2)+1,'y+');
    plot(co23(i,1)+1,co23(i,2)+1,'y+');
    plot(co24(i,1)+1,co24(i,2)+1,'y+');
    plot(co25(i,1)+1,co25(i,2)+1,'y+');
    plot(co26(i,1)+1,co26(i,2)+1,'y+');
    plot(co27(i,1)+1,co27(i,2)+1,'y+');
    plot(co28(i,1)+1,co28(i,2)+1,'y+');
    plot(co29(i,1)+1,co29(i,2)+1,'y+');
    plot(co30(i,1)+1,co30(i,2)+1,'y+');
end

