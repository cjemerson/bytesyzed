#!/bin/bash

# Compiles main.cpp and ByteSyzed.cpp beforehand
g++-6 -std=c++14 main.cpp ByteSyzed.cpp -o a.out;

# Note "Debug/E1.txt" this debug file requires user input to properly debug
# Input 0x42 (66 in decimal) to properly debug
for debugFile in Debug/*.txt;
do
	# Separates loop iterations for readability
	echo "\n";

	# Runs ByteSyzed with the debugFile as the input
	./a.out $debugFile;

	# debug.txt is updated by the ./a.out call and
	# compared with the theoretical output of the debugFile
	# through the python script debugger.py
	python3 debugger.py debug.txt $debugFile;
done;