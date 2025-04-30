js(n, base = 2) = {
  my(m = Mod(n, base));
  sqrtnint(n^(if(m == -1, base + 1, lift(m) + 1)), base);
};

juggler(seed, base = 2) = {
  my(n = seed, past = 0, count = -1);

  while(n != past,
    count += 1;
    past = n;
    n = js(n, base);
  );

  return(count);
};
