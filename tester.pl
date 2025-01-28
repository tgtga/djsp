use warnings;
use strict;

our $failed = 0; our $passed = 0;

sub pass { print "PASSED\n";                  ++$passed; }
sub fail { print "FAILED: got ", shift, "\n"; ++$failed; }

sub test2 {
  my ($seed, $expected) = @_;
  `./djsp $seed,$seed` =~ /L\(\d+\) = (\d+)/;
  print "L($seed) = $expected: ";
  ($expected == $1) ? pass : fail($1);
}

sub test {
  my ($seed, $base, $expected) = @_;
  `./djsp $seed,$seed $base` =~ /L_\d+\(\d+\) = (\d+)/;
  print "L_$base($seed) = $expected: ";
  ($expected == $1) ? pass : fail($1);
}

my @cases = (
  [     1,  2,   0 ],
  [    25,  2,  11 ],
  [ 78901,  2, 258 ],
  [    13,  3,   5 ],
  [    29,  3,   5 ],
  [    32,  3,   5 ],
  [    44,  3,   7 ],
  [    19,  4,   4 ],
  [    23,  4,   4 ],
  [    44,  9,   5 ],
  [    89, 10,   5 ],
  [    32, 11,   6 ]
);

for (@cases) {
  my ($seed, $base, $expected) = @$_;

  test2 $seed,        $expected if $base == 2;
  test  $seed, $base, $expected;
}

my $total = $failed + $passed;
printf "$passed passed and $failed failed out of $total\n";
