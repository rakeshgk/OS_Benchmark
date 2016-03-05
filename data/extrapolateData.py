import os
import numpy as np
import pprint

def main():
	csvFiles=['peak_bandwidth.csv']
	results={}
	#for fileName in os.listdir("."):
	#	if fileName.endswith('.csv'):
	#		 csvFiles.append(fileName)
	#csvFiles.remove('measurement_overhead_0.csv')
	#csvFiles.remove('measurement_overhead_1.csv')
	for csvFile in csvFiles:
		entries=[]
		with open(csvFile) as f:
    			content = f.readlines()
			for entry in content:
				entries.append(abs(int(entry.split(",")[1])-int(entry.split(",")[0])))
		entries = np.array(entries)		
		entries = removeOutliers(entries)				
		results[csvFile]=[mean(entries),sdDeviation(entries)]
	pp = pprint.PrettyPrinter(indent=4)
	pp.pprint(results)
	pp.pprint(convert([results['peak_bandwidth.csv'][0]]))

def mean(entries):
	return np.mean(entries, axis=0)

def sdDeviation(entries):
	return np.std(entries)

def removeOutliers(entries,m=2):
	return entries[abs(entries - np.mean(entries)) < m * np.std(entries)]	

def convert(clock_cycles):
	time = 4.1666666666667E-10
	nano_snd = 1E-09
	for cycle in clock_cycles:
    		cycle_time =  (cycle * time) / nano_snd
    		print("%s Cycles -> %s nano seconds\n" %(cycle, cycle_time))	 
			
				
if __name__=="__main__":
	main()
