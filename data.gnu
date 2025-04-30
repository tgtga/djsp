#           READING                      DEFAULT VALUES          INTEGER PARSING
TERM   = "`echo $data_term`"  ; if (TERM   eq "") TERM = "svg";
SIZE   = "`echo $data_size`"  ;                               ;
EXT    = "`echo $data_ext`"   ; if (EXT    eq "") EXT = ".svg";
BASE   = "`echo $data_base`"  ;                               ; BASE   = 0+BASE
N      = "`echo $data_n`"     ;                               ; N      = 0+N
MAX    = "`echo $data_max`"   ; if (MAX    eq "") MAX = 0     ; MAX    = 0+MAX
LOGLOG = "`echo $data_loglog`"; if (LOGLOG eq "") LOGLOG = 0  ; LOGLOG = 0+LOGLOG



CMD = sprintf("set term %s font 'Serif,16'", TERM)
if (!(SIZE eq "")) { CMD = sprintf("%s size %s", CMD, SIZE) }
eval CMD
set output "".N . EXT

set key noautotitle
set key title "".N



# evil string reading hacking
DATA = "`cat`"
set print $DATA # what the fuck?
print DATA
unset print



IL = 1/log(BASE)
bl(x) = log(x) * IL
BC = bl(2)
power_ceil(x) = BASE**ceil(bl(x))

set xlabel "number of steps" font ",22"
set ylabel "base-" . BASE . " logarithm of the step" font ",22"

stats $DATA nooutput
set xrange [-10:STATS_max_x + 10]

if (MAX == 0) MAX = power_ceil(STATS_max_y * BC)
print STATS_max_y, STATS_max_y * BC

if (LOGLOG) {
  set yrange [10:MAX]
  set nonlinear y via log(log(y) * IL) * IL inverse BASE**BASE**y
} else {
  set yrange [2:MAX]
  set nonlinear y via log(y) * IL inverse BASE**y
}
set ytics log

plot $DATA using 1:($2 * BC) w l lw 2 lt -1