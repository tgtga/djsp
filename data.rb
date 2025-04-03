#!/usr/bin/env ruby

require "open3"

BASE   = eval ENV["base"]   || ENV["BASE"]   || "1.5"
LOGLOG = eval ENV["loglog"] || ENV["LOGLOG"] || "false"

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

so = data.map {|a| a.join " " }.join("\n") + "\n"
File.write "#{N}.dat", so

Open3.capture2 *%W[gnuplot], stdin_data: <<~HERE
  $DATA <<END
    #{so}
  END

  set term svg
  set output '#{N}.svg'

  set key noautotitle
  set key title '#{N}'

  stats $DATA nooutput

  set ytics log
  IL = 1/log(#{BASE})
  set nonlinear y via #{LOGLOG ? "log(log(y) * IL) * IL inverse #{BASE}**#{BASE}**y" : "log(y) * IL inverse #{BASE}**y"}

  f(x) = 10**ceil(log10(x))
  set yrange [#{LOGLOG ? 10 : 2}:f(STATS_max_y)]
  set xrange [-10:STATS_max_x + 10]

  plot $DATA w l
HERE
