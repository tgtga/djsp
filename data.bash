#!/usr/bin/env bash

if [ -z "$1" ]; then
  echo "lol kill -9 saved"
  exit 1
fi

stdbuf -oL ./djsp.rb -s -- $1 \
  | perl -lne 'print if /step/' \
  | perl -pe 's/.+ step (\d+) ~2\*\*(\d+)/$1 $2/' >>$1.dat;

{
  echo '$DATA <<END';
  cat $1.dat;
  echo 'END';

  cat <<HERE
set term svg
set output '$1.svg'

set key noautotitle
set key title '$1'

set nonlinear y via log10(y) inverse 10**y
set ytics log

f(x) = 10**ceil(log10(x))
stats \$DATA nooutput
set xrange [-10:STATS_max_x + 10]
set yrange [1:f(STATS_max_y)]

plot \$DATA w l
HERE
} | gnuplot
