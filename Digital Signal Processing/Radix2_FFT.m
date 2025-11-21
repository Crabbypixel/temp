clc; clear; close all;

x = input("Enter the Input Sequence: ");
N = length(x);
rev = bitrevorder(0:N-1) + 1;
x = x(rev);

for s = 1:log2(N)
    m = 2^s;
    h = m/2;
    Wm = exp(-1j*2*pi/m);
    for k = 1:m:N
        for j = 0:h-1
            t = Wm^j * x(k+j+h);
            u = x(k+j);
            x(k+j) = u + t;
            x(k+j+h) = u - t;
        end
    end
end

X = x;
disp(X);

figure;
stem(0:N-1, abs(X));
title('Magnitude Spectrum');
xlabel('Frequency Index');
ylabel('|X(k)|');
