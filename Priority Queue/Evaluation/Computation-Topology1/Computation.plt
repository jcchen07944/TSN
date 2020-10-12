set xlabel "Number of deployed streams" font "Arial,12"
set ylabel "Time (Millisecond)" font "Arial,12"

set xtics
set ytics
set key left top Left reverse nobox
set terminal png font "Arial,12"
set output "Computation.png"
set xtics 0 , 20, 300
set yr [0:40]

set title "Computation Time" font "Arial,16"
# Plotting
plot \
"Total-ATS-U.txt" using 1:2 every 10 with lines linewidth 2 title "ATS (Uniform)", \
"Total-TDMA-U.txt" using 1:2 every 10 with lines linewidth 2 title "TDMA-based (Uniform)", \
"Total-ATS-N.txt" using 1:2 every 10 with lines linewidth 2 title "ATS (Normal)", \
"Total-TDMA-N.txt" using 1:2 every 10 with lines linewidth 2 title "TDMA-based (Normal)" \