import os
import numpy as np
import math
import matplotlib.pyplot as plt

def main():
	csvFile="memory_latency_0.csv"
	flag=False
	output=[]
	outputs=[64,128,256,512,1024]	
	with open(csvFile) as f:
		content = f.readlines()
		cycles=[]
		for entry in content:
			sample=int(entry.split(",")[0])
			size=int(entry.split(",")[1])
			stride=int(entry.split(",")[2])
			cycle=int(entry.split(",")[3])
			if sample==0 and len(cycles)>0:
				cycles=np.array(cycles)				
				cycles=	removeOutliers(cycles)			
				avg=mean(cycles)
				stdev=sdDeviation(cycles)
				output.append((avg,stdev,size,stride))
				cycles=[]		
				
			else:
				flag=True				
				cycles.append(cycle)
	print output
	xaxis=[]
	yaxis=[]
	for e in outputs:
		x=[]
		y=[]
		for entry in output:
			strides=entry[-1]
			if strides==e:
				x.append(math.log(entry[-2],2))
				y.append(entry[0]*0.41666*(10**(-9)))
		xaxis.append(x)
		yaxis.append(y)
	i=0
	while i<len(xaxis):
		plt.plot(xaxis[i],yaxis[i], label=str(outputs[i]))
		i+=1
	plt.xlim(xmin=10)

	plt.show()

	
		
		
	
			
def mean(entries):
	return np.mean(entries, axis=0)

def sdDeviation(entries):
	return np.std(entries)

def removeOutliers(entries,m=2):
	return entries[abs(entries - np.mean(entries)) < m * np.std(entries)]	


if __name__=="__main__":
	main()
