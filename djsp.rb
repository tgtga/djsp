class Integer
  def int_root base = 2
    return self if self <= 1

    r = (1 << ((self.bit_length + base - 1) / base)) - 1

    loop do
      break if self >= r ** base

      print "#{r} -> "
      r = ((base - 1) * r + self / (r ** (base - 1))) / base
      puts "#{r}"
    end

    r
  end
end

def power n, p
  chunk = 8

  l = []
  while n > 0
    l << (n & ((1 << chunk) - 1))
    n >>= chunk
  end

  p l

  l.map! { _1 ** p }

  p l

  o = 0
  l.each do |c|
    o += c
    o <<= chunk
  end

  o
end
