#!/usr/bin/env ruby

require "open3"

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
  N = data.lazy.map { Integer(_1.match(/(\d+) step/)&.[] 1) rescue nil }.find { _1 } # pull out N from data
end

data = data.filter_map { _1.match(/.+ step (\d+) ~2\*\*(\d+)/)&.[](1..)&.map(&:to_i) }

so = data.map { _1.join " " }.join("\n") + "\n"
File.write "#{N}.dat", so

Open3.capture2(
  { "data_n" => N.to_s },
  *%W[gnuplot data.gnu],
  stdin_data: so
)
