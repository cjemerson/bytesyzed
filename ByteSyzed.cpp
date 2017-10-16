/*	ByteSyzed.cpp
*
*	ByteSyzed Micro CPU Emulator Definition.
*
*	Based on Lawlor's UEMU and Funk EMU assembly emulators.
*	https://www.cs.uaf.edu/2005/fall/cs301/uemu/
*	https://www.cs.uaf.edu/2006/fall/cs301/funk_emu/ 
*
*	Written by Charles Emerson (cjemerson AT alaska.edu)
*	Created: 10/1/2017
*	Last Edited: 10/15/2017
*/

#include "ByteSyzed.h"

/* Prints out the registers */
void ByteSyzed::dump(unsigned char id) {
	switch (id) {
		case 0xFD: /* Dump both all of memory and all the registers */ 
		case 0xFE: /* Dump all memory */
			dumpMemRange(0, sizeof(mem)/sizeof(unsigned char) - 1);
			if (id == 0xFE)	break; /* Dump only memory */
		case 0xFF: /* Dump all registers */
			dumpRegs(0xFF);
			break;
		default: /* If it is a valid register then dump, else do nothing */
			if (0 <= id && id < (sizeof(regs)/sizeof(unsigned char)))
				dumpRegs(id);
			else
				if (verbose) printf("Invalid dump id: %i. Disabled dump.\n", id);
	}
	return;
}

/* Prints a range of memory */
void ByteSyzed::dumpMemRange(unsigned char first, unsigned char last) {
		if (verbose) printf("Memory:\n");

		/* Handles printing memory from lowest to highest memroy */
		for (int index = first; index <= last; ++index)
			printf("0x%02X : 0x%02X\n", index, mem[index]);

		/* Handles printing memory from highest to lowest memroy */
		for (int index = first; index >= last; --index)
			printf("0x%02X : 0x%02X\n", index, mem[index]);
}

/* Prints individual or all registers */
void ByteSyzed::dumpRegs(unsigned char id) {
	/* Prints an individual register if between 0 and the max register index */
	if (0 <= id && id < sizeof(regs)/sizeof(unsigned char)) {
		printf("regs[0x%01X] = 0x%02X\n", id, regs[id]);
	}
	else { /* Prints all registers */
		if (verbose) printf("Registers:\n");
		for (int index = 0; index < (sizeof(regs)/sizeof(unsigned char)); ++index) {
			printf("0x%01X : 0x%02X\n", index, regs[index]);
		}
	}
}

/* File dump. Dumps memory andy register values, used for debugging suite. */
void ByteSyzed::fileDump() {
	/* File pointer */
	FILE * file = fopen("debug.txt", "w");

	/* Prints out the value of each index of memory in hex */
	for (int index = 0; index < sizeof(mem)/sizeof(unsigned char); ++index) {
		fprintf(file, "%02X\n", mem[index]);
	}

	/* Prints out the value of each register in hex */
	for (int index = 0; index < sizeof(regs)/sizeof(unsigned char); ++index) {
		fprintf(file, "%02X\n", regs[index]);
	}

	/* Closes the file pointer */
	fclose(file);
}

