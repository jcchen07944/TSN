set xlabel "Request streams" font "Arial,12"
set ylabel "Accepted streams" font "Arial,12"

set xtics
set ytics
set key left top Left reverse nobox
set terminal png font "Arial,12"
set output "Schedulability-T2.png"
set xtics 0 ,500 ,3000

set title "Schedulability" font "Arial,16"
# Plotting
plot \
"ATS-U-T2.dat" using 1:2 with lines linewidth 2 title "ATS (Uniform)", \
"TDMA-U-T2.dat" using 1:2 with lines linewidth 2 title "TDMA-based (Uniform)", \
"ATS-N-T2.dat" using 1:2 with lines linewidth 2 title "ATS (Normal)", \
"TDMA-N-T2.dat" using 1:2 with lines linewidth 2 title "TDMA-based (Normal)" \