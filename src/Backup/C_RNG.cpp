#include "C_RNG.h"
#include <string.h>

//-------------------------------------------------
// Seed
// Used to define the seed.
// Must be called before RandomNumber() is called
//-------------------------------------------------
void C_RNG::seed(unsigned long sd) {
	d_lSeed = sd;

	unsigned short sNumber = sd & MASK16;   // The low 16 bits
    unsigned long number = sd & MASK31;   // Only want 31 bits

    short *p = d_mother1;
    for(int n = 18; n--; )  {
        number = 30903 * sNumber + ( number >> 16 );   
        *p++ = sNumber = number & 0xFFFF;
        if ( n == 9 )
                p = d_mother2;
    }
    // make cary 15 bits
    d_mother1[0] &= MASK15;
    d_mother2[0] &= MASK15;
}

//-------------------------------------------------
// RandomNumber
// Generates a random number between 0.0 and 1.0
//-------------------------------------------------
double C_RNG::randomNumber() {
	// Move elements 1 to 8 to 2 to 9
    memcpy((char*)d_mother1 + 2,(char*)d_mother1 + 1, 8 * sizeof(short));
    memcpy((char*)d_mother2 + 2,(char*)d_mother2 + 1, 8 * sizeof(short));

    // Put the carry values in numberi
    unsigned long number1 = d_mother1[0];
    unsigned long number2 = d_mother2[0];

    // Form the linear combinations
	number1 += 1941 * d_mother1[2] + 1860 * d_mother1[3] +
		1812 * d_mother1[4] + 1776 * d_mother1[5] +
  		1492 * d_mother1[6] + 1215 * d_mother1[7] + 
		1066 * d_mother1[8] + 12013 * d_mother1[9];

	number2 += 1111 * d_mother2[2] + 2222 * d_mother2[3] + 
		3333 * d_mother2[4] + 4444 * d_mother2[5] + 
		5555 * d_mother2[6] + 6666 * d_mother2[7] + 
		7777 * d_mother2[8] + 9272 * d_mother2[9];

    // Save the high bits of numberi as the new carry
	d_mother1[0] = (short)(number1 / LONG16);
    d_mother2[0] = (short)(number2 / LONG16);

    // Put the low bits of numberi into motheri[1]
    d_mother1[1] = MASK16 & number1;
    d_mother2[1] = MASK16 & number2;

    // Combine the two 16 bit random numbers into one 32 bit
    d_lSeed = (((long)d_mother1[1]) << 16) + (long)d_mother2[1];

    // Return a double value between 0 and 1
    return ((double)d_lSeed) / DOUBLE32; 
}