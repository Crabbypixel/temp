clc;
clear all;
close all;

%Program for Decimated Signal
t = 0:0.00025:1; %Time Vector
x = sin(2*pi*30*t)+sin(2*pi*60*t);
r = 4; %Factor of 4
y = decimate(x, r);

%View the Original Signal
figure(1);
subplot(2,1,1);
stem(x(1:120));
xlabel('n--->');
ylabel('Amplitude--->');
title('Original Signal');

%View the Decimated Signal
subplot(2,1,2);
stem(y(1:30));
xlabel('n--->');
ylabel('Amplitude--->');
title('Decimated Signal');

%Program for Interpolated Signal
t = 0:0.001:1; %Time Vector
x = sin(2*pi*30*t)+sin(2*pi*60*t);
r = 4; %Factor of 4
y = interp(x, r);

%View the Original Signal
figure(2);
subplot(2,1,1);
stem(x(1:30));
xlabel('n--->');
ylabel('Amplitude--->');
title('Original Signal');

%View the Interpolated Signal
subplot(2,1,2);
stem(y(1:120));
xlabel('n--->');
ylabel('Amplitude--->');
title('Interpolated Signal');