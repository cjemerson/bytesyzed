/*	ByteSyzed.h
*
*	ByteSyzed Micro CPU Emulator Header.
*
*	Based on Lawlor's UEMU and Funk EMU assembly emulators.
*	https://www.cs.uaf.edu/2005/fall/cs301/uemu/
*	https://www.cs.uaf.edu/2006/fall/cs301/funk_emu/ 
*
*	Written by Charles Emerson (cjemerson AT alaska.edu)
*	Created: 10/1/2017
*	Last Edited: 10/1/2017
*/

#ifndef BYTESYZED_H
#define BYTESYZED_H

#include <stdio.h>

/* Base on Lawlor's tiny CPU class */
class ByteSyzed {
public:
	enum {n_regs=16};
	unsigned char regs[n_regs]; /* "registers" -- temporary working space of machine */
	enum {n_mem=256};
	unsigned char mem[n_mem]; /* "memory"-- program memory and potentially hard coded values */

	unsigned char progStart = 0x00;
	bool prompt = true; /* Prints "Enter value: " prompt for getchar*/
	bool verbose = true; /* Prints out summary of the instruction executed */
	bool loadVerbose = false; /* Prints out a summary of the loadFromFile. Generally leave false, this gets annoying.  */
	
	unsigned char run(void); /* Executes the loaded program instructions */
	void dump(unsigned char id); /* Print out memory and/or all registers. Has a disabled dump. */
	void dumpMemRange(unsigned char first, unsigned char last); /* Prints a memory range */
	void dumpRegs(unsigned char id); /* Prints an individual register or all registers */
	void fileDump(); /* File dump, for debugging suite */
	void wipeMemory(void); /* Zeroes out memory and registers */
	bool loadFromFile(const char * inputFileName); /* Loads from file */
};

#endif