use warnings;
use strict;
use Test::More;
# use Data::Dump;

my @tests = (
  [  2,     1,   0 ],
  [  2,    25,  11 ],
  [  2, 78901, 258 ],
  [  3,    13,   5 ],
  [  3,    29,   5 ],
  [  3,    32,   5 ],
  [  3,    44,   7 ],
  [  4,    19,   4 ],
  [  4,    23,   4 ],
  [  9,    44,   5 ],
  [ 10,    89,   5 ],
  [ 11,    32,   6 ]
);

# plan tests => @tests + grep { $_->[0] == 2 } @tests;

sub test {
  $_ = shift;
  `$_`;
  chomp;
  //;
}

for (@tests) {
  my ($base, $seed, $expected) = @$_;

  if ($base == 2) {
    # $_ = `./djsp $seed,$seed 2>/dev/null`; chomp;
    # is $_, "L($seed) = $expected", "L($seed)";
    is strip_backticks("./djsp $seed,$seed 2>/dev/null"), "L($seed) = $expected", "L($seed)";
  }
  
  $_ = `./djsp $seed,$seed $base 2>/dev/null`; chomp;
  is $_, "L_$base($seed) = $expected", "L_$base($seed)";
}

done_testing;
