set xlabel "Request streams" font "Arial,12"
set ylabel "Accepted streams" font "Arial,12"

set xtics
set ytics
set key left top Left reverse nobox
set terminal png font "Arial,12"
set output "Schedulability (Normal Distribution).png"
set xtics 0 ,500 ,3000

set title "Normal Distribution" font "Arial,16"
# Plotting
plot \
"TDMA (Normal Distribution).dat" using 1:2 with lines linewidth 2 title "TDMA-based", \
"ATS (Normal Distribution).dat" using 1:2 with lines linewidth 2 title "ATS" \