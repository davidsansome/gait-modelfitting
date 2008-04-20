set term png
set xlabel "Time"


set output '81-leftthightheta.png'
plot 'set81.dat' using 1:3 lt 1 title "Auto", 'set81-manual.dat' using 1:3 lt 3 title "Manual"

set output '81-leftlowertheta.png'
plot 'set81.dat' using 1:5 lt 1 title "Auto", 'set81-manual.dat' using 1:5 lt 3 title "Manual"

set output '81-rightthightheta.png'
plot 'set81.dat' using 1:7 lt 1 title "Auto", 'set81-manual.dat' using 1:7 lt 3 title "Manual"

set output '81-rightlowertheta.png'
plot 'set81.dat' using 1:9 lt 1 title "Auto", 'set81-manual.dat' using 1:9 lt 3 title "Manual"



set output "96-leftthightheta.png"
plot 'set96.dat' using 1:3 lt 1 title "Auto", 'set96-manual.dat' using 1:3 lt 3 title "Manual"

set output "96-leftlowertheta.png"
plot 'set96.dat' using 1:5 lt 1 title "Auto", 'set96-manual.dat' using 1:5 lt 3 title "Manual"

set output "96-rightlowertheta.png"
plot 'set96.dat' using 1:9 lt 1 title "Auto", 'set96-manual.dat' using 1:9 lt 3 title "Manual"

set output "96-rightthightheta.png"
plot 'set96.dat' using 1:7 lt 1 title "Auto", 'set96-manual.dat' using 1:7 lt 3 title "Manual"
