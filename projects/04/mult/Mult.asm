// This file is part of www.nand2tetris.org
// and the book "The Elements of Computing Systems"
// by Nisan and Schocken, MIT Press.
// File name: projects/04/Mult.asm

// Multiplies R0 and R1 and stores the result in R2.
// (R0, R1, R2 refer to RAM[0], RAM[1], and RAM[2], respectively.)

// Put your code here.
@0
D=M
@ZERO
D;JEQ // jump to zero if R0 is equal to 0
@1
D=M
@ZERO
D;JEQ // jump to zero if R1 is equal to 0
@1
D=M
@3
M=D // R3=R1
@0
D=M 
@2
M=D // Going to add R0+R0, R1 times and store in R2
@3
MD=M-1 // Using R3 as keeping track of the number of times adding
(LOOP)
	@END
	D;JEQ // jump to end if R3 == 0
	@0
	D=M
	@2
	M=M+D // incrementing R2 by R2+R0
	@3
	MD=M-1
	@LOOP
	0;JMP
(ZERO)
	@0
	D=A
	@2
	M=D // R2 = 0
(END)
	@END
	0;JMP