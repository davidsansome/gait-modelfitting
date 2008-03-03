set title "Parameters over time"
set xlabel "Time"
set ylabel "Value"
#unset key

# Plot to an X11 window
set term x11 persist title "Params over time - graph"
plot '__DATA_FILENAME__' using 1:2 title "Left thigh theta"

exit
