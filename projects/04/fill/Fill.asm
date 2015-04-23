// This file is part of www.nand2tetris.org
// and the book "The Elements of Computing Systems"
// by Nisan and Schocken, MIT Press.
// File name: projects/04/Fill.asm

// Runs an infinite loop that listens to the keyboard input. 
// When a key is pressed (any key), the program blackens the screen,
// i.e. writes "black" in every pixel. When no key is pressed, the
// program clears the screen, i.e. writes "white" in every pixel.

// Put your code here.
(LOOP)
@KBD // Check the keyboard
D=M // Set the value in the keyboard equal to register D
@BLACKINIT
D;JGT // Jump to blackinit if there is something in register D
@WHITEINIT
D;JEQ // Jump to whiteinit if there is nothing in register D

(BLACKINIT)
@SCREEN // Set the RAM[i] location equal to the value of the address of the screen
D=A
@i
M=D
@BLACK // jump to the code that blackens the screen
0;JMP

(BLACK)
@i // Set the A register equal to the address of the screen
A=M
M=-1 // Blacken the 16 pixels associated with the RAM location by setting each pixel to 1
@i
M=M+1 // Increment i to the next screen location
@KBD // KBD is the last location in ROM, which is also the location after the last pixel on the screen
D=A
@i
D=D-M // Check if i is at the KBD location; last location in ROM
@LOOP
D;JEQ // Jump back to loop if it is because we have filled the whole screen as black
@BLACK 
0;JMP // else, jump back to the beginning of BLACK

(WHITEINIT) // does the exact same thing as the BLACKINIT code
@SCREEN
D=A
@i
M=D
@WHITE
0;JMP

(WHITE) // does the same thing as the BLACK code except, we now set each pixel to 0, meaning white
@i
A=M
M=0
@i
M=M+1
@KBD
D=A
@i
D=D-M
@LOOP
D;JEQ
@WHITE
0;JMP