import os
import numpy as np

def main():
	csvFile="mem_latency_0.csv"
	flag=False
	output=[]	
	with open(csvFile) as f:
		content = f.readlines()
		cycles=[]
		for entry in content:
			sample=entry.split(",")[0]
			size=entry.split(",")[1]
			stride=entry.split(",")[2]
			cycle=entry.split(",")[3]
			if size==0 and flag:
				cycles=np.array(cycles)				
				cycles=	removeOutliers(cycles)			
				avg=mean(cycles)
				stdev=sdDeviation(cycles)
				output.append((avg,stdev,size,stride))
				cycles[:]=[]		
				
			else:
				flag=True				
				cycles.append(cycle)
			
def mean(entries):
	return np.mean(entries, axis=0)

def sdDeviation(entries):
	return np.std(entries)

def removeOutliers(entries,m=2):
	return entries[abs(entries - np.mean(entries)) < m * np.std(entries)]	


if __name__=="__main__":
	main()
