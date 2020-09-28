set xlabel "Number of deployed streams" font "Arial,12"
set ylabel "Time (Millisecond)" font "Arial,12"

set xtics
set ytics
set key left top Left reverse nobox
set terminal png font "Arial,12"
set output "Computation.png"
set xtics 0 , 20, 300

set title "Computation Time" font "Arial,16"
# Plotting
plot \
"Computation_ATS_uniform.dat" using 1:2 every 10 with lines linewidth 2 title "ATS (Uniform)", \
"Computation_TDMA_uniform.dat" using 1:2 every 10 with lines linewidth 2 title "TDMA-based (Uniform)", \
"Computation_ATS_normal.dat" using 1:2 every 10 with lines linewidth 2 title "ATS (Normal)", \
"Computation_TDMA_normal.dat" using 1:2 every 10 with lines linewidth 2 title "TDMA-based (Normal)" \