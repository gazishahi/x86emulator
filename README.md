# x86 Emulator

CSc 21000 - Assembly Programming
Project #1 - x86 Emulator
Fall 2021
Professor Michael Vulis
Name: Gazi Shahi

This is a simple x86 Emulator that was done in C++.
It is able to read .COM files and print out the values
of the registers.

It is limited in its functionality, and is able to
perform the following instructions:

* NOP
* ADD
* SUB
* MOV
* INC
* DEC
* LOOP

## Areas to Improve Upon

* Due to time restraints (of my own doing), the program is unable to perform all
the learned instructions so far in the course.
* Due to its limited functionality, any instruction outside of
the ones mentioned above will not work.
* Interrupts are not implemented with the exception of
int 20h to terminate the program.
* I attempted to implement int 21h to display some output, but upon
looking into the documentation for int 21h, I found that a lot more
cases needed to be taken into consideration in order to print an output.
As such, I have removed int 21h from my program for the time being.
* More instructions and interrupts will be implemented in future iterations
of this project.
* The efficiency of the code can be greatly improved upon. Much of the program
can be reduced via struct or class instead of relying on so many switch cases.

## Included Files

* GaziShahi_Project_1.cpp - This is the source file, containing all the code for my
emulator
* GSP1 - This is the executable. I compiled via g++ so running the program would require
the user to type ./GSP1 in a terminal
* EXAMPLE1.COM - This .COM file is meant to see if the ADD, SUB, and MOV instructions work
properly
* EXAMPLE2.COM - This .COM file is meant to see if the INC, DEC, MOV, and LOOP instructions work
properly
* EXAMPLE3.COM - This .COM file is meant to test if all the instructions I have implemented work
properly together
* README.md - Well, this is the README of course! You are reading it right now!

## Video Memory Implementation

In this second iteration of project 1, I implemented writing to video memory with
limited capabilities. The program is able to read .COM files and print out what is
written to video memory. The program is aimed to emulate an 80x25 screen, which is the
default resolution of standard DOS text mode. The following instructions were expanded
and implemented:

* XOR
* MOV

### Limitations

* The screen resolution is 80x25, which is 2000 pixels. Trying to output something that would take
more than 2000 pixels would result in the program not printing the output.
* The printReg() function is only for debugging purposes now. It has been commented out of the program
and needs to be uncommented in order to see the register values and make sure they are correct.
Uncommenting it and running the program would result in the output not displaying properly. This is because
the register values are printed after each instruction.
* The program prints the output from left to right, top to bottom. If the .COM file is supposed to print
in any other order, the result will likely not look like what it is supposed to.
* As a result of the aforementioned limitation, the screen cannot backtrack and go back to the starting
top left cell of the screen. The ncurses library could remedy this to some extent, as it would require
clearing the screen as well, which would not be ideal if we want to make sure the program works properly
(i.e. We would want to see if int 20h is reached, but if the screen is cleared we would not know if that
occurred).
* The colors from the output are printed via ANSI escape sequences. This limits the usage of our program to
UNIX terminals and any other platform that supports ANSI escape sequences. I used Ubuntu so running this
program was not an issue for me.
* The colors may appear slightly different than they do in emu8086. This is because I had to hand pick some
of the escape sequences for the different shades of colors as it was not clear to me which sequences were for
the shades. Regardless, the output should appear to be similar to that of emu8086.
* Speed was not taken into account here. As the program had been extended from ~1800 lines to ~4100 lines, the
compile time was noticeably slower. As mentioned in the previous iteration, the efficiency of the program
could have been greatly improved through the usage of a struct or class. Doing this would have been an
extensive effort that would likely require me to rework the entire program. Due to time constraints (from
other classes and from focusing on project 2), this was not feasible for me.

## New Files

### Video Memory Implementation Folder

* GaziShahi_Project_1_V2.cpp - This is the new source file that includes the video memory implementation
* GSP1_V2 - This is the new executable which includes the video memory implementation
* ABC.COM - This .COM file prints the ABCs with colored foreground and background
* COLORS.COM - This .COM file prints a block of 4 colors
* FILL.COM - This .COM file prints a block of red

### Output Screenshots Folder

**The screenshots here compare the output of my program to that of emu8086.**

* ABC.jpg - This screenshot compares the output from the ABC.COM file
* COLORS.jpg - This screenshot compares the output from the COLORS.COM file
* FILL.jpg - This screenshot compares the output from the FILL.COM file
