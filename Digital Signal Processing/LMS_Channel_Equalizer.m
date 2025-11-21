% **********************************************
%      LEAST MEAN SQUARE (LMS) EQUALIZER
% **********************************************

clear;
close all;

rng('shuffle');              % random seed

numpoints = 5000;            % number of samples
numtaps   = 10;              % number of equalizer coefficients
mu        = 0.01;            % step size

% Complex random input (transmitted signal)
x = randn(numpoints,1) + 1j*randn(numpoints,1);

% Channel / filter to be equalized
h = rand(numtaps,1);
h = h / max(h);              % normalize

% Desired signal (channel output)
d = filter(h, 1, x);

% Initialize equalizer weights and buffers
w = zeros(numtaps,1);        % equalizer coefficients
y = zeros(numpoints,1);      % equalizer output
e = zeros(numpoints,1);      % error signal

% LMS adaptation
for n = numtaps:numpoints
    in     = x(n:-1:n-numtaps+1);   % input vector
    y(n)   = w' * in;               % equalizer output
    e(n)   = d(n) - y(n);           % error
    w      = w + mu * conj(e(n)) * in;  % weight update
end

% Learning curve
figure;
semilogy(abs(e));
grid on;
title(['LMS Adaptation Learning Curve using \mu = ', num2str(mu)]);
xlabel('Iteration Number');
ylabel('Output estimation error |e(n)|');