/* Operates the ByteSyzed CPU */
unsigned char ByteSyzed::run(void) {
	if (verbose) printf("Running...\n");
	regs[0xF] = progStart; /* "Program counter" -- points to program start */
	regs[0xE] = (sizeof(mem)/sizeof(unsigned char))-1; /* "Stack pointer" -- points to highest memory address */
	mem[regs[0xE]] = progStart; /* Program start is stored at the bottom of the stack */

	/* Continues to run until invalid opcode, seg faults, or emulator exits */
	while(true) {
		unsigned char opcode = mem[regs[0xF]];
		unsigned char next1 = mem[regs[0xF]+1], next2 = mem[regs[0xF]+2], next3 = mem[regs[0xF]+3];

		if (verbose) printf("  [0x%02X] : 0x%02X", regs[0xF], opcode);

		switch(opcode) {
			case 0x00: /* "movA val" -- mov reg[A], val */
			case 0x01:
			case 0x02:
			case 0x03:
			case 0x04:
			case 0x05:
			case 0x06:
			case 0x07:
			case 0x08:
			case 0x09:
			case 0x0A:
			case 0x0B:
			case 0x0C:
			case 0x0D:
			case 0x0E:
			case 0x0F:
				regs[opcode & 0xF] = next1;
				if (verbose) printf(" 0x%02X\t\tregs[0x%01X] = 0x%02X\n", next1, opcode & 0xF, next1);
				if ((opcode & 0xF) != 0xF) { /* Have to compensate for the idea of immediately changing the program counter */
					++regs[0xF];
					break;
				} else {
					continue;
				}
			case 0x10: /* "mov AB" -- reg[A] = reg[B] */
				regs[next1 >> 4] = regs[next1 & 0xF];
				++regs[0xF];
				if (verbose) printf(" 0x%02X\t\tregs[0x%01X] = regs[0x%01X]\n", next1, next1 >> 4, next1 & 0xF);
				break;
			case 0x11: /* "mov AB" -- reg[A] = mem[regs[B]] */
				regs[next1 >> 4] = mem[regs[next1 & 0xF]];
				++regs[0xF];
				if (verbose) printf(" 0x%02X\t\tregs[0x%01X] = mem[regs[0x%01X]]\n", next1, next1 >> 4, next1 & 0xF);
				break;
			case 0x12: /* "mov AB" -- mem[reg[A]]=reg[B] */
				mem[regs[next1 >> 4]] = regs[next1 & 0xF];
				++regs[0xF];
				if (verbose) printf(" 0x%02X\t\tmem[regs[0x%01X]] = regs[0x%01X]\n", next1, next1 >> 4, next1 & 0xF);
				break;
			case 0x13: /* "mov AB" -- mem[reg[A]] = mem[reg[B]] */
				mem[regs[next1 >> 4]] = mem[regs[next1 & 0xF]];
				++regs[0xF];
				if (verbose) printf(" 0x%02X\t\tmem[regs[0x%01X]] = mem[regs[0x%01X]]\n", next1, next1 >> 4, next1 & 0xF);
				break;
			case 0x14: /* "mov AB val" -- reg[A] = val, reg[B] = val */
				regs[next1 >> 4] = next2, regs[next1 & 0xF] = next2;
				regs[0xF] += 2;
				if (verbose) printf(" 0x%02X 0x%02X\tregs[0x%01X] = 0x%02X, regs[0x%01X] = 0x%02X\n", next1, next2, next1>>4,next2, next1 & 0xF, next2);
				break;
			case 0x15: /* "inc AB" -- reg[A] += B */
				regs[next1 >> 4] += next1 & 0xF;
				++regs[0xF];
				if (verbose) printf(" 0x%02X\t\tregs[0x%01X] += 0x%01X\n", next1, next1 >> 4, next2 & 0xF);
				break;
			case 0x16: /* "dec AB" -- reg[A] -= B */
				regs[next1 >> 4] -= next1 & 0xF;
				++regs[0xF];
				if (verbose) printf(" 0x%02X\t\tregs[0x%01X] -= 0x%01X\n", next1, next1 >> 4, next1 & 0xF);
				break;
			case 0x17: /* "pco AB" -- reg[A] = regs[0xF] + B */
				regs[next1 >> 4] = regs[0xF] + (next1 & 0xF);
				++regs[0xF];
				if (verbose) printf(" 0x%02X\t\tregs[0x%01X] = regs[0xF] + 0x%02X\n", next1, next1 >> 4, next1 & 0xF);
				break;
			case 0x18: /* "pco AB" -- reg[A] = reg[0xF] - B */
				regs[next1 >> 4] = regs[0xF] - (next1 & 0xF);
				++regs[0xF];
				if (verbose) printf(" 0x%02X\t\tregs[0x%01X] = regs[0xF] - 0x%02X\n", next1, next1 >> 4, next1 & 0xF);
				break;
			case 0x20: /* "and AB" -- reg[A] &= reg[B] */
				regs[next1 >> 4] &= regs[next1 & 0xF];
				++regs[0xF];
				if (verbose) printf(" 0x%02X\t\tregs[0x%01X] &= regs[0x%01X]\n", next1, next1 >> 4, next1 & 0xF);
				break;
			case 0x30: /* "or AB" -- reg[A] |= reg[B] */
				regs[next1 >> 4] |= regs[next1 & 0xF];
				++regs[0xF];
				if (verbose) printf(" 0x%02X\t\tregs[0x%01X] |= regs[0x%01X]\n", next1, next1 >> 4, next1 & 0xF);
				break;
			case 0x40: /* "xor AB" -- reg[A] ^= reg[B] */
				regs[next1 >> 4] ^= regs[next1 & 0xF];
				++regs[0xF];
				if (verbose) printf(" 0x%02X\t\tregs[0x%01X] ^= regs[0x%01X]\n", next1, next1 >> 4, next1 & 0xF);
				break;
			case 0x50: /* "shl AB" -- reg[A] << (reg[B] % 8) */
				regs[next1 >> 4] = regs[next1 >> 4] << (regs[next1 & 0xF] & 0x7);
				++regs[0xF];
				if (verbose) printf(" 0x%02X\t\tregs[0x%01X] << regs[0x%01X]\n", next1, next1 >> 4, next1 & 0xF);
				break;
			case 0x51: /* "shr AB" -- reg[A] >> (reg[B] % 8) */
				regs[next1 >> 4] = regs[next1 >> 4] << (regs[next1 & 0xF] & 0x7);
				++regs[0xF];
				if (verbose) printf(" 0x%02X\t\tregs[0x%01X] >> regs[0x%01X]\n", next1, next1 >> 4, next1 & 0xF);
				break;
			case 0x52: /* "rol AB" -- reg[A] = (reg[A] >> 8-(reg[B]%8)) + (reg[A] << (reg[B]%8))*/
				regs[next1 >> 4] = (regs[next1 >> 4] >> (8 - (regs[next1 & 0xF] & 0x7))) + (regs[next1 >> 4] << (regs[next1] & 0x7));
				++regs[0xF];
				if (verbose) printf(" 0x%02X\t\treg[0x%01X] = (reg[0x%01X] >> 8-(reg[0x%01X]%%8)) + (reg[0x%01X] << (reg[0x%01X]%%8))\n", next1, next1 >> 4, next1 >> 4,  next1 & 0xF, next1 >> 4, next1 & 0xF);
				break;
			case 0x53: /* ror AB* -- reg[A] = (reg[A] << 8-(reg[B]%8)) + (reg[A] >> (reg[B] % 8)) */
				regs[next1 >> 4] = (regs[next1 >> 4] << (8 - (regs[next1 & 0xF] & 0x7))) + (regs[next1 >> 4] >> (regs[next1] & 0x7));
				++regs[0xF];
			if (verbose) printf(" 0x%02X\t\treg[0x%01X] = (reg[0x%01X] << 8-(reg[0x%01X]%%8)) + (reg[0x%01X] >> (reg[0x%01X]%%8))\n", next1, next1 >> 4, next1 >> 4, next1 & 0xF, next1 >> 4, next1 & 0xF);
				break;
			case 0x70: /* "jmp adr" -- jmp [adr] */
				regs[0xF] = next1;
				if (verbose) printf(" 0x%02X\t\t(jmp) Jumping to [0x%02X]\n", next1, next1);
				continue;
			case 0x71: /* "jl AB adr" -- jmp [adr], if reg[A] < reg[B] */ 
				if (regs[next1 >> 4] < regs[next1 & 0xF]) {
					regs[0xF] = next2;
					if (verbose) printf(" 0x%02X 0x%02X\t(jl) Jumping to [0x%02X]\n", next1, next2, next2);
					continue;
				} else {
					if (verbose) printf(" 0x%02X 0x%02X\t(jl) No jump.\n", next1, next2);
					regs[0xF] += 2;
					break;
				}
			case 0x72: /* "jle AB adr" -- jmp [adr], if reg[A] <= reg[B] */
				if (regs[next1 >> 4] <= regs[next1 & 0xF]) {
					regs[0xF] = next2;
					if (verbose) printf(" 0x%02X 0x%02X\t(jle) Jumping to [0x%02X]\n", next1, next2, next2);
					continue;
				} else {
					if (verbose) printf(" 0x%02X 0x%02X\t(jle) No jump.\n", next1, next2);
					regs[0xF] += 2;
					break;
				}
			case 0x73: /* "je AB adr" -- jmp [adr], if reg[A] == reg[B] */
				if (regs[next1 >> 4] == regs[next1 & 0xF]) {
					regs[0xF] = next2;
					if (verbose) printf(" 0x%02X 0x%02X\t(je) Jumping to [0x%02X]\n", next1, next2, next2);
					continue;
				} else {
					if (verbose) printf(" 0x%02X 0x%02X\t(je) No jump.\n", next1, next2);
					regs[0xF] += 2;
					break;
				}
			case 0x74: /* "jge AB adr" -- jmp [adr], if reg[A] >= reg[B] */
				if (regs[next1 >> 4] >= regs[next1 & 0xF]) {
					regs[0xF] = next2;
					if (verbose) printf(" 0x%02X 0x%02X\t(jge) Jumping to [0x%02X]\n", next1, next2, next2);
					continue;
				} else {
					if (verbose) printf(" 0x%02X 0x%02X\t(jge) No jump.\n", next1, next2);
					regs[0xF] += 2;
					break;
				}
			case 0x75: /* "jg AB adr" -- jmp [adr], if reg[A] > reg[B] */
				if (regs[next1 >> 4] > regs[next1 & 0xF]) {
					regs[0xF] = next2;
					if (verbose) printf(" 0x%02X 0x%02X\t(jg) Jumping to [0x%02X]\n", next1, next2, next2);
					continue;
				} else {
					if (verbose) printf(" 0x%02X 0x%02X\t(jg) No jump.\n", next1, next2);
					regs[0xF] += 2;
					break;
				}
			case 0x76: /* "jne AB adr" -- jmp [adr], if reg[A] != reg[B] */
				if (regs[next1 >> 4] != regs[next1 & 0xF]) {
					regs[0xF] = next2;
					if (verbose) printf(" 0x%02X 0x%02X\t(jne) Jumping to [0x%02X]\n", next1, next2, next2);
					continue;
				} else {
					if (verbose) printf(" 0x%02X 0x%02X\t(jne) No jump.\n", next1, next2);
					regs[0xF] += 2;
					break;
				}
			case 0x77: /* "skipIfNZ AB" -- jmp [reg[0xF] + B + 2], if reg[A] != 0 */
				if (regs[next1 >> 4] != 0x0) {
					regs[0xF] += (next1 & 0xF) + 2;
					if (verbose) printf(" 0x%02X\t\t(skipIfNZ) Skipping to [0x%02X]\n", next1, regs[0xF]);
					continue;
				} else {
					if (verbose) printf(" 0x%02X\t\t(skipIfNZ) No skip.\n", next1);
					++regs[0xF];
				}
				break;
			case 0x78: /* "skipIfZ AB" -- jmp [reg[0xF] + B + 2], if reg[A] == 0 */
				if (regs[next1 >> 4] == 0x0) {
					regs[0xF] += (next1 & 0xF) + 2;
					if (verbose) printf(" 0x%02X\t\t(skipIfZ) Skipping to [0x%02X]\n", next1, regs[0xF]);
					continue;
				} else {
					if (verbose) printf(" 0x%02X\t\t(skipIfZ) No skip.\n", next1);
					++regs[0xF];
				}
				break;
			case 0x80: /* "pushA" -- push reg[A] */
			case 0x81:
			case 0x82:
			case 0x83:
			case 0x84:
			case 0x85:
			case 0x86:
			case 0x87:
			case 0x88:
			case 0x89:
			case 0x8A:
			case 0x8B:
			case 0x8C:
			case 0x8D:
			case 0x8E:
			case 0x8F:
				/* Push if not at the edge of memory */
				if(regs[0xE] > 0) {
					mem[--regs[0xE]] = regs[opcode & 0xF];
					if (verbose) printf("\t\t\tPushed regs[0x%01X]=0x%02X to [0x%02X]\n", opcode & 0xF, mem[regs[0xE]], regs[0xE]);
					break;
				} else {
					printf("\nSegmentation fault. Unable to push 0x%02X because at address 0.\n", regs[opcode & 0xF]);
					return(regs[0x0]);
				}
			case 0x90: /* "popA" -- pop reg[A] */
			case 0x91:
			case 0x92:
			case 0x93:
			case 0x94:
			case 0x95:
			case 0x96:
			case 0x97:
			case 0x98:
			case 0x99:
			case 0x9A:
			case 0x9B:
			case 0x9C:
			case 0x9D:
			case 0x9E:
			case 0x9F:
				/* Pop if not at the edge of memory */
				if (regs[0xE] != (sizeof(mem)/sizeof(unsigned char))-1) {
					regs[opcode & 0xF] = mem[regs[0xE]];
					if (verbose) printf("\t\t\tPopped 0x%02X from [0x%02X] into regs[0x%01X]\n", regs[opcode & 0xF], regs[0xE], opcode & 0xF);
					++regs[0xE];
					if ((opcode & 0xF) == 0xF) continue; else break; /* Have to compensate for the idea of immediately changing the program counter */
				} else {
					printf("\nSegmentation fault. At the edge of memory, unable to pop into regs[0x%01X]. Exiting...\n", opcode & 0xF);
					return(regs[0x0]);
				}
			case 0xA0: /* "add AB" -- reg[A] += reg[B] */
				regs[next1 >> 4] += regs[next1 & 0xF];
				++regs[0xF];
				if (verbose) printf(" 0x%02X\t\tregs[0x%01X] += regs[0x%01X]\n", next1, next1 >> 4, next1 & 0xF);
				break;
			case 0xA1: /* "add AB adr" -- reg[A] = reg[B] + mem[adr] */
				regs[next1 >> 4] = regs[next1 & 0xF] + mem[next2];
				regs[0xF] += 2;
				if (verbose) printf(" 0x%02X 0x%02X\tregs[0x%01X] = regs[0x%01X] + mem[0x%02X]\n", next1, next2, next1 >> 4, next1 & 0xF, next2);
				break;
			case 0xA2: /* "add adr AB" -- mem[adr] = reg[A] + reg[B] */
				mem[next1] = regs[next2 >> 4] + regs[next2 & 0xF];
				if (verbose) printf(" 0x%02X 0x%02X\tmem[0x%02X] = regs[0x%01X] + regs[0x%01X]\n", next1, next2, next1, next2 >> 4, next2 & 0xF);
				regs[0xF] += 2;
				break;
			case 0xA3: /* "add AB val" -- reg[A] = reg[B] + val */
				regs[next1 >> 4] = regs[next1 & 0xF] + next2;
				regs[0xF] += 2;
				if (verbose) printf(" 0x%02X 0x%02X\tregs[0x%01X] = regs[0x%01X] + 0x%02X\n", next1, next2, next1 >> 4, next1 & 0xF, next2);
				break;
			case 0xA4: /* "lea AB val" -- reg[A] = B*val */
				regs[next1 >> 4] = (next1 & 0xF)*next2;
				regs[0xF] += 2;
				if (verbose) printf(" 0x%02X 0x%02X\tregs[0x%01X] = 0x%01X * 0x%02X\n", next1, next2, next1 >> 4, next1 & 0xF, next2);
				break;
			case 0xA5: /* "lea AB val" -- reg[A] = B + val */
				regs[next1 >> 4] = (next1 & 0xF) + next2;
				regs[0xF] += 2;
				if (verbose) printf(" 0x%02X 0x%02X\tregs[0x%01X] = 0x%01X + 0x%02X\n", next1, next2, next1 >> 4, next1 & 0xF, next2);
				break;
			case 0xA6: /* "lea AB val1 val2" -- reg[A] = B*val1 + val2 */
				regs[next1 >> 4] = (next1 & 0xF) * next2 + next3;
				regs[0xF] += 3;
				if (verbose) printf(" 0x%02X 0x%02X 0x%02X\tregs[0x%01X] = 0x%01X * 0x%02X + 0x%02X\n", next1, next2, next3, next1 >> 4, next1 & 0xF, next2, next3);
				break;
			case 0xA7: /* "sub AB" -- reg[A] -= reg[B] */
				regs[next1 >> 4] -= regs[next1 & 0xF];
				++regs[0xF];
				if (verbose) printf(" 0x%02X\t\tregs[0x%01X] -= regs[0x%01X]\n", next1, next1 >> 4, next1 & 0xF);
				break;
			case 0xA8: /* "sub AB adr" -- reg[A] = reg[B] - mem[adr] */
				regs[next1 >> 4] = regs[next1 & 0xF] - mem[next2];
				regs[0xF] += 2;
				if (verbose) printf(" 0x%02X 0x%02X\tregs[0x%01X] = regs[0x%01X] - mem[0x%02X]\n", next1, next2, next1 >> 4, next1 & 0xF, next2);
				break;
			case 0xA9: /* "sub AB adr" -- reg[A] = mem[adr] - reg[B] */
				regs[next1 >> 4] = mem[next2] - regs[next1 & 0xF];
				regs[0xF] += 2;
				if (verbose) printf(" 0x%02X 0x%02X\tregs[0x%01X] = mem[0x%02X] - regs[0x%01X]\n", next1, next2, next1 >> 4, next2, next1 & 0xF);
				break;
			case 0xAA: /* "sub adr AB" -- mem[adr] = reg[A] - reg[B] */
				mem[next1] = regs[next2 >> 4] - regs[next2 & 0xF];
				regs[0xF] += 2;
				if (verbose) printf(" 0x%02X 0x%02X\tmem[0x%02X] = regs[0x%01X] - regs[0x%01X]\n", next1, next2, next1, next2 >> 4, next2 & 0xF);
				break;
			case 0xAB: /* "sub AB val" -- reg[A] = reg[B] - val */
				regs[next1 >> 4] = regs[next1 & 0xF] - next2;
				regs[0xF] += 2;
				if (verbose) printf(" 0x%02X 0x%02X\tregs[0x%01X] = regs[0x%01X] - 0x%02X\n", next1, next2, next1 >> 4, next1 & 0xF, next2);
				break;
			case 0xAC: /* "sub AB val" -- reg[A] = val - reg[B] */
				regs[next1 >> 4] = next2 - regs[next1 & 0xF];
				regs[0xF] += 2;
				if (verbose) printf(" 0x%02X 0x%02X\tregs[0x%01X] = 0x%02X - regs[0x%01X]\n", next1, next2, next1 >> 4, next2, next1 & 0xF);
				break;			
			case 0xC2: /* "call adr" -- call [adr] */
				if (regs[0xE] > 0) { /* If not at the edge of memory */
					mem[--regs[0xE]] = regs[0xF] + 2; /* push the return address onto the stack */
					regs[0xF] = next1;
					if (verbose) printf(" 0x%02X\t\tCalling [0x%02X]\n", next1, next1);
					continue;
				} else {
					printf(" 0x%02X\nSegmentation fault. At the edge of memory, unable to push return address [0x%02X] onto the stack.\n", next1, regs[0xF] + 2);
					return regs[0x0];
				}
			case 0xC3: /* "ret" -- return */
				if (regs[0xE] < (sizeof(mem)/sizeof(unsigned char) - 1)) { /* If not at the edge of memory */
					regs[0xF] = mem[regs[0xE]++]; /* pop the return address of the stack */
					if (verbose) printf("\t\t\tReturning to [0x%02X]\n", regs[0xF]);
					continue;
				} else {
					printf("\nSegmentation fault. At the edge of memory, unable to pop return address.\n");
					return regs[0x0];
				}
			case 0xD0: /* "nop" -- do nothing */
				if (verbose) printf("\t\t\tDoing nothing\n");
				break;
			case 0xE0: /* "putchar" -- putchar(regs[0x1]) */
				if (verbose) printf("\t\t\tEmulator Output. putchar(regs[0x1]) = '%c'\n", (regs[0x1] != '\n')? regs[0x1] : 1);
				else printf("%c",regs[0x1]);
				break;
			case 0xE1: /* "getchar" -- regs[0x0] = getchar(int) */
				if (verbose) printf("\t\t\tEmulator Input. regs[0x0] = getchar(int)\n");
				if (prompt) printf("Enter an integer value (of a char): ");
				int tempInt;
				scanf("%i", &tempInt);
				regs[0x0] = (unsigned char)(tempInt % 256);
				break;
			case 0xE2: /* "printstr char[],0" -- puts(mem[++regs[0xF]]), while mem[regs[0xF]] != 0 */
				if (verbose) printf("\t\t\tEmulator String Output: ");
				while(mem[++regs[0xF]] != 0) printf("%c", mem[regs[0xF]]);
				if (verbose) printf("\n");
				break;
			case 0xE8: /* "dumpRegs[0x0]" -- dumpRegs(0x0) */
				if (verbose) printf("\t\t\tEmulator Register Dump. Dumping register 0x0.\n");
				dumpRegs(0x0);
				break;
			case 0xE9: /* "dumpRegs[0x1]" -- dumpRegs(0x1) */
				if (verbose) printf("\t\t\tEmulator Register Dump. Dumping register 0x1.\n");
				dumpRegs(0x1);
				break;
			case 0xEA: /* "dumpMem" -- dumpMemRange(0, 0xFF)*/
				if (verbose) printf("\t\t\tEmulator Memory Dump. Dumping all memory.\n");
				dumpMemRange(0, sizeof(mem)/sizeof(unsigned char) - 1);
				break;
			case 0xEB: /* "dumpMemRange" -- dumpMemRange(regs[0x1], regs[0x2]) */
				if (verbose) printf("\t\t\tEmulator Memory Range Dump. Dumping memory range [0x%02X] to [0x%02X]\n", regs[0x1], regs[0x2]);
				dumpMemRange(regs[0x1], regs[0x2]);
				break;
			case 0xEC: /* "dumpRegs" -- dumpRegs(regs[0x1]) */
				if (verbose) printf("\t\t\tEmulator Register Dump. Dumping register 0x%01X.\n", regs[0x1]);
				dumpRegs(regs[0x1]);
				break;
			case 0xED: /* "dump id" -- dump(id) */
				if (verbose) printf(" 0x%02X\t\tEmulator Generic Dump. Dump id = 0x%02X\n", next1, next1);
				dump(next1);
				++regs[0xF];
				break;
			case 0xEE: /* "exit" -- return regs[0x0] */
				if (verbose) printf("\t\t\tEmulator Exit. Returning 0x%02X\n", regs[0x0]);
				return regs[0x0];
			case 0xEF: /* "fileDump" -- dumps to debug.txt (for debugging) */
				if (verbose) printf("\t\t\tEmulator Debug. Dumping to file debug.txt\n");
				fileDump();
				break;
			default: /* Invalid opcode. Exit the emulator. */
				printf("\nInvalid opcode: 0x%02X at mem[0x%02X] Exiting...\n", opcode, regs[0xF]);
				return regs[0x0];
		}
		++regs[0xF]; /* increment the program counter */
	}
	return regs[0x0]; /* I don't know how you'd get here */
}

