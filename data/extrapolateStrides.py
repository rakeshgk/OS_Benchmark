import os
import numpy as np
import math
import matplotlib.pyplot as plt
import pprint
def main():
	csvFile="memory_latency_0.csv"
	flag=False
	output=[]
	outputs=[64,128,256,512,1024]	
	with open(csvFile) as f:
		content = f.readlines()
		cycles=[]
		for entry in content:
			if entry=='\n':
				cycles=np.array(cycles)				
				cycles=	removeOutliers(cycles)			
				avg=mean(cycles)
				stdev=sdDeviation(cycles)
				output.append((avg,stdev,size,stride))
				cycles=[]
				#cycles.append(cycle)		
				
			else:
				sample=int(entry.split(",")[0])
				size=int(entry.split(",")[1])
				stride=int(entry.split(",")[2])
				cycle=int(entry.split(",")[3])
				#flag=True				
				cycles.append(cycle)
	xaxis=[]
	yaxis=[]
	outstride={}
	for e in outputs:
		outstride[e]=[]
	for e in outputs:
		x=[]
		y=[]
		for entry in output:
			stride=entry[-1]
			size=entry[-2]
			avg=entry[0]
			stddev=entry[1]	
			if stride==e:
				if size>stride:
					outstride[e].append((size,avg,stddev))
					x.append(math.log(entry[-2],2))
					y.append(entry[0]*0.41666*(10**(-9)))
		xaxis.append(x)
		yaxis.append(y)
	 
	i=0
	pp = pprint.PrettyPrinter(depth=6)
	for e in outputs:
		if e==512:
			start=1024
		elif e==1024:
			start=2048
		else:
			start=512
		print e
		filename=str(e)+".csv"
		f = open(filename,'w')
		for elem in outstride[e]:
			if elem[0]==start: 
				f.write(str(elem[0]) + " " + str(elem[1]) + " " + str(elem[2]) + "\n")	
				start*=2
	
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
