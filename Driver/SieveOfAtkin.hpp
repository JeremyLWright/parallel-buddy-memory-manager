/**
 * @brief Prime Number Generator using Sieve of Atkin
 *
 * @par
 * Copyright Jeremy Wright (c) 2011
 * Creative Commons Attribution-ShareAlike 3.0 Unported License.
 */
#ifndef _SIEVEOFATKIN
#define _SIEVEOFATKIN

#include <iostream>
#include <limits.h>
#include <math.h>

using namespace std;
class SieveOfAtkin
{
public:
    SieveOfAtkin(const unsigned);
    unsigned next();
protected:
    bool getBit(const unsigned);
    void onBit(const unsigned);
    void offBit(const unsigned);
    void switchBit(const unsigned);
    unsigned char* sieve;
    unsigned sievelen;
    unsigned limit;
    unsigned mark;
    void firstPass();
};


#endif /* end of include guard: _SIEVEOFATKIN */