/* Zeroes out memory. */
void ByteSyzed::wipeMemory(void) {
	/* Iterate through memory and set to zero */
	for (int index = 0; index < sizeof(mem)/sizeof(unsigned char); ++index) {
		mem[index] = 0;
	}

	/* Iterate through registers and set to zero */
	for (int index = 0; index < sizeof(regs)/sizeof(unsigned char); ++index) {
		regs[index] = 0;
	}
}

/* Load from file. Returns false if it fails to load from file. */
bool ByteSyzed::loadFromFile(const char * inputFileName) {
	/* File pointer */
	FILE * file = fopen(inputFileName, "r");

	/* If the file exists */
	if (file == NULL) {
		printf("Can't open %s. Unable to load.\n", inputFileName);
		return false;
	} else {
		if (loadVerbose) printf("Opened %s\n", inputFileName);
	}

	char inLine[1024]; /* Line buffer */
	int val[4]; /* Will hold the read numbers */
	int n, loadCount=0; /* loadCount is the total number of instructions loaded */

	/* While reading valid info from file */
	while (fgets(inLine, 1024, file) != NULL) {
		/* n is the number of successful integers read from sscanf */
		switch (n = sscanf(inLine, "%i %i %i %i", &val[0],&val[1],&val[2],&val[3])) {
			case -1: /* In case of newline */
				if (loadVerbose) printf("  Read newline.\n");
				break;
			case 0: /* If no numbers were read */
				if (loadVerbose) printf("  No instructions in line.\n");
				break;
			case 1:	/* If numbers were successfully read */
			case 2:
			case 3:
			case 4:
				if (loadVerbose) printf("  Read %s", inLine);
				/* Load the n successfully read numbers */
				for (int index = 0; index < n ; ++index) {
					/* Check to see if the number read is outside of valid limits */
					if (val[index] < 0 || val[index] >= sizeof(mem)/sizeof(unsigned char)) {
						printf("  Error. Invalid value %i. Cannot fit into byte (Check input file).\n", val[index]);
						return false;
					}

					/* Check the address to be loaded is within bounds of memory */ 
					if (((int) progStart) + loadCount + index >= sizeof(mem)/sizeof(unsigned char)) {
						printf("  Error. Starting at 0x%02X, the number of instructions loaded (%i) exceed maximum memory.\n", progStart, ((int) progStart)+loadCount+index);
						return false;
					}

					/* Load the value into the ByteSyzed CPU */
					mem[progStart + loadCount + index]=((char)(val[index]));
					if (loadVerbose) printf("    Loaded 0x%02X to mem[0x%02X].\n",val[index], progStart + loadCount + index);
				}

				/* Update the load count */
				loadCount += n;
				break;
			default: /* I don't know how you can get here */
				printf("Error. sscanf returned n = %i\n", n);
				return false;
			}
	}

	if (loadVerbose) printf("Instructions loaded.\n\n");

	fclose(file);
	return true;
}