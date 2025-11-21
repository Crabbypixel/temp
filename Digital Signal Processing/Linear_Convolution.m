%Input Sequence
N = 5;
n = 0:1:N-1;
y = n;
figure;
subplot(3,1,1);
stem(n,y);
xlabel("time");
ylabel("amplitude");
title("Input Sequence");

%Impulse Sequence
N1 = 5;
n1 = 0:1:N-1;
y1 = n1 + 5;
subplot(3,1,2);
stem(n1,y1);
xlabel("time");
ylabel("amplitude");
title("Impulse Sequence");

%Linear Convolution
N2 = 9;
n2 = 0:1:N2-1;
y2 = conv(y, y1);
subplot(3,1,3);
stem(n2, y2);
xlabel("time");
ylabel("amplitude");
title("Linear Convolution");