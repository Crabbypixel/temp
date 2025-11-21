% Program: Circular Convolution of two sequences
clc;
clear all;
close all;

% Input sequences
x = input('Enter the first sequence x(n): ');
h = input('Enter the second sequence h(n): ');

% Length of sequences
N1 = length(x);
N2 = length(h);

% Make lengths equal by zero-padding
N = max(N1, N2);
x = [x, zeros(1, N-N1)];
h = [h, zeros(1, N-N2)];

% Circular convolution using formula
y = zeros(1, N);
for n = 0:N-1
    for k = 0:N-1
        y(n+1) = y(n+1) + x(k+1) * h(mod(n-k, N) + 1);
    end
end

% Display result
disp('Circular Convolution result y(n):');
disp(y);

% Plot sequences
subplot(3,1,1);
stem(0:N-1, x, 'filled');
title('Input Sequence x(n)');
xlabel('n'); ylabel('x(n)');

subplot(3,1,2);
stem(0:N-1, h, 'filled');
title('Input Sequence h(n)');
xlabel('n'); ylabel('h(n)');

subplot(3,1,3);
stem(0:N-1, y, 'filled');
title('Circular Convolution y(n)');
xlabel('n'); ylabel('y(n)');