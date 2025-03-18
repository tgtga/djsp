def next_seq n
  root = Rational(1 << (n.bit_length / 2 + 1))
  past = nil

  (1..).each do |prec|
    past, root = root, (root + n / root) / 2
    break if (n * past).floor == (n * root).floor
  end

  [(n * root).floor, root]
end

def size_of_seq n
  _, frac = next_seq n

  [frac.numerator.bit_length + frac.denominator.bit_length, (n ** 3).bit_length]
end

if $0 == __FILE__
  (2..).each {|n| puts n if next_seq(n).first != Integer.sqrt(n ** 3) }
end
