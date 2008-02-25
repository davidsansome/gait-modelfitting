# Graph options
set view map
set pm3d map

set title "__LIMB__ energy for __VOXEL_FILENAME__"
set xlabel "Theta"
set ylabel "Alpha"
unset key

# Save the file
set output '__OUT_FILENAME__'
set term __TERM_TYPE__
splot '__DATA_FILENAME__'

exit
