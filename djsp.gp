juggler(seed, base) = {
  my(n = seed, past = 0, count = -1);

  while(n != past,
    count += 1;
    past = n;

    my(m = Mod(n, base)); print(n, " % ", base, " = ", lift(m));
    n = sqrtnint(n^if(m == -1, base + 1, lift(m) + 1), base);
  );

  return(count);
};
