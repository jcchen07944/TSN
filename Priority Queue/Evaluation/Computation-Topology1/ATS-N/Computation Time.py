import os, glob
import subprocess

with open('Total-ATS-N.txt', 'w') as filetowrite:
	for i in range(300):
		filetowrite.write("0\n")

for i in range(20):
	process = subprocess.Popen(["StreamReserve.exe", str(i)])
	process.wait()
	f = open('ATS-S2P2.txt', 'r')
	new_time = [float(line) for line in f.readlines()]
	f.close()
	f = open('Total-ATS-N.txt', 'r')
	total_time = [float(line) for line in f.readlines()]
	f.close()
	for j in range(300):
		total_time[j] += (new_time[j] / 20)
	with open('Total-ATS-N.txt', 'w') as filetowrite:
		for t in total_time:
			filetowrite.write("%f\n" % t)
	for filename in glob.glob("./ATS*"):
		os.remove(filename)

f = open('Total-ATS-N.txt', 'r')
total_time = [float(line) for line in f.readlines()]
f.close()
with open('Total-ATS-N.txt', 'w') as filetowrite:
	for i in range(300):
		filetowrite.write("%d %f\n" % (i, total_time[i]))