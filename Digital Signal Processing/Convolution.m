x = input('Enter the sequence 1: ');
h = input('Enter the sequence 2: ');

% Convolution
m = length(x);
n = length(h);
X = [x, zeros(1, n)];  % zero-padding
H = [h, zeros(1, m)];

Y = zeros(1, m+n-1);   % preallocate output

for i = 1:m+n-1
    for j = 1:m
        if (i-j+1 > 0) && (i-j+1 <= n+m)
            Y(i) = Y(i) + X(j) * H(i-j+1);
        end
    end
end

% Plot results
figure;
subplot(3,1,1); stem(x);
xlabel('n'); ylabel('x[n]'); grid on;

subplot(3,1,2); stem(h);
xlabel('n'); ylabel('h[n]'); grid on;

subplot(3,1,3); stem(Y);
xlabel('n'); ylabel('Y[n]'); grid on;
title('Convolution of Two Signals without conv function');