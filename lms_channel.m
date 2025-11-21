clear all;
close all;

randn('state', sum(100 * clock));   % Returns a matrix with pseudorandom values in normal distribution
rand('state', sum(100 * clock));    % Returns a matrix with pseudorandom values in uniform distribution

numpoints = 5000;                   % Sampling points
numtaps = 10;                       % No of filter coefficients
Mu = 0.01;

x = randn(numpoints,1) + i*randn(numpoints,1);   % Complex Random Input Signal
h = rand(numtaps,1);                                % filter transfer function
h = h / max(h);
d = filter(h, 1, x);                                 % digital filter with num coeff h and den coeff 1 and vector x

w = [];
y = [];
in = [];
e = [];

w = zeros(numtaps+1,1) + i*zeros(numtaps+1,1);       % initialize the w matrix

for n = numtaps+1 : numpoints
    in = x(n:-1:n-numtaps);
    y(n) = w' * in;
    e(n) = d(n) - y(n);                               % Calculates the error value
    w = w + Mu * ( real(e(n) * conj(in)) - i*imag(real(e(n) * conj(in))) );
end

figure(10);
semilogy(abs(e)); grid on;
title(['LMS Adaptation Learning Curve using Mu=', num2str(Mu)]);
xlabel('Iteration Number');
ylabel('Output estimation error in db');