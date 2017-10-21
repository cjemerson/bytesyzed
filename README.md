# ByteSyzed: Micro CPU Emulator

## Introduction
ByteSyzed is a micro CPU emulator with 256 bytes of memory based off of Orion Lawlor's [UEMU](https://www.cs.uaf.edu/2005/fall/cs301/uemu/uemu/uemu.html) and [FUNK emu](https://www.cs.uaf.edu/2006/fall/cs301/funk_emu/) micro emulator architectures. Instructions are loaded from an input file and then executed. ByteSyzed uses scratch and (theoretically) preserved registers to manipulate memory and each other. Powerful things come in small packages. If you desire to use of this emulator, make sure not to byte off more than you can nibble.

To get started: Compile main.cpp and ByteSyzed.cpp and run. (NOTE: You can input a file name in the command line to load and run). To see a trace of the instructions, set ```verbose``` to true.

## Memory
Used for storing program instructions, hard-coded values and the stack.

## Registers
There are 16 one-byte registers numbered 0 through F (in hex). These will expressed as an array with hexadecimal indexes throughout this reference (i.e. register 11 is ```reg[0xB]```). The first 14 registers are scratch registers and the others should not be manipulated directly. The program counter is ```reg[0xF]```, which stores the memory address of the current instruction to execute. The stack pointer is ```reg[0xE]```, which stores values and return addresses.

## Instructions
The instruction encoding is chosen such that everything is a multiple of one byte. This makes machine language programs easy to write in hex. There are currently 55 valid instructions (a.k.a opcodes for operation codes). Instructions vary in size and format. Some take only one byte while others take up to 4 bytes to concisely manipulate the 16 registers and memory. Some use nibbles (or half-bytes) to specify registers and byte-sized values. Because of the memory limitation, the cheapest encoding was considered.

