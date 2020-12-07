import os, glob
import subprocess
import shutil


# Schedulability ----------------------------------------------------------------------
process = subprocess.Popen(["../StreamReserve.exe", "A", "0", "U"])
process.wait()
shutil.copy2('Schedulability.txt', './Schedulability/ATS-U.dat')
process = subprocess.Popen(["../StreamReserve.exe", "A", "0", "N"])
process.wait()
shutil.copy2('Schedulability.txt', './Schedulability/ATS-N.dat')
process = subprocess.Popen(["../StreamReserve.exe", "T", "0", "U"])
process.wait()
shutil.copy2('Schedulability.txt', './Schedulability/TDMA-U.dat')
process = subprocess.Popen(["../StreamReserve.exe", "T", "0", "N"])
process.wait()
shutil.copy2('Schedulability.txt', './Schedulability/TDMA-N.dat')


# Computation time --------------------------------------------------------------------
EPOCH = 20

with open('./Computation/Total-ATS-U.txt', 'w') as filetowrite:
	for i in range(300):
		filetowrite.write("0\n")
with open('./Computation/Total-ATS-N.txt', 'w') as filetowrite:
	for i in range(300):
		filetowrite.write("0\n")
with open('./Computation/Total-TDMA-U.txt', 'w') as filetowrite:
	for i in range(300):
		filetowrite.write("0\n")
with open('./Computation/Total-TDMA-N.txt', 'w') as filetowrite:
	for i in range(300):
		filetowrite.write("0\n")

# ATS + Uniform_distribution
for i in range(EPOCH):
	for filename in glob.glob("./ATS*"):
		os.remove(filename)
	process = subprocess.Popen(["../StreamReserve.exe", "A", str(i), "U"])
	process.wait()
	f = open('ATS-S2P2.txt', 'r')
	new_time = [float(line) for line in f.readlines()]
	f.close()
	f = open('./Computation/Total-ATS-U.txt', 'r')
	total_time = [float(line) for line in f.readlines()]
	f.close()
	for j in range(300):
		total_time[j] += (new_time[j] / EPOCH)
	with open('./Computation/Total-ATS-U.txt', 'w') as filetowrite:
		for t in total_time:
			filetowrite.write("%f\n" % t)
f = open('./Computation/Total-ATS-U.txt', 'r')
total_time = [float(line) for line in f.readlines()]
f.close()
with open('./Computation/Total-ATS-U.txt', 'w') as filetowrite:
	for i in range(300):
		filetowrite.write("%d %f\n" % (i, total_time[i]))

# ATS + Normal_distribution
for i in range(EPOCH):
	for filename in glob.glob("./ATS*"):
		os.remove(filename)
	process = subprocess.Popen(["../StreamReserve.exe", "A", str(i), "N"])
	process.wait()
	f = open('ATS-S2P2.txt', 'r')
	new_time = [float(line) for line in f.readlines()]
	f.close()
	f = open('./Computation/Total-ATS-N.txt', 'r')
	total_time = [float(line) for line in f.readlines()]
	f.close()
	for j in range(300):
		total_time[j] += (new_time[j] / EPOCH)
	with open('./Computation/Total-ATS-N.txt', 'w') as filetowrite:
		for t in total_time:
			filetowrite.write("%f\n" % t)
f = open('./Computation/Total-ATS-N.txt', 'r')
total_time = [float(line) for line in f.readlines()]
f.close()
with open('./Computation/Total-ATS-N.txt', 'w') as filetowrite:
	for i in range(300):
		filetowrite.write("%d %f\n" % (i, total_time[i]))
		
# TDMA + Uniform_distribution
for i in range(EPOCH):
	for filename in glob.glob("./TDMA*"):
		os.remove(filename)
	process = subprocess.Popen(["../StreamReserve.exe", "T", str(i), "U"])
	process.wait()
	f = open('TDMA-S2P2.txt', 'r')
	new_time = [float(line) for line in f.readlines()]
	f.close()
	f = open('./Computation/Total-TDMA-U.txt', 'r')
	total_time = [float(line) for line in f.readlines()]
	f.close()
	for j in range(300):
		total_time[j] += (new_time[j] / EPOCH)
	with open('./Computation/Total-TDMA-U.txt', 'w') as filetowrite:
		for t in total_time:
			filetowrite.write("%f\n" % t)
f = open('./Computation/Total-TDMA-U.txt', 'r')
total_time = [float(line) for line in f.readlines()]
f.close()
with open('./Computation/Total-TDMA-U.txt', 'w') as filetowrite:
	for i in range(300):
		filetowrite.write("%d %f\n" % (i, total_time[i]))
		
# TDMA + Normal_distribution
for i in range(EPOCH):
	for filename in glob.glob("./TDMA*"):
		os.remove(filename)
	process = subprocess.Popen(["../StreamReserve.exe", "T", str(i), "N"])
	process.wait()
	f = open('TDMA-S2P2.txt', 'r')
	new_time = [float(line) for line in f.readlines()]
	f.close()
	f = open('./Computation/Total-TDMA-N.txt', 'r')
	total_time = [float(line) for line in f.readlines()]
	f.close()
	for j in range(300):
		total_time[j] += (new_time[j] / EPOCH)
	with open('./Computation/Total-TDMA-N.txt', 'w') as filetowrite:
		for t in total_time:
			filetowrite.write("%f\n" % t)
f = open('./Computation/Total-TDMA-N.txt', 'r')
total_time = [float(line) for line in f.readlines()]
f.close()
with open('./Computation/Total-TDMA-N.txt', 'w') as filetowrite:
	for i in range(300):
		filetowrite.write("%d %f\n" % (i, total_time[i]))
		

# Clear -----------------------------------------------------------------------------
for filename in glob.glob("./ATS*"):
	os.remove(filename)
for filename in glob.glob("./TDMA*"):
	os.remove(filename)
for filename in glob.glob("./Schedulability*"):
	os.remove(filename)
