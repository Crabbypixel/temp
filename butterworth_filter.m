clc;
clear;
close all;

%% -------------------------
%  USER INPUTS
%% -------------------------
Fs = input('Enter Sampling Frequency (Hz): ');
Fc = input('Enter Cutoff Frequency (Hz): ');
N  = input('Enter Filter Order: ');
filter_type = input('Enter Filter Type (low/high): ','s');

%% -------------------------
%  NORMALIZED CUTOFF
%% -------------------------
Wn = Fc / (Fs/2);    % Normalized cutoff (0–1)

%% -------------------------
%  DESIGN DIGITAL BUTTERWORTH FILTER
%% -------------------------
switch lower(filter_type)
    case 'low'
        [b, a] = butter(N, Wn, 'low');
        plot_title = 'Low Pass Butterworth Filter';
    case 'high'
        [b, a] = butter(N, Wn, 'high');
        plot_title = 'High Pass Butterworth Filter';
    otherwise
        error('Filter type must be "low" or "high".');
end

%% -------------------------
%  FREQUENCY RESPONSE
%% -------------------------
[H, w] = freqz(b, a, 2048);  % 2048-point better resolution
mag = 20*log10(abs(H));
freq = w / pi;               % Normalized freq (0 → 1)

%% -------------------------
%  PLOT (MATCHES YOUR MODEL PLOTS)
%% -------------------------
figure;
plot(freq, mag, 'b', 'LineWidth', 1.4);
grid on;

xlabel('frequency --->','FontSize',11);
ylabel('magnitude --->','FontSize',11);
title(plot_title,'FontSize',14,'FontWeight','bold');

% Adjust Y-axis for same visual effect as your images
ylim([-50 5]);          % Low-pass look
if strcmp(filter_type,'high')
    ylim([-200 5]);     % High-pass look
end
xlim([0 1]);
