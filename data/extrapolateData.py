import os
import numpy as np

def main():
	csvFiles=[]
	results={}
	for fileName in os.listdir("."):
		if fileName.endswith('.csv'):
			 csvFiles.append(fileName)
	for csvFile in csvFiles:
		entries=[]
		with open(csvFile) as f:
    			content = f.readlines()
			for entry in content:
				entries.append(abs(int(entry.split(",")[1])-int(entry.split(",")[0])))
		entries = np.array(entries)		
		entries = removeOutliers(entries)				
		results[csvFile]=[mean(entries),sdDeviation(entries)]

	print results

def mean(entries):
	return np.mean(entries, axis=0)

def sdDeviation(entries):
	return np.std(entries)

def removeOutliers(entries,m=2):
	return entries[abs(entries - np.mean(entries)) < m * np.std(entries)]		 
			
				
if __name__=="__main__":
	main()
