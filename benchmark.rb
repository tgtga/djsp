require "benchmark"

TEST_CONSTANT = 1247677915
TEST_PROGRAMS = ["./djsp-rennonn", "./djsp-reyyoyy"]

accumulator = Array.new TEST_PROGRAMS.length, 0
runs = 0

loop do
  TEST_PROGRAMS.each_with_index do |executable_name, index|
    accumulator[index] += (Benchmark.measure do
      system executable_name, TEST_CONSTANT.to_s
    end.real)
  end

  runs += 1

  puts TEST_PROGRAMS.zip(accumulator).map {|(name, av)| "#{name}: #{av / runs}" }.join("\n") + "\n"
end