<table>
	<tr> <th>opcode<br>(in&nbsp;hex)</th> <th>Encoding<br>(in&nbsp;bytes)</th> <th>Psuedo-Implementation</th> <th>Description</th> </tr>
	<tr> <td>0x0R</td> <td>movA val</td> <td>reg[A] = val</td> <td>Moves a value into a register immediately.</td> </tr>
	<tr> <td>0x10</td> <td>mov AB</td> <td>reg[A] = reg[B]</td> <td>Copies a register into another register.</td></tr>
	<tr> <td>0x11</td> <td>mov AB</td> <td>reg[A] = mem[reg[B]]</td> <td>Copies the value that a register is pointing to into another register.</td></tr>
	<tr> <td>0x12</td> <td>mov AB</td> <td>mem[reg[A]] = reg[B]</td> <td>Copies a register into the address another register is pointing to.</td> </tr>
	<tr> <td>0x13</td> <td>mov AB</td> <td>mem[reg[A]] = mem[reg[B]]</td> <td>Copies the value a register is pointing to into the address another register is pointing to.</td> </tr>
	<tr> <td>0x14</td> <td>mov AB val</td> <td>reg[A]=val, reg[B]=val</td> <td>Moves a value into two registers.</td></tr>
	<tr> <td>0x15</td> <td>inc AB</td> <td>reg[A] += B</td> <td>Increments the register by a certain amount.</td> </tr>
	<tr> <td>0x16</td> <td>dec AB</td> <td>reg[A] -= B</td> <td>Decrements the register by a certain amount.</td> </tr>
	<tr> <td>0x17</td> <td>pco AB</td> <td>reg[A] = reg[0xF] + B</td> <td>Positive program counter offset. Loads the progam counter plus an offset into a register.</td> </tr>
	<tr> <td>0x18</td> <td>pco AB</td> <td>reg[A] = reg[0xF] - B</td> <td>Negative program counter offset. Loads the progam counter minus an offset into a register.</td> </tr>
	<tr> <td>0x20</td> <td>and AB</td> <td>reg[A] &amp;= reg[B]</td> <td>Bitwise AND two registers.</td> </tr>
	<tr> <td>0x30</td> <td>or AB</td> <td>reg[A] |= reg[B]</td> <td>Bitwise OR two registers.</td> </tr>
	<tr> <td>0x40</td> <td>xor AB</td> <td>reg[A] ^= reg[B]</td> <td>Bitwise XOR two registers.</td> </tr>
	<tr> <td>0x50</td> <td>shl AB</td> <td>reg[A] = reg[A] &lt;&lt; (reg[B] % 8)</td> <td>Bit shifts left the first register by the second register modulo 8.</td> </tr>
	<tr> <td>0x51</td> <td>shr AB</td> <td>reg[A] = reg[A] >> (reg[B] % 8)</td> <td>Bit shifts right the first register by the second register modulo 8.</td> </tr>
	<tr> <td>0x52</td> <td>rol AB</td> <td>reg[A] = (reg[A] >> 8 - (reg[B] % 8)) + (reg[A] &lt;&lt; (reg[B] % 8))</td> <td>Bit rotates left the first register by the second register modulo 8.</td> </tr>
	<tr> <td>0x53</td> <td>ror AB</td> <td>reg[A] = (reg[A] &lt;&lt; 8 - (reg[B] % 8)) + (reg[A] >> (reg[B] % 8))</td> <td>Bit rotates right the first register by the second register modulo 8.</td> </tr>
	<tr> <td>0x70</td> <td>jmp adr</td> <td>goto [adr]</td> <td>Jumps to an address.</td> </tr>
	<tr> <td>0x71</td> <td>jl AB adr</td> <td>goto [adr], if reg[A] &lt; reg[B]</td> <td>Jumps to an address if the first register is less than the other.</td> </tr>
	<tr> <td>0x72</td> <td>jle AB adr</td> <td>goto [adr], if reg[A] &lt;= reg[B]</td> <td>Jumps to an address if the first register is less than or equal to the other.</td> </tr>
	<tr> <td>0x73</td> <td>je AB adr</td> <td>goto [adr], if reg[A] == reg[B]</td> <td>Jumps to an address if the first register is equal to the other.</td> </tr>
	<tr> <td>0x74</td> <td>jge AB adr</td> <td>goto [adr], if reg[A] >= reg[B]</td> <td>Jumps to an address if the first register is greater than or equal to the other.</td> </tr>
	<tr> <td>0x75</td> <td>jg AB adr</td> <td>goto [adr], if reg[A] > reg[B]</td> <td>Jumps to an address if the first register is greater than the other.</td> </tr>
	<tr> <td>0x76</td> <td>jne AB adr</td> <td>goto [adr], if reg[A] != reg[B]</td> <td>Jumps to an address if the first register is not equal to the other.</td> </tr>
	<tr> <td>0x77</td> <td>skipIfNZ AB</td> <td>goto [reg[0xF] + B + 2], if reg[A] != 0</td> <td>Skips up to 15 following bytes if the register is not equal to zero.</td> </tr>
	<tr> <td>0x78</td> <td>skipIfZ AB</td> <td>goto [reg[0xF] + B + 2], if reg[A] == 0</td> <td>Skips up to 15 following bytes if the register is equal to zero.</td> </tr>
	<tr> <td>0x8R</td> <td>pushA</td> <td>push reg[A]</td> <td>Pushes a register onto the stack.</td> </tr>
	<tr> <td>0x9R</td> <td>popA</td> <td>pop reg[A]</td> <td>Pops a value off the stack and assigns to a register.</td> </tr>
	<tr> <td>0xA0</td> <td>add AB</td> <td>reg[A] += reg[B]</td> <td>Adds a register to another.</td> </tr>
	<tr> <td>0xA1</td> <td>add AB adr</td> <td>reg[A] = reg[B] + mem[adr]</td> <td>Adds a register and the value at a memory address and assigns to another register.</td> </tr>
	<tr> <td>0xA2</td> <td>add adr AB</td> <td>mem[adr] = reg[A] + reg[B]</td> <td>Adds two registers and stores to memory.</td> </tr>
	<tr> <td>0xA3</td> <td>add AB val</td> <td>reg[A] = reg[B] + val</td> <td>Adds a register and a value and assigns to a register.</td> </tr>
	<tr> <td>0xA4</td> <td>lea AB val</td> <td>reg[A] = B*val</td> <td>Loads the effective address of a scaled value into a register.</td> </tr>
	<tr> <td>0xA5</td> <td>lea AB val</td> <td>reg[A] = B + val</td> <td>Loads the effective address of an offset plus a value into a register.</td> </tr>
	<tr> <td>0xA6</td> <td>lea AB val1 val2</td> <td>reg[A] = B*val1+val2</td> <td>Loads the effective address of a scaled value plus a non-scaled value into a register.</td> </tr>
	<tr> <td>0xA7</td> <td>sub AB</td> <td>reg[A] -= reg[B]</td> <td>Subtracts a register from another.</td> </tr>
	<tr> <td>0xA8</td> <td>sub AB adr</td> <td>reg[A] = reg[B] - mem[adr]</td> <td>Subtracts the value at a memory address from a register and assigns to another register.</td> </tr>
	<tr> <td>0xA9</td> <td>sub AB adr</td> <td>reg[A] = mem[adr] - reg[B]</td> <td>Subtracts a register from the value at a memory address and assigns to another register.</td> </tr>
	<tr> <td>0xAA</td> <td>sub adr AB</td> <td>mem[adr] = reg[A] - reg[B]</td> <td>Subtracts a register from another and assigns to a memory address.</td> </tr>
	<tr> <td>0xAB</td> <td>sub AB val</td> <td>reg[A] = reg[B] - val</td> <td>Subtracts a register by a value and assigns to another register.</td> </tr>
	<tr> <td>0xAC</td> <td>sub AB val</td> <td>reg[A] = val - reg[B]</td> <td>Subtracts a value by a register and assigns to another register.</td> </tr>
	<tr> <td>0xC2</td> <td>call adr</td> <td>call [adr]</td> <td>Calls an address (Pushes the program counter onto the stack).</td> </tr>
	<tr> <td>0xC3</td> <td>ret</td> <td>return</td> <td>Returns (Pops into the program counter off the stack).</td> </tr>
	<tr> <td>0xD0</td> <td>nop</td> <td>continue</td> <td>Do nothing.</td> </tr>
	<tr> <td>0xE0</td> <td>putchar</td> <td>putchar(reg[0x1])</td> <td>Emulator call. Outputs a character stored in register 0x1.</td> </tr>
	<tr> <td>0xE1</td> <td>getchar</td> <td>reg[0x0] = getchar(int)</td> <td>Emulator call. Prompts for a input value which stored in an int then converted to a char.</td> </tr>
	<tr> <td>0xE2</td> <td>printstr char[],0x00</td> <td>puts(mem[++regs[0xF]]), while mem[regs[0xF]] != 0</td> <td>Emulator call. Prints out each character at the next location of memory until a null character.</td> </tr>
	<tr> <td>0xE8</td> <td>dumpregs[0x0]</td> <td>dumpRegs(0x0)</td> <td>Emulator call. Calls the emulator register dump function using 0x0 as the arguement.</td> </tr>
	<tr> <td>0xE9</td> <td>dumpregs[0x1]</td> <td>dumpRegs(0x1)</td> <td>Emulator call. Calls the emulator register dump function using 0x1 as the arguement.</td> </tr>
	<tr> <td>0xEA</td> <td>dumpMem</td> <td>dumpMemRange(0, 0xFF)</td> <td>Emulator call. Calls the emulator memory dump function.</td> </tr>
	<tr> <td>0xEB</td> <td>dumpMem</td> <td>dumpMemRange(regs[0x1], regs[0x2])</td> <td>Emulator call. Calls the emulator memory range dump function using registers 0x1 and 0x2 as arguements. Can print in ascending or descending order.</td> </tr>
	<tr> <td>0xEC</td> <td>dumpRegs</td> <td>dumpRegs(regs[0x1])</td> <td>Emulator call. Calls the emulator register dump function. Prints either an individual register or all registers in ascending order.</td> </tr>
	<tr> <td>0xED</td> <td>dump id</td> <td>dump(id)</td> <td>Emulator call. Calls the emulator dump function using id as the arguement. Can do a disabled dump.</td> </tr>
	<tr> <td>0xEE</td> <td>exit</td> <td>exit(reg[0x0])</td> <td>Emulator call. Exits the emulator, returning the value of register 0.</td> </tr>
	<tr> <td>0xEF</td> <td>fileDump</td> <td>fileDump()</td> <td>Emulator call. Prints out all of memory and all the registers in ascending order to a file "debug.txt" (for debugging).</td> </tr>
</table>

## Input File
The input file consists of machine language instructions, written with up to four instructions per line. Instructions should be encoded in hexadecimal bytes (i.e. ```0xAB 0xCD```) but can be written in octal (```0253 0315```) or decimal. Instructions larger than one byte will cause a runtime error. Anything after the read instructions is ignored and can be used for comments. Lines that do not start with integers are ignored.

The first instruction is read and loaded into the memory address of ```progstart```, which is the initial value of the program counter ```reg[0xF]```. Each additional instruction read is loaded into the subsequent (e.g. higher) memory address.

## ByteSyzed Class
The ByteSyzed Class has memory and registers stored as arrays of unsigned char. Every aspect of ByteSyzed is a public member. The program start ```progstart``` is where the first instruction is loaded (this is stored at the initial value of register 0xE). The bool ```prompt``` stores whether or not to display "```Enter value: ```" when getting input from getchar. The bool ```verbose``` stores whether or not to display instruction traces. The bool ```loadVerbose``` stores whether or not to display results from reading and loading from a file. 

## Debugging
The debugging suite is a bash shell script which loads and runs manaully-defined debugging files and prints out all memory and all registers to ```debug.txt``` and compares the file to the debugging files "theoretically correct" memory and registers using a Python script. Credit to the Python script goes to the illustrious and ubiquitous Jacob Butler. He is just happy to have sunk his pristine fingers into yet another grimy assembly course.

