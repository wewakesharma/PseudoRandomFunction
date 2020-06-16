readme.txt

The function declaration has been moved to the header file named weak23prf.h
The file weak23prf.cpp contains the function definition
unit_test.cpp contains the code for the unit testing of weak 23 PRF.

Errors/Warning:

Ignore a path warning, it came up when the header file name was changed. It doesn't affect the working of the test and will be removed later.

**To run the unit testing, follow the following command**

1) $g++ -c unit_test.cpp weak23prf.cpp 

It compiles both files together and on successful compilation, outputs two .o file.


2) $g++ -o unit_test unit_test.o weak23prf.o

This command will link both the file and an output file is created


3) $./unit_test
