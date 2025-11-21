function hd = ideal_lp(wc, M)
alpha = (M-1)/2;
n = 0:1:M-1;
N = n - alpha + eps;     % avoid divide by zero
hd = sin(wc*N)./(pi*N);  % ideal low pass impulse response
end
