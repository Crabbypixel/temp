x = input("Enter the Input Sequence: ");
y = xcorr(x, x);
figure;
subplot(2,1,1);
stem(x);
ylabel("amplitude");
xlabel("x(n)");
subplot(2,1,2);
stem(y);
ylabel("amplitude");
xlabel("y(n");
disp("The resultant Siganl is: ");

%Auto Correlation: Rxx[k] = Summation from n = -infinity to infinity x[n].x[n-k]
%Cross Correlation: Rxh[k] = Summation from n = -infinity to infinity x[n].h[n-k]