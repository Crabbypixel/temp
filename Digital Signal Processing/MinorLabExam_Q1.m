x = [1,2,1,2];
h = [1,1,1];
y = conv(x, h);

figure;
subplot(3,1,1);
stem(x);
xlabel("n");
ylabel("x[n]");
title("input sequence");

subplot(3,1,2);
stem(h);
xlabel("n");
ylabel("h[n]");
title("impulse sequence");

subplot(3,1,3);
stem(y);
xlabel("n");
ylabel("y[n]");
title("convoluted sequence");
disp("The convoluted sequence is ");
disp(y);