require "open3"

gpin, gpout, gpwait = Open3.popen2 "gp -q"
gpin.puts "default(colors, no)"

imin, imout, imwait = Open3.popen2 "./intmath"

2.upto(128) do |base|
  passed = true

  1.upto(63) do |power|
    n = 2 ** power

    gpin.puts "sqrtnint(#{n}, #{base})"; k = gpout.gets.strip.to_i
    imin.puts "#{n} #{base}";            t = imout.gets.strip.to_i

    if k != t
      puts "#{n}^(1/#{base}): #{k} vs #{t}"
      passed = false
    end
  end

  puts "#{base} passed!" if passed
end

gpin.close
gpout.close
