set xlabel "Number of deployed streams" font "Arial,12"
set ylabel "Time (Millisecond)" font "Arial,12"

set xtics
set ytics
set key left top Left reverse nobox
set terminal png font "Arial,12"
set output "Computation.png"
set xtics 0 ,20 ,200

set title "Computation Time" font "Arial,16"
# Plotting
plot \
"Computation_ATS.dat" using 1:2 with lines linewidth 2 title "ATS", \
"Computation_TDMA.dat" using 1:2 with lines linewidth 2 title "TDMA-based" \