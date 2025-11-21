x = [1,2,3,4];
h = [1,2,1,2];
N = 4;
y1 = conv(x, h);
y2 = cconv(x, h, N);

figure;
subplot(2,2,1);
stem(x);
xlabel("n");
ylabel("x[n]");
title("input sequence");

subplot(2,2,2);
stem(h);
xlabel("n");
ylabel("h[n]");
title("impulse sequence");

subplot(2,2,3);
stem(y1);
xlabel("n");
ylabel("y1[n]");
title("linearly - convoluted sequence");
disp("The linearly - convoluted sequence is ");
disp(y1);

subplot(2,2,4);
stem(y2);
xlabel("n");
ylabel("y2[n]");
title("circularly - convoluted sequence");
disp("The circularly - convoluted sequence is ");
disp(y2);