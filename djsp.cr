require "big"

TIME_FORMAT = "%Y/%m/%d %H:%M:%S"
def print_time(
  separator : String,
  *outputs : IO
) : Nil
  strtime = Time.local.to_s "%Y/%m/%d %H:%M:%S"

  outputs.each {|current| current << strtime << separator }
end

LOG_PATH   = "log.dat"
GLOBAL_LOG = File.open LOG_PATH, "a"
def print_to_log(
  where : IO,
  format : String,
  *printf_args
) : Nil
  print_time ": ", where, GLOBAL_LOG

  where     .printf format, *printf_args
  GLOBAL_LOG.printf format, *printf_args

  GLOBAL_LOG.flush
end

struct Int
  def isqrt : Int
    a = typeof(self).new(1) << ((self.bit_length + 1) >> 1)
    b = (a + self // a) >> 1

    while b < a
      a = b
      b = (a + self // a) >> 1
    end

    a
  end

  # def icbrt : Int
  #   a = typeof(self).new(1) << ((self.bit_length + 2) // 3)
  #   b = (2 * a + n // (a * a)) // 3
  #
  #   while b < a
  #     a = b
  #     b = (2 * a + n // (a * a)) // 3
  #   end
  #
  #   a
  # end
end

def juggler(
  seed : UInt64
) : UInt16
  count = 0u16
  value = BigInt.new(seed)

  until value == 1
    value **= 3 if value.odd?
    value = Math.isqrt value

    count += 1
  end

  count
end

hwm_index = 0u64
hwm_value = 0u16
hwm_count = 0u8

(2u64..).each do |n|
  count = juggler n

  if count > hwm_value
    hwm_value = count
    hwm_index = n
    hwm_count += 1

    print_to_log(
      STDOUT,
      "A(%d) = %d at %d ~10**%f\n",
      hwm_count,
      hwm_value,
      hwm_index,
      Math.log10(hwm_index)
    )
  end
end
