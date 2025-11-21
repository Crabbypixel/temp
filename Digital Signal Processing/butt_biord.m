function [N1, Wc1] = butt_biord(w1, w2, a1, a2)

T = 1;
wp1 = (2/T)*tan(w1/2);
ws1 = (2/T)*tan(w2/2);

lam = sqrt((1/a2^2)-1);
ep = sqrt((1/a1^2)-1);

num1 = log10(lam/ep);
den1 = log10(ws1/wp1);

N1 = ceil(num1/den1);
Wc1 = wp1/((ep)^(1/N1));

end
