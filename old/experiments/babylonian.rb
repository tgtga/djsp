# 1: 8
# 2: 512
# 3: 131127
# 4: 8589934592

def babylonian n, count
  a = 1 << ((n.bit_length >> 1) + 1)

  count.times { a = (a + n / a) >> 1 }

  a * a > n ? a - 1 : a
end

(1..).each do |iterations|
  print "#{iterations}: "

  bound = (1..).find {|n| Integer.sqrt(2 ** n) != babylonian(2 ** n, iterations) }
  found = (2 ** bound).downto(0).find {|n| Integer.sqrt(n) != babylonian(n, iterations) }

  puts found
end
