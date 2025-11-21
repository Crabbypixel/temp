clc; clear; close all;

x = input('Enter the Input Sequence: ');
N = length(x);
M = 4^ceil(log(length(x))/log(4));
if M ~= N
    x = [x, zeros(1, M - N)];
    fprintf('Input was zero-padded to length %d (next power of 4)\n', M);
end

n = 0:M-1;
rev = bitrevorder(n) + 1;
x = x(rev);

numStages = log(M) / log(4);
for s = 1:numStages
    m = 4^s;
    q = m / 4;
    Wm = exp(-1j * 2 * pi / m);
    for k = 1:m:M
        for j = 0:q-1
            a0 = x(k+j);
            a1 = Wm^(j)     * x(k+j+q);
            a2 = Wm^(2*j)   * x(k+j+2*q);
            a3 = Wm^(3*j)   * x(k+j+3*q);
            x(k+j)     = a0 + a1 + a2 + a3;
            x(k+j+q)   = a0 - 1j*a1 - a2 + 1j*a3;
            x(k+j+2*q) = a0 - a1 + a2 - a3;
            x(k+j+3*q) = a0 + 1j*a1 - a2 - 1j*a3;
        end
    end
end

X = x;
disp('FFT Output:');
disp(X);

figure;
subplot(2,1,1);
stem(0:M-1, abs(X));
title('Magnitude Spectrum');
xlabel('Frequency Index');
ylabel('|X(k)|');
subplot(2,1,2);
stem(0:M-1, angle(X));
title('Phase Spectrum');
xlabel('Frequency Index');
ylabel('∠X(k)');
