//--------------------------------------------
// RandomNumberGenerator
// A class that generates a random number using
// the mother off all random number method
//--------------------------------------------

#ifndef RANDOMNUMBERGENERATOR
#define RANDOMNUMBERGENERATOR

#define LONG16 65536L			// 2^16
#define MASK16 0xFFFF          // mask for lower 16 bits
#define MASK15 0x7FFF          // mask for lower 15 bits
#define MASK31 0x7FFFFFFF      // mask for 31 bits
#define DOUBLE32  4294967295.0 // 2^32-1

class C_RNG {
private:
	short d_mother1[10];
	short d_mother2[10];
	unsigned long d_lSeed;

public:
	C_RNG() {
		d_lSeed = 0;
	}

	virtual ~C_RNG() {};
	double randomNumber();
	void seed(unsigned long sd);

};

#endif
