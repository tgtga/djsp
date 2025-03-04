require "./djsp"
require "benchmark"

def range_test range
  hwm = hwm_index = 0

  range.each do |i|
    r = DJSP.oneshot i

    if r > hwm
      hwm = r
      hwm_index += 1
      # puts "A(#{hwm_index}) = #{hwm}"
    end
  end
end

def memo_test range, memo
  hwm = hwm_index = 0

  range.each do |i|
    r =
      if memo[i]
        memo[i]
      else
        memo[i] = DJSP.oneshot(i) do |_, steps, int, big|
          next 0 if !big.null?
          next if memo[int]
        end
      end

    if r > hwm
      hwm = r
      hwm_index += 1
      # puts "A(#{hwm_index}) = #{hwm}"
    end
  end
end

# require "stackprof"
# StackProf.run(mode: :cpu, out: "./stackprof-memo_test.dump") { memo_test (2..10**6) }

if $0 == __FILE__
  BOUND = 10**7

  puts "\e[2J\e[H"

  times = {}
  (2..BOUND).each do |i|
    time = Benchmark.measure { DJSP.oneshot i }.total
    if time > 0.5
      times[i] = time.round 4

      puts "\e[2J\e[H"
      sorted = times.sort_by(&:last).reverse
      len = sorted.map(&:first).max.to_s.length
      sorted.each do |(i, t)|
        puts "#{i.to_s.rjust len}: #{t}"
      end
    end
  end

  # Benchmark.bm do
  #   _1.report("normal:  ") { DJSP::C.sequence 2, 1, 10**7, 1, false }
  #   _1.report("rootopt: ") { DJSP::C.sequence_rootopt 1, 10**7, 1, false }
  # end

  # Benchmark.bmbm do
  #   _1.report("native c:        ") { DJSP::C.sequence 0, 1, 10**6, 1, false }
  #   _1.report("FFI:             ") { range_test (2..10**6) }
  #   _1.report("FFI memo, array: ") { memo_test (2..10**6), [] }
  #   _1.report("FFI memo, hash:  ") { memo_test (2..10**6), {} }
  # end
end
