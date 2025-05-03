# set term pngcairo font 'Serif,16' size 800,600
# set output 'hwm.png'

set term postscript enh font 'Serif,16'
set out 'hwm.ps'

set yrange [-10:550]
set xrange [1:10**10]

IL = 1 / log(10)
set nonlinear x via log(x)*IL inverse 10**x
set xtics log
set format x "10^{%T}

set xlabel "{/:Italic J(n)} sets a new high water mark" font ",22"
set ylabel "step length" font ",22"

set key noautotitle

set label "my contribution" right at (4488817391 - 3.7*10**9),527
set arrow from (4488817391 - 3.5*10**9),527 to (4488817391 - 1.25*10**9),527

set style fill solid
set style circle radius graph 0.005
plot 'hwm.dat' w circles lc -1
