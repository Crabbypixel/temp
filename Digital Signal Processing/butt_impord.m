function [N, Wc] = butt_impord(w1, w2, a1, a2)

l = sqrt((1/a2^2)-1);
e = sqrt((1/a1^2)-1);

num = log10(l/e);
den = log10(w2/w1);

N = ceil(num/den);
Wc = w1/((e)^(1/N));

end
