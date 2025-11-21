clc;
clear all;
close all;

M = 7;
wc = 1.2;

n = 0:1:M-1;
hd = ideal_lp(wc, M);

w = hamming(M);
h = hd .* w';

[H, W] = freqz(h, 1, 2000);
mag = 20*log10(abs(H));

subplot(2,2,1);
stem(n, hd);
xlabel('n');
ylabel('hd(n)');
title('Ideal Impulse Response');

subplot(2,2,2);
stem(n, w');
xlabel('n');
ylabel('w(n)');
title('Hamming Window');

subplot(2,2,3);
stem(n, h);
xlabel('n');
ylabel('h(n)');
title('Actual Impulse Response');

subplot(2,2,4);
plot(W/pi, mag);
grid on;
xlabel('Normalized Frequency');
ylabel('Magnitude (dB)');
title('Frequency Response');
