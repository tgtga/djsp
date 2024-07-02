# 1: 8
# 2: 512
# 3: 131127
# 4: 8589934592

def babylonian(n : Int)
  a = typeof(n).new(1) << ((n.bit_length >> 1) + 1)
  b = (a + n // a) >> 1

  while b < a
    a, b = b, (b + n // b) >> 1
  end

  a
end

def babylonian(n : Int, count : Int)
  a = n
  # a = typeof(n).new(1) << ((n.bit_length >> 1) + 1)

  count.times { a = (a + n // a) >> 1 }

  a * a > n ? a - 1 : a
end

(1u8..).each do |iterations|
  STDOUT << iterations << ": " << (2u64..).find do |n|
    babylonian(n) != babylonian(n, iterations)
  end << "\n"
end
