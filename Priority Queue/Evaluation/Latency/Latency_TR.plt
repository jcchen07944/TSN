# Unset all properties
unset title
unset key
unset xtics
unset ytics

# Setup output environment
set term postscript eps enhanced
set output "Latency_TR.eps"

# Graphic settings
# set style data linespoints

set size 0.8,0.8

set xlabel "Number of packet" font "Arial,22"
set ylabel "End-to-end latency (microsecond)" font "Arial,22"

set xtics
set xzeroaxis
set ytics
set yzeroaxis
set key outside
set terminal postscript solid "Arial" 18

set grid y

set datafile missing "-"

# Plotting
plot[:][0:60] \
"TR.dat" using 1:2 with linespoints linewidth 2 title "flow0", \
"TR.dat" using 1:3 with linespoints linewidth 2 title "flow1", \
"TR.dat" using 1:4 with linespoints linewidth 2 title "flow2", \
"TR.dat" using 1:5 with linespoints linewidth 2 title "flow3", \
"TR.dat" using 1:6 with linespoints linewidth 2 title "flow4", \
"TR.dat" using 1:7 with linespoints linewidth 2 title "flow5", \
"TR.dat" using 1:8 with linespoints linewidth 2 title "flow6", \