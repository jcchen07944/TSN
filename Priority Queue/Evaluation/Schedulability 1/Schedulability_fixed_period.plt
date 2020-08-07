# Unset all properties
unset title
unset key
unset xtics
unset ytics

# Setup output environment
set term postscript eps enhanced
set output "Schedulability_fixed_period.eps"

# Graphic settings
set style fill pattern
set style histogram clustered
set style data histogram

set size 0.8,0.8

set xlabel "Packet size (Bytes)" font "Arial,22"
set ylabel "Accepted Streams" font "Arial,22"

set xtics
set ytics
set key right top Left reverse nobox
set terminal postscript solid "Arial" 18

set grid y

set datafile missing "-"

# Plotting
plot \
"Schedulability_fixed_period.dat" using 2:xtic(1) with histogram title "Time slot", \
"Schedulability_fixed_period.dat" using 3:xtic(1) with histogram title "ATS", \