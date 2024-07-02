\\ upper(n, c) = prod(k = 0, c - 1, n - k);
\\ do(x, n, c) = 1 + sum(k = 1, c, upper(n, k) * x^k / k!);
do(x, n, c) = {
  my(o = 1, rn = n, rp = n, p = x, f = 1);
  for(k = 1, c,
    print("rp = ", rp, ", p = ", p, ", f = ", f, ", o = ", o);
    o += rp * p / f;
    rn -= 1; rp *= rn; p *= x; f *= k + 1;
  );
  o;
};
