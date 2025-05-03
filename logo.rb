D = [[2, 0], [0, 0], [1, 1], [2, 0], [2, 4]]
J = [[0, 0], [1, 1], [2, 0], [2, -2], [0, -2]]
DOT = [[1, 2], [1, 2]]
S = [[0, 0], [1, 1], [2, 0], [2, 1], [0, 1], [0, 2], [2, 2]]
P = [[0, 0], [1, 1], [2, 0], [0, 0], [0, -2]]

SCALE = 10
SPACING = 5/2r
HEIGHT = 70
WIDTH = 105

$xo = 1/2r
$yo = 2 + 1/2r
def fix_point x, y
  [x.+($xo).*(SCALE).to_i, HEIGHT - y.+($yo).*(SCALE).to_i]
end

puts "<svg width='#{WIDTH}' height='#{HEIGHT}' xmlns='http://www.w3.org/2000/svg'>"

[D, J, DOT, S, P].each do |a|
  print "<path fill='none' stroke='black' stroke-width='1' stroke-linecap='round' stroke-linejoin='round' d='"

  head, *tail = a
  print "M #{fix_point(*head).join ","} "

  tail.each {|x, y| print "L #{fix_point(x, y).join ","} " }

  puts "' />"

  $xo += SPACING if a != J
end

puts "<path fill='none' stroke='black' stroke-width='1' stroke-linecap='round' stroke-linejoin='round' d='' />"

puts "</svg>"
