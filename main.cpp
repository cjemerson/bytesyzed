/*	main.cpp
*
*	Basic CPU Emulator.
*
*	Uses the ByteSyzed (CPU) Class.
*
*	Based on Lawlor's UEMU and Funk EMU assembly emulators.
*	https://www.cs.uaf.edu/2005/fall/cs301/uemu/
*	https://www.cs.uaf.edu/2006/fall/cs301/funk_emu/ 
*
*	Written by Charles Emerson (cjemerson AT alaska.edu)
*	Created: 9/29/2017
*	Last Edited: 10/15/2017
*/

#include "ByteSyzed.h"

int main(int argc, const char * argv[]) {
	ByteSyzed lawlor = {0};

	lawlor.progStart = 0x00; /* Sets where to load program */

	/* Don't want flashy */
	lawlor.prompt = true; /* Enables "Enter value: " prompt */
	lawlor.verbose = false; /* Gives a description of the opcode */
	lawlor.loadVerbose = false;

	/* Default input file if no file name passed. */
	const char * inputFileName = "input.txt";

	/* If a file name was passed use it */
	if (argc > 1) inputFileName = argv[1];

	/* Load from file */
	if(!lawlor.loadFromFile(inputFileName)) {
		printf("Error. Failed to load from file.\n");
		return 0;
	}

	/* Run the ByteSyzed CPU */
	lawlor.run();

	/* Zeroes memory */
	lawlor.wipeMemory();

	return 1;
}