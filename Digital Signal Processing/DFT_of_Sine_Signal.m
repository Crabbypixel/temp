% Parameters
Fs = 1000; % Sampling Frequency (Hz)
T = 1/Fs; % Sampling Period
L = 1000; % Number of Samples (Keep small for manual DFT)
t = (0:L-1)*T; % Time Vector

f = 50; % Sine Frequency
A = 1; % Amplitude

% Generate sine wave
x = A * sin(2*pi*f*t);

% Manual DFT
X = zeros(1, L); % Preallocate output
for k = 0:L-1
    for n = 0:L-1
        X(k+1) = X(k+1) + x(n+1)*exp(-1j*2*pi*k*n/L);
    end
end

% Compute magnitude
P2 = abs(X)/L; % Two-sided
P1 = P2(1:L/2+1); % Single-sided
P1(2:end-1) = 2*P1(2:end-1); %Adjust for single-sided spectrum

%Frequency axis
f_axis = Fs*(0:(L/2))/L;

% Plot time-domain signal
figure;
plot(t, x);
title('Time Domain Signal');
xlabel('Time (s)');
ylabel('Amplitude');
grid on;

% Plot frequency-domain manual dft
figure;
plot(f_axis, P1);
title('Frequency Domain - Manual DFT');
xlabel('Frequency (Hz)');
ylabel('|X(f)|');
grid on;