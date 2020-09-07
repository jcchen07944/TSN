# Unset all properties
unset title
unset key
unset xtics
unset ytics

# Setup output environment
set term postscript eps enhanced
set output "Schedulability_p150.eps"

# Graphic settings
set style fill pattern
set style histogram clustered
set style data histogram

set size 0.8,0.8

set xlabel "Packet size (Bytes)" font "Arial,22"
set ylabel "Number of deployed streams" font "Arial,22"

set xtics
set ytics
set key right top Left reverse nobox
set terminal postscript solid "Arial" 18

set grid y

set datafile missing "-"

# set title "Pass through two switches"
# Plotting
plot \
"Schedulability_p150.dat" using 2:xtic(1) with histogram title "TDMA-based", \
"Schedulability_p150.dat" using 3:xtic(1) with histogram title "ATS" \