clc;
clear;
close all;

%% ----- SIGNAL GENERATION -----
n = 0:0.00025:1;  
t = 0:0.001:1;
x = sin(2*pi*30*n) + sin(2*pi*60*n);       
y = sin(2*pi*30*t) + sin(2*pi*60*t);

%% ----- DECIMATION AND INTERPOLATION -----
M = 4;                                   % Decimation factor
L = 4;                                   % Interpolation factor

x_dec = decimate(x, M);                % Decimated signal
x_int = interp(y, L);                  % Interpolated signal

%% ----- PLOTTING -----
figure('Name', 'Decimation and Interpolation', 'NumberTitle', 'off');

% --- (1) Original Signal (Full)
subplot(2,2,1);
stem(x(1:120));
title('Original Signal');
xlabel('n →');
ylabel('Amplitude →');
grid on;

% --- (2) Original Signal (Zoomed In)
subplot(2,2,2);
stem(y(1:30));
title('Original Signal');
xlabel('n →');
ylabel('Amplitude →');
grid on;

% --- (3) Decimated Signal
subplot(2,2,3);
stem(x_dec(1:30));
title('Decimated Signal');
xlabel('n →');
ylabel('Amplitude →');
grid on;

% --- (4) Interpolated Signal
subplot(2,2,4);
stem(x_int(1:120));
title('Interpolated Signal');
xlabel('n →');
ylabel('Amplitude →');
grid on;

%% ----- DISPLAY INFO -----
% disp(['Original length = ', num2str(length(x))]);
% disp(['Decimated length = ', num2str(length(x_dec))]);
% disp(['Interpolated length = ', num2str(length(x_int))]);