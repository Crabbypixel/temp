clc; clear; close all;

x = randn(1, 1000);
h = [1 2 3 2 1];

X = length(x);
M = length(h);

L = 128;
N = L + M - 1;

H = fft(h, N);

numBlocks = ceil(X/L);
x = [x zeros(1, numBlocks*L - X)];

y = zeros(1, numBlocks*L + M - 1);

for k = 0:numBlocks-1
    x_block = x(k*L + 1 : k*L + L);
    x_block = [x_block zeros(1, N-L)];
    X_block = fft(x_block, N);
    Y_block = X_block .* H;
    y_block = ifft(Y_block, N);
    y(k*L + 1 : k*L + N) = y(k*L + 1 : k*L + N) + real(y_block);
end

y_direct = conv(x, h);

disp('Output using Overlap-Add:');
disp(y);

disp('Direct Convolution (conv):');
disp(y_direct);

figure;
stem(y,'b','filled'); hold on;
stem(y_direct,'r--');
legend('Overlap-Add','Direct conv');
xlabel('n'); ylabel('y[n]');
title('Convolution using Overlap-Add Method');
grid on;