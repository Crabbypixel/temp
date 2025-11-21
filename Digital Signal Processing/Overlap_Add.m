clc; clear; close all;

x = [1 2 3 4 5 6 7 8 9 10];
h = [1 2 3];

Nx = length(x);
Nh = length(h);

L = 4;
N = L + Nh - 1;

H = fft(h, N);

numBlocks = ceil(Nx/L);
x = [x zeros(1, numBlocks*L - Nx)];

y = zeros(1, numBlocks*L + Nh - 1);

for k = 0:numBlocks-1
    x_block = x(k*L + 1 : k*L + L);
    x_block = [x_block zeros(1, N-L)];
    X_block = fft(x_block, N);
    Y_block = X_block .* H;
    y_block = ifft(Y_block, N);
    y(k*L + 1 : k*L + N) = y(k*L + 1 : k*L + N) + real(y_block);
end

y_direct = conv([1 2 3 4 5 6 7 8 9 10], h);

disp('Output using Overlap-and-Add:');
disp(y);

disp('Direct Convolution (conv):');
disp(y_direct);

figure;
stem(y,'b','filled'); hold on;
stem(y_direct,'r--');
legend('Overlap-and-Add','Direct conv');
xlabel('n'); ylabel('y[n]');
title('Convolution using Overlap-and-Add Method');
grid on;