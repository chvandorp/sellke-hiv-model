#!/usr/bin/python2.7

##### script for splitting parametersweeps in multiple threads #####

## -> run several parametersweeps (with the -d and -r flags)
## -> and then join the xml output files

import sys # for command line arguments 
from multiprocessing import Process, Pipe
import time
from itertools import izip
import subprocess
import xml.etree.ElementTree as ET

## get the command line arguments given

seed = "144169"
parameter_file_name = "parameters.dat"
number_of_cores = 2

for i in range(1,len(sys.argv),2):
	if sys.argv[i] == "-s":
		seed = sys.argv[i+1] ## we only need the representation as a string
	if sys.argv[i] == "-p":
		parameter_file_name = sys.argv[i+1]
	if sys.argv[i] == "-c":
		number_of_cores = int(sys.argv[i+1])


## :: aux function for pretty printing xml files

def indent(elem, level=0):
    i = "\n" + level*"  "
    if len(elem):
        if not elem.text or not elem.text.strip():
            elem.text = i + "  "
        if not elem.tail or not elem.tail.strip():
            elem.tail = i
        for elem in elem:
            indent(elem, level+1)
        if not elem.tail or not elem.tail.strip():
            elem.tail = i
    else:
        if level and (not elem.tail or not elem.tail.strip()):
            elem.tail = i

## :: functions for processes

def spawn(f):  
       def fun(pipe,x):
           pipe.send(f(x))  
           pipe.close()  
       return fun

def parmap(f,X,cores):
	pipe=[Pipe() for x in X]	
	processes = [Process(target=spawn(f),args=(c,x)) for x,(p,c) in izip(X,pipe)]
	def proc():
		for proc in processes:
			yield proc
	numProcesses = len(processes)
	processNum = 0 
	outputList = []

	free_cores = cores
	running = set()
	done = []
	process_gen = proc()
	print "starting", min(free_cores,numProcesses), "processes"
	while processNum < numProcesses:
		throw_out = set()
		for i in range(free_cores):
			try:
				proc = process_gen.next()
				#time.sleep(0.5)
			except StopIteration:
				break
			proc.start()
			free_cores -= 1
			print free_cores, "free cores after starting"
			running.add(proc)
			processNum +=1
		#time.sleep(0.5)
		for proc in running:
			if not proc.is_alive():
				free_cores += 1
				print free_cores, "free cores after throwing out"                                       
				throw_out.add(proc)               
				done.append(proc)
		running -= throw_out

	for proc in done: 
		proc.join() 
	for proc,c in pipe: 
		outputList.append(proc.recv())
	return outputList

## :: make the processes

def toastThis(x): 
	subprocess.call(["nice"] + ["./parametersweep"] + x)
	#subprocess.call(["echo"] + x)

flagsandargs = ["-p",parameter_file_name,"-s",seed,"-d",str(number_of_cores),"-r"]
X = [flagsandargs+[str(remainder)] for remainder in range(0,number_of_cores)]

parmap(toastThis,X,number_of_cores)

## :: join the output files...

## :: get the name of the output xml file form parameters.dat

parfile = open(parameter_file_name)
lines = parfile.readlines()

outfileparname = 'NGMPARSWEEP_WRITE_FILENAME'
ell = len(outfileparname)
rightline = filter(lambda line: line[0:ell] == outfileparname,lines)[0]
splitted_rightline = rightline.split('\t')
outfilename = filter(lambda ch: not ch == '\n',splitted_rightline[len(splitted_rightline)-1])


indOutfilenames = ['' for _ in range(0,number_of_cores)]
for r in range(0,number_of_cores):
	indexString = '[' + str(r) + '(' + str(number_of_cores) + ')]'
	splittedOutfilename = outfilename.split('.')
	indOutfilenames[r] = splittedOutfilename[0] + indexString + '.' + splittedOutfilename[1]

## :: import the xml files

roots = []
for indOutFN in indOutfilenames:
	file_r = open(indOutFN)
	tree_r = ET.parse(file_r)
	roots += [tree_r.getroot()]

root = roots[0]

for r in range(1,number_of_cores):
	for element in roots[r]:
		root.append(element)

indent(root) # makes the xml file pretty

## :: write to one file (as mentioned in the parameter file)

output_file = open(outfilename,'w')
output_file.write(ET.tostring(root))
output_file.close()







