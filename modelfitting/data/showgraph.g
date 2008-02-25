# Graph options
set view map
set pm3d map

set title "__LIMB__ energy for __VOXEL_FILENAME__"
set xlabel "Theta"
set ylabel "Alpha"
unset key

# Plot to an X11 window
set term x11 persist title "__VOXEL_FILENAME__ - graph"
splot '__DATA_FILENAME__'

exit
