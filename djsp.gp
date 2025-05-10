js(n, base = 2) = {
  my(m = Mod(n, base));
  sqrtnint(n^(if(m == -1, base + 1, lift(m) + 1)), base);
};

juggler(seed, base = 2) = {
  my(n = seed, past = 0, count = -1);

  while(n != past,
    past = n; n = js(n, base);
    count += 1;
  );

  return(count);
};

juggler_hash(hash, which, seed, base = 2) = {
  my(
    n = seed, past = 0,
    hs = List()
  );

  while(n != past,
    listput(hs, hash(n));

    past = n; n = js(n, base);
  );

  return(if(which, hs, #hs - #Set(hs)));
};

juggler_hashcp(hash, which, seed, base = 2) = {
  my(
    n = seed, past = 0,
    hs = List()
  );

  while(n != past,
    listput(hs, hash(past, n));

    past = n; n = js(n, base);
  );

  return(if(which, hs, #hs - #Set(hs)));
};
