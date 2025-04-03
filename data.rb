#!/usr/bin/env ruby

require "open3"

TETRATION = eval ENV["tetration"] || "false"

if STDIN.tty? # no data waiting in stdin
  case ARGV.length
  when 0
    puts "lol kill -9 saved"
    exit 1
  when 1 then N = Integer ARGV.first
  else
    puts "only provide one argument, unless data is provided to stdin"
    exit 1
  end

  data = Open3.capture2(*%W[stdbuf -oL ./djsp.rb -s -- #{N}])
    .first # discard return status
    .lines
else # data in stdin
  data = STDIN.read.lines
  N = data.first.match(/(\d+) step/)[1].to_i # pull out N from data
end

data = data.filter_map {|line| line.match(/.+ step (\d+) ~2\*\*(\d+)/)&.[](1..)&.map(&:to_i) }

if TETRATION
  Open3.popen2(*%W[gp -q]) do |i, o, _|
    i.puts <<~HERE
      read("kneser.gp");
      quietmode = 1;
      init(3/2);
    HERE
    21.times { o.gets } # get rid of kneser.gp preamble

    data.map! do |a|
      w, v = a

      i.puts "print(slog(#{v}));"
      
      r = Float o.gets

      a = [w, r]
    end
  end
end

so = data.map {|a| a.join " " }.join("\n") + "\n"
File.write "#{N}.dat", so

driver =
  if TETRATION
    "set yrange [2:ceil(STATS_max_y)]"
  else
    <<~HERE
      set nonlinear y via log10(y) inverse 10**y
      set ytics log

      f(x) = 10**ceil(log10(x))
      set yrange [1:f(STATS_max_y)]
    HERE
  end


Open3.capture2 *%W[gnuplot], stdin_data: <<~HERE
  $DATA <<END
    #{so}
  END

  set term svg
  set output '#{N}.svg'

  set key noautotitle
  set key title '#{N}'

  stats $DATA nooutput

  #{driver}
  set xrange [-10:STATS_max_x + 10]

  plot $DATA w l
HERE
