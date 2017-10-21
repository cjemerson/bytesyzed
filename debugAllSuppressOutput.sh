#!/bin/bash

# Compiles main.cpp and ByteSyzed.cpp beforehand
g++-6 -std=c++14 main.cpp ByteSyzed.cpp -o a.out;

# Note that this debug file requires user input to properly debug
requiresInputFile="Debug/E1.txt";

# Iterates through all debug files
for debugFile in Debug/*.txt;
do
	# Lets the user know that the debugFile is being run
	# This is important because the current method of suppressing output
	# also suppresses the "run debugFile" output.
	printf "\n./a.out $debugFile\n";

	# Checks to see if this debugFile requires user input
	if [ "$debugFile" == "$requiresInputFile" ]
	then
		printf "Enter 0x42 (66 in decimal): ";
	fi

	#Suppresses program output.
	./a.out $debugFile >/dev/null;

	# debug.txt is updated by the ./a.out call and
	# compared with the theoretical output of the debugFile
	# through the python script debugger.py
	python3 debugger.py debug.txt $debugFile;
done;