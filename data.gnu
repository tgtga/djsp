#           READING                      DEFAULT VALUES          INTEGER PARSING
TERM   = "`echo $data_term`"  ; if (TERM   eq "") TERM = "svg";
SIZE   = "`echo $data_size`"  ;                               ;
EXT    = "`echo $data_ext`"   ; if (EXT    eq "") EXT = ".svg";
BASE   = "`echo $data_base`"  ;                               ; BASE   = 0+BASE
N      = "`echo $data_n`"     ;                               ; N      = 0+N
LOGLOG = "`echo $data_loglog`"; if (LOGLOG eq "") LOGLOG = 0  ; LOGLOG = 0+LOGLOG



eval sprintf("set term %s %s", TERM, SIZE)
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
power_ceil(x) = BASE**ceil(bl(x))

set ytics log

stats $DATA nooutput
set xrange [-10:STATS_max_x + 10]

if (LOGLOG) {
  set yrange [10:power_ceil(STATS_max_y)]
  set nonlinear y via log(log(y) * IL) * IL inverse BASE**BASE**y
} else {
  set yrange [2:power_ceil(STATS_max_y)]
  set nonlinear y via log(y) * IL inverse BASE**y
}

plot $DATA w l