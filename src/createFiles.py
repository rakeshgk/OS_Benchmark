def main():
	fileSizes=[524288]
	size=524288*2
	while(size<=8589934592):
		fileSizes.append(size)
		size=size*2
	for filename in fileSizes:
		f = open('../data/files/'+str(filename),"wb")
		f.seek(filename-1)
		f.write("\0")
		f.close()

if __name__=='__main__':
	main()
