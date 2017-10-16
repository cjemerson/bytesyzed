#!/bin/bash

g++-6 -std=c++14 main.cpp ByteSyzed.cpp -o a.out;

for debugFile in Debug/*.txt;
do

	echo "\n";
	./a.out $debugFile;

	#debug.txt is updated by the 
	python3 debugger.py debug.txt $debugFile;
done;