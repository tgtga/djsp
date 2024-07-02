def icbrt n
  return 0 if n == 0

  r0 = 1 << ((n.bit_length + 2) / 3); r1 = nil
  begin
    r1 = r0
    r0 = (2 * r1 + n / (r1 * r1)) / 3
  end while r0 < r1
  r1
end

=begin
def cube n
  c = 0 # 3n
  r = 0 # n

  (n.bit_length * 3).downto(0) do |i|
    c |= 1 << i
    # r = icbrt(c)
    r += icbrt(1 << i)

    puts "#{r}, #{icbrt(c)}"

    if r >= n
      c &= ~(1 << i)
      r -= icbrt(1 << i)
    end
  end

  c
end
=end

=begin
def ipow32 n
  root = Integer.sqrt n # 0.5b

  l = n * root # 1.5b
  root = nil

  r = l + n # 1.5b

  while r - l > 1
    m = (l + r) / 2 # 1.5b
    s = m * m       # 3b
    c = icbrt(s)    # 1b

    STDERR.puts "#{l} #{m} #{r}"

    c < n ? (l = m) : (r = m)
  end

  l
end
=end

=begin
def ipow32 n
  root = Integer.sqrt n
  l    = n * root
  root = nil

  d = n

  while d != 0
    m = l + d
    s = m * m
    c = icbrt s

    l += d if c < n
    d >>= 1
  end

  l
end
=end

=begin
def ipow32 n
  result = 0

  (n.bit_length * 3 / 2).downto(0) do |i|
    result |= (1 << i)
    square  = result ** 2

    result &= ~(1 << i) if icbrt(square) >= n
  end

  result += 1 if Integer.sqrt(n) ** 2 == n

  result
end
=end

=begin
def ipow32 n
  l = [n ** 2, (n + 1) * Integer.sqrt(n)]

  f = ->v{ icbrt(v * v) - n }

  loop do
    *_, m2, m1 = l

    begin
      l.push (m2 * f[m1] - m1 * f[m2]) / (f[m1] - f[m2])
    rescue ZeroDivisionError
      return l.last
    end

    p l
  end
end
=end

def ipow32 n
  r = (n * Integer.sqrt(n)) & ~((1 << n.bit_length) - 1)

  (n.bit_length + 1).downto(0) do |i|
    r |= 1 << i
    r &= ~(1 << i) if icbrt(r * r) >= n
  end

  r
end

if $0 == __FILE__
  (2..).each do |i|
    k = Integer.sqrt(i ** 3)
    t = ipow32 i

    if k != t
      puts "#{i}: #{k}, #{t}" "\n#{k.to_s(2)}\n#{t.to_s(2)}\n"
    end
  end
end

