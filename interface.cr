time_format = "%Y/%m/%d %H:%M:%S"
global_log : IO? = nil
def log(
  where : IO,
  format : String,
  *printf_args
) : Nil
  strtime = Time.local.to_s(time_format) + ": "

  where.print strtime
  where.printf format, *printf_args
  where.flush

  unless global_log.nil?
    global_log.print strtime
    global_log.printf format, *printf_args
    global_log.flush
  end
end

banner = "#{"=" * 10}%s#{"=" * 10}"

{% for type in Int::Unsigned.union_types %}
  struct {{type}}
    def isqrt : {{type}}
      a = {{type}}.new(1) << ((self.bit_length + 1) >> 1)
      b = (a + self // a) >> 1

      while b < a
        a = b
        b = (a + self // a) >> 1
      end

      a
    end
  end
{% end %}

p 4u8.isqrt, 5u8.isqrt, 8u8.isqrt, 9u8.isqrt
