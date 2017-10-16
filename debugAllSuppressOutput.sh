#!/bin/bash

g++-6 -std=c++14 main.cpp ByteSyzed.cpp -o a.out;

requiresInputFile="Debug/E1.txt";

for debugFile in Debug/*.txt;
do
	printf "\n./a.out $debugFile\n";

	if [ "$debugFile" == "$requiresInputFile" ]
	then
		printf "Enter 0x42 (66 in decimal): ";
	fi
	#Suppresses program output.
	./a.out $debugFile >/dev/null;

	#debug.txt is updated by the 
	python3 debugger.py debug.txt $debugFile;
done;