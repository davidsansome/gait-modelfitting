set title "FFT"
set xlabel "Frequency component"
set ylabel "Value"
#unset key

# Plot to an X11 window
set output '__OUT_FILENAME__'
set term __TERM_TYPE__
plot __PLOTS__

exit
