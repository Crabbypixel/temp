clc;
clear all;
close all;

M = 11;
wc1 = 1.2;
wc2 = 2.0;

n = 0:1:M-1;

hd = ideal_lp(wc2, M) - ideal_lp(wc1, M);

w = hann(M);
h = hd .* w';

[H, W] = freqz(h, 1, 2000);
mag = 20*log10(abs(H));

subplot(2,2,1);
stem(n, hd);
title('Ideal Impulse Response');

subplot(2,2,2);
stem(n, w');
title('Hanning Window');

subplot(2,2,3);
stem(n, h);
title('Actual Impulse Response');

subplot(2,2,4);
plot(W/pi, mag);
grid on;
xlabel('Normalized Frequency');
ylabel('Magnitude (dB)');
title('Frequency Response');
