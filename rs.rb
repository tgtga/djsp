class Integer
  def gcd_ext other
    a = self
    b = other

    old_r, r = a, b
    old_s, s = 1, 0
    old_t, t = 0, 1

    while r > 0
      q = old_r / r

      old_r, r = r, old_r - q * r
      old_s, s = s, old_s - q * s
      old_t, t = t, old_t - q * t
    end

    [old_s, old_t]
  end
end

class PrimeList
  def initialize what
    case what
    when Integer
      @list = Prime.first what
    when Array
      @list = what
    else
      raise TypeError, "invalid type `#{what.class}`"
    end
  end
end

