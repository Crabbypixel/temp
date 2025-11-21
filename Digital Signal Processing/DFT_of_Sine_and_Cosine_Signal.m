% Parameters
Fs = 1000; %Sampling frequency (Hz)
T = 1/Fs; % sampling period (s)
L = 1000; % Length of Signal
t = (0:L-1)*T; % Time vector

% Frequencies
f1 = 50; % Frequency of sine wave (Hz)
f2 = 120; % Frequency of cosine wave (Hz)

% Signals
y_sin = sin(2*pi*f1*t); % Sine wave
y_cos = cos(2*pi*f2*t); % Cosine wave

% Fourier Transform
Y_sin = fft(y_sin);
Y_cos = fft(y_cos);

% Compute the two-sided spectrum and single-sided spectrum
P2_sin = abs(Y_sin/L);
P1_sin = P2_sin(1:L/2+1);
P1_sin(2:end-1) = 2*P1_sin(2:end-1);

P2_cos = abs(Y_cos/L);
P1_cos = P2_cos(1:L/2+1);
P1_cos(2:end-1) = 2*P1_cos(2:end-1);

% Frequency domain
f = Fs*(0:(L/2))/L;

% Plot Time-Domain Signal
figure;
subplot(2,1,1);
plot(t,y_sin);
title('Sine Wave (50 Hz)');
xlabel('Time (s)');
ylabel('Amplitude');

subplot(2,1,2);
plot(t,y_cos);
title('Cosine Wave (120 Hz)');
xlabel('Time (s)');
ylabel('Amplitude');

%Plot Frequency-Domain Manual DFT
figure;
subplot(2,1,1);
plot(f_axis, P1_sin);
title('Frequency Domain - Manual DFT Sine Wave');
xlabel('Frequency (Hz)');
ylabel('|X(f)|');

subplot(2,1,2);
plot(f_axis, P1_cos);
title('Frequency Domain - Manual DFT Cosine Wave');
xlabel('Frequency (Hz)');
ylabel('|X(f)|');