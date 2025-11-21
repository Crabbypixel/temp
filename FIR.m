clc; 
clear; 
close all;

% ----- USER INPUT -----
Fs = input('Enter Sampling Frequency (Hz): ');
N = input('Enter Filter Order (N): ');
filter_type = input('Enter filter type (low/high/bandpass/bandstop): ','s');
window_type = input('Enter window type (hamming/hanning/blackman/rectwin): ','s');

% ----- Cutoff frequency input -----
if strcmpi(filter_type,'low') || strcmpi(filter_type,'high')
    Fc = input('Enter cutoff frequency (Hz): ');
elseif strcmpi(filter_type,'bandpass') || strcmpi(filter_type,'bandstop')
    Fc = input('Enter cutoff frequencies as [F1 F2] (Hz): ');
else
    error('Invalid filter type.');
end

% ----- NORMALIZATION -----
Wc = Fc / (Fs/2);

% ----- IDEAL IMPULSE RESPONSE -----
n = 0:N;
M = N/2;
k = n - M;

switch lower(filter_type)
    case 'low'
        hd = Wc * sinc(Wc * k);
    case 'high'
        hd = sinc(k) - Wc * sinc(Wc * k);
    case 'bandpass'
        hd = (Wc(2)*sinc(Wc(2)*k)) - (Wc(1)*sinc(Wc(1)*k));
    case 'bandstop'
        hd = sinc(k) - ((Wc(2)*sinc(Wc(2)*k)) - (Wc(1)*sinc(Wc(1)*k)));
    otherwise
        error('Invalid filter type.');
end

% ----- WINDOW FUNCTION -----
switch lower(window_type)
    case 'hamming',  w = hamming(N+1)';
    case 'hanning',  w = hanning(N+1)';
    case 'blackman', w = blackman(N+1)';
    case 'rectwin',  w = rectwin(N+1)';
    otherwise, error('Invalid window type');
end

% ----- FINAL FILTER COEFFICIENTS -----
h = hd .* w;

% ----- FREQUENCY RESPONSE -----
[Hf, f] = freqz(h, 1, 1024, Fs);    % Frequency in Hz (linear)
[Hn, wn] = freqz(h, 1, 1024);       % Normalized frequency (0 to π)
HdB = 20*log10(abs(Hn));            % Magnitude in dB

% ----- DISPLAY RESULTS -----
disp('--- RESULTS ---');
disp('Ideal Impulse Response (hd):'); disp(hd');
disp('Window Coefficients (w):'); disp(w');
disp('Filter Coefficients (h):'); disp(h');

% ----- PLOTS -----
figure;

% 1. Ideal Impulse Response
subplot(4,1,1);
stem(n, hd, 'filled');
title(['Ideal Impulse Response (', upper(filter_type), ')']);
xlabel('n'); ylabel('hd[n]'); grid on;

% 2. Windowed Impulse Response
subplot(4,1,2);
stem(n, h, 'filled');
title(['Windowed Impulse Response (', upper(window_type), ' Window)']);
xlabel('n'); ylabel('h[n]'); grid on;

% 3. Magnitude Response (Linear, in Hz)
subplot(4,1,3);
plot(f, abs(Hf), 'LineWidth', 1.5);
title(['Magnitude Response of ', upper(filter_type), ' FIR Filter (Linear)']);
xlabel('Frequency (Hz)'); ylabel('|H(f)|'); grid on;

% 4. Magnitude Response in dB vs Normalized Frequency
subplot(4,1,4);
plot(wn/pi, HdB, 'LineWidth', 1.5);
grid on;
xlabel('Normalized Frequency (\times \pi rad/sample)');
ylabel('Magnitude (dB)');
title(['Frequency Response of ', upper(filter_type), ' FIR Filter (dB)']);
axis([0 1 -100 5]); hold on;
plot([0 1],[0 0],'r--');  % zero dB reference line