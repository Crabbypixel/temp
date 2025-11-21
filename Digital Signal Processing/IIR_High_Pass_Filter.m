% Program for Butterworth High Pass Filter

clc;
clear all;
close all;

w1 = input('Enter the Passband frequency w1 = ');
w2 = input('Enter the Stopband frequency w2 = ');
a1 = input('Enter the Passband ripple a1 = ');
a2 = input('Enter the Stopband ripple a2 = ');

[N1, Wc1] = butt_biord(w2, w1, a1, a2);

disp('HPF ANALOG FILTER TRANSFER FUNCTION');

[num1, den1] = butter(N1, Wc1, 'high', 's');

disp('HPF DIGITAL FILTER TRANSFER FUNCTION');

[b1, a1] = bilinear(num1, den1, 1);

[mag1, angle1] = freqz(b1, a1, 512);
magnitude1 = 20*log10(abs(mag1));

figure(2);
plot(angle1/pi, magnitude1);
grid on;
title('High Pass Butterworth Filter');
xlabel('frequency --->');
ylabel('magnitude --->');
