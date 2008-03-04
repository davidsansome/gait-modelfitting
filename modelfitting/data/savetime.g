set title "Parameters over time"
set xlabel "Time"
set ylabel "Value"
#unset key

# Plot to an X11 window
set output '__OUT_FILENAME__'
set term __TERM_TYPE__
plot __PLOTS__

exit
