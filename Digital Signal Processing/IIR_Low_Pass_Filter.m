% Program for Butterworth Low Pass Filter

clc;
clear all;
close all;

w1 = input('Enter the Passband frequency w1 = ');
w2 = input('Enter the Stopband frequency w2 = ');
a1 = input('Enter the Passband ripple a1 = ');
a2 = input('Enter the Stopband ripple a2 = ');

[N, Wc] = butt_impord(w1, w2, a1, a2);

disp('LPF ANALOG FILTER TRANSFER FUNCTION');

[num, den] = butter(N, Wc, 's');

disp('LPF DIGITAL FILTER TRANSFER FUNCTION');

[b, a] = impinvar(num, den);

[mag, angle] = freqz(b, a, 512);
magnitude = 20*log10(abs(mag));

figure(1);
plot(angle/pi, magnitude);
grid on;
title('Low Pass Butterworth Filter');
xlabel('frequency --->');
ylabel('magnitude --->');