## Author Notes
The following is a summary of important addendeums:

 * Everything is a multiple of a byte (and thus stored in a ```char``` or ```char[]```), larger values will be modulo 256 (with exception to loading from files).
 * Instructions vary in size and format from one to four bytes. When writing in machine language, your program will go haywire if the instructions do not have the correct byte encoding. In other words, the CPU may expect following instruction bytes (that you possibly did not include) and, if it does, it expects them in a particular order (e.g. your byte encoding might be backwards).
 * As a general rule the encoding has the destination in the "left-most" memory slot after the opcode and the source(s) in the following "right" memory slot(s) sequentially.
 * The program counter ```reg[0xF]``` starts at ```progstart```code>, a public member of ```ByteSyzed```.
 * The stack pointer ```reg[0xE]``` initially points to the highest memory address whose value is ```progstart```.
 * The stack pointer ```reg[0xE]``` grows into lower memory (e.g. subtract to increase the size of the stack and vice versa).
 * When reading the input file, the first instruction is (basically) at ```progstart```. If there are ```n``` valid instructions (e.g. they will be read and loaded) in the input file, the final instruction loaded is located at ```(progstart+n-1)```.
 * There is no check currently to validate that the stack pointer ```reg[0xE]``` is not encroaching on loaded memory.
 * Doing an operation to the program counter ```reg[0xF]``` may have odd results. The program counter is incremented by the size (e.g. number of bytes) of the instruction in most cases.
 * It is possible (and easy) to have static local variables for functions.
 * ByteSyzed is small enough that bugs are easy enough to find by dumping the memory and the registers.

## More Information
To learn more about instruction set architecture and the benefits of focusing different aspects of them, read "Chapter 2: Instructions:
Language of the
Computer" of *Computer Organiztion and Design: The Hardware/Software (Fifth Edition)*, by D. Patterson and J. Hennessy.
