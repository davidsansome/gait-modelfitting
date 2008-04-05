set title "FFT"
set xlabel "Frequency component"
set ylabel "Value"
#unset key

# Plot to an X11 window
set term x11 persist title "FFT - graph"
plot __PLOTS__

exit
