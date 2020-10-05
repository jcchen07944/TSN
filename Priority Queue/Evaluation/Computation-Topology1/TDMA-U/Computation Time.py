import os, glob
import subprocess

for i in range(5):
	process = subprocess.Popen('StreamReserve.exe')
	process.wait()
	f = open('TDMA-S2P1.txt', 'r')
	new_time = [float(line) for line in f.readlines()]
	f.close()
	f = open('Total-TDMA-U.txt', 'r')
	total_time = [float(line) for line in f.readlines()]
	f.close()
	for j in range(300):
		total_time[j] += (new_time[j] / 5)
	with open('Total-TDMA-U.txt', 'w') as filetowrite:
		for t in total_time:
			filetowrite.write("%f\n" % t)
	for filename in glob.glob("./TDMA*"):
		os.remove(filename) 