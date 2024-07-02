juggler(n) =                                           \
  local(v = n, count = 0);                             \
  until(v == 1,                                        \
    local(maxfac = vecmax(factor(v, 2^24)[, 1]));      \
    print(log(v) / log(2), " ", log(maxfac) / log(2)); \
    count += 1;                                        \
    v = if(v % 2 == 0, sqrtint(v), sqrtint(v^3))       \
  );                                                   \
  count
