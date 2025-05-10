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

jugglerhash(hash, which, seed, base = 2) = {
  my(
    n = seed, past = 0, count = -1,
    hs = List()
  );

  while(n != past,
    count += 1;
    past = n;
    n = js(n, base);

    if(n != 1, listput(hs, hash(n)));
  );

  return(if(which, hs, #hs - #Set(hs)));
};
