clc;
clear all;
close all;

M = 7;
wc1 = 1.2;
wc2 = 2.0;

n = 0:1:M-1;

hd_lp1 = ideal_lp(wc1, M);
hd_lp2 = ideal_lp(wc2, M);

delta = [zeros(1, (M-1)/2) 1 zeros(1, (M-1)/2)];
hd = hd_lp1 + (delta - hd_lp2);

w = rectwin(M);
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
