% Unit Step Sequence
N = 7;
n = 0:1:N-1;
y = ones(1,N);
figure; subplot(3,2,1);
stem(n,y);
xlabel('time');
ylabel('amplitude');
title('unit step sequence');

% Unit Ramp Sequence
N1 = 5;
n1 = 0:1:N1-1;
y1 = n1;
subplot(3,2,2);
stem(n1, y1);
xlabel('time');
ylabel('amplitude');
title('unit ramp sequence');

% Sinusoidal sequence
N2 = 6;
n2 = 0:0.1:N2-1;
y2 = sin(2*pi*n2);
subplot(3,2,3);
stem(n2, y2);
xlabel('time');
ylabel('amplitude');
title('sinusoidal sequence');

% Cosine Sequence
N3 = 4;
n3 = 0:0.1:N3-1;
y3 = cos(2*pi*n3);
subplot(3,2,4);
stem(n3,y3);
xlabel('time');
ylabel('amplitude');
title('cosine sequence');

% Exponential sequence
N4 = 5;
n4 = 0:0.1:N4-1;
a = 3;
y4 = exp(a*n4);
subplot(3,2,5);
stem(n4, y4);
xlabel('time');
ylabel('amplitude');
title('exponential sequence');

% Unit Impulse Sequence
N5 = 6;
n5 = -3:1:3;
y5 = [zeros(1,3), ones(1,1), zeros(1,3)];
subplot(3,2,6);
stem(n5,y5);
xlabel('time');
ylabel('amplitude');
title('unit impulse');