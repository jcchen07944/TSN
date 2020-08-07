# Unset all properties
unset title
unset key
unset xtics
unset ytics

# Setup output environment
set term postscript eps enhanced
set output "P1.eps"

# Graphic settings
set style fill pattern
set style histogram clustered
set style data histogram

set size 0.8,0.8

set xlabel "Swtich types" font "Arial,22" offset 0,-1.5
set ylabel "Number of rule replacements (K)" font "Arial,22" offset -1,0

set xtics offset 1,0
set ytics
set key left top Left reverse nobox
set terminal postscript solid "Arial" 18

set grid y

set datafile missing "-"

# Plotting
plot [][0:5000] "P1.dat" \
	   u ($2 /1000):xtic(1) t "VLB", \
	"" u ($3 /1000) t "Random", \
	"" u ($4 /1000) t "FSMA w RI",\
	"" u ($5 /1000) t "FSMA w RF",\
	"" u ($6 /1000) t "FSMA"
	
