clc;
clear all;
close all;

M = 11;
wc = 1.2;

n = 0:1:M-1;

hd_lp = ideal_lp(wc, M);
delta = [zeros(1, (M-1)/2) 1 zeros(1, (M-1)/2)];
hd = delta - hd_lp;

w = hamming(M);
h = hd .* w';

[H, W] = freqz(h, 1, 2000);
mag = 20*log10(abs(H));

subplot(2,2,1);
stem(n, hd);
title('Ideal Impulse Response');

subplot(2,2,2);
stem(n, w');
title('Hamming Window');

subplot(2,2,3);
stem(n, h);
title('Actual Impulse Response');

subplot(2,2,4);
plot(W/pi, mag);
grid on;
xlabel('Normalized Frequency');
ylabel('Magnitude (dB)');
title('Frequency Response');
