# PseudoRandomFunction
Implementation of MPC-friendly PRF
Candidate Weak PRF

A) Running the example:
Right now it is as simple as running a C++ program. The first input will be the value of security parameter. Right now enter it as 4 because the length of input is harcoded as a 4 bit value. This will change in future modification. The rest will be handled by the program which shows the output of intermediate functions.

B) Explanation:

The program contains three functions
1) main(): 
This function will run at the start of the program. Variables pertaining to security parameter is declared and input is taken from the user.
This function calls the generateRandom function. After receiving the random number from the generateRandom function, the main function calls the matrixCreation function and passes the value of that random number along with values of row and column of the matrix of secret key. The values of row and columns are function of the security parameter defined before.

2) generateRandom():
At first, this may seem a simple execution of rand() function. One can say without any doubt that rand() function generate pseudo random number, but it is not good for multiple execution, since it returns the same random number on multiple execution. One solution is to use srand which calculates the time from jan 1st 1970 in millisecond. This can be used as seed to generate the random number. This makes it extremely difficult if not impossible to guess the random number. The random number (unsigned long number) is sent back to main function which then calls the matrixCreation function.

3) matrixCreation():
This function first transforms the decimal number into binary by taking each number and finding it's mod 2 output. Right now, the program can print out atmost 31 bits of random digits. This will be changed with future modifications. These bits are then arranged in a matrix and this becomes our secret key. The secret key can be any mxn matrix, but right now the value of m and n are same, so the secret key is a square matrix. The input also at this time has the same size as that of the value of m(and n).

C) Coming up next:

1) Make the value of m and n different which will no longer keep the secret key as a square matrix
2) Take a custom length input length and figure out how it will be added. Maybe padding can be used.
3) Make a change in mapping the product of input and secret key as a mod 2 value vector which will be then subjected to an additive mod 3 operation.