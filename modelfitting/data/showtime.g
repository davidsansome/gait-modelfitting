set title "Parameters over time"
set xlabel "Time"
set ylabel "Value"
#unset key

# Plot to an X11 window
set term x11 persist title "Params over time - graph"
plot __PLOTS__

exit
