#!/bin/python3

## debugger.py
## Jacob Butler
## 15 October 2017
##
## This script will do a comparison between the output from the ByteSyzed memory dump 
## and a debug file with the correct memory and register values written in it
##
## The first argument to the script is the output from the memory dump
## The second argument is the debugging file it is compared against

import sys

def extractMemRegDebug(inputFile):

	debugFile = open(inputFile, "r")

	inMem = False
	inReg = False

	memory = list()
	registers = list()

	for line in debugFile:
		if(line[0:5] == "!mem#"):
			inMem = True
		elif(line[0:5] == "!mem!"):
			inMem = False
		if(line[0:6] == "!regs#"):
			inReg = True
		elif(line[0:6] == "!regs!"):
			inReg = False
		if(inMem):
			memory.append(line[2:4])
		if(inReg):
			registers.append(line[2:4])

	## remove the first element since it is part of the divider flag 
	del(registers[0])
	del(memory[0])

	## pads out the rest of the memory to 00
	if(len(memory) < 256):
		memory.extend(["00"]*(256-len(memory)))
	return memory, registers

def extractMemRegOutput(inputFile):

	outputFile = open(inputFile, "r")

	memory = list()
	registers = list()

	lineCount = 0

	for line in outputFile:
		## checks if the line is in the memory segment of the output file
		if(lineCount < 256): 
			memory.append(line[0:2])
		## checks if the line in the register segment of the output file 
		elif(lineCount > 255 and lineCount < 272):
			registers.append(line[0:2])
		lineCount += 1
	return memory, registers

def compareOutputDebug(outputMem, debugMem, outputReg, debugReg):
	errors = False
	if(len(debugMem) > 256):
		print("Error: Memory exceeds 256 addresses")
		errors = True
	if(len(debugReg) > 16):
		print("Error: Number of registers exceeds 16")
		errors = True
	if(len(debugReg) < 16):
		print("Error: " + str(16-len(debugReg)) + " registers are unspecified")
		errors = True
	else:
		for i in range(256):
			if(outputMem[i] != debugMem[i]):
				print(" Error: At address " + str("0x%02x" % i) + " should be " + str(debugMem[i]) + " but is " + str(outputMem[i]))
				errors = True
		for i in range(16):
			if(outputReg[i] != debugReg[i]):
				print("Error: Register " + str("0x%02x" % i) + " should be " + str(debugReg[i]) + " but is " + str(outputReg[i]))
				errors = True
	return errors

def debug(outputFile, debugFile):

	memOutput, regOutput = extractMemRegOutput(outputFile)
	memDebug, regDebug = extractMemRegDebug(debugFile)
	print("Testing " + outputFile + " against " + debugFile)

	compareOutputDebug(memOutput, memDebug, regOutput, regDebug)

	print("Testing Complete")
	return

## This call executes the comparison
debug(sys.argv[1], sys.argv[2])

