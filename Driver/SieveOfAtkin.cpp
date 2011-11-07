#include "SieveOfAtkin.hpp"
namespace {
static const unsigned short DWORD_BITS = 8;

unsigned char flag(const unsigned char bit_index)
{
    return ((unsigned char) 128) >> bit_index;
}

}
SieveOfAtkin::SieveOfAtkin(const unsigned max_num)
{
    limit = max_num;
    sievelen = limit / DWORD_BITS + 1;
    mark = 0;

    sieve = new unsigned char[sievelen];
    for (unsigned i = 0; i < sievelen; i++)
    {
        sieve[i] = 0;
    }

    firstPass();
}


inline bool SieveOfAtkin::getBit(const unsigned index)
{
    return sieve[index/DWORD_BITS] & flag(index%DWORD_BITS);
}


inline void SieveOfAtkin::onBit(const unsigned index)
{
    sieve[index/DWORD_BITS] |= flag(index%DWORD_BITS);
}


inline void SieveOfAtkin::offBit(const unsigned index)
{
    sieve[index/DWORD_BITS] &= ~flag(index%DWORD_BITS);
}


inline void SieveOfAtkin::switchBit(const unsigned index)
{
    sieve[index/DWORD_BITS] ^= flag(index%DWORD_BITS);
}


void SieveOfAtkin::firstPass()
{
    unsigned nmod,n,x,y,xroof, yroof;

    //n = 4x^2 + y^2
    xroof = (unsigned) sqrt(((double)(limit - 1)) / 4);
    for (x = 1; x <= xroof; x++)
    {
        yroof = (unsigned) sqrt((double)(limit - 4 * x * x));
        for (y = 1; y <= yroof; y++)
        {
            n = (4 * x * x) + (y * y);
            nmod = n % 12;
            if (nmod == 1 || nmod == 5)
            {
                switchBit(n);
            }
        }
    }

    xroof = (unsigned) sqrt(((double)(limit - 1)) / 3);
    for (x = 1; x <= xroof; x++)
    {
        yroof = (unsigned) sqrt((double)(limit - 3 * x * x));
        for (y = 1; y <= yroof; y++)
        {
            n = (3 * x * x) + (y * y);
            nmod = n % 12;
            if (nmod == 7)
            {
                switchBit(n);
            }
        }
    }

    xroof = (unsigned) sqrt(((double)(limit + 1)) / 3);
    for (x = 1; x <= xroof; x++)
    {
        yroof = (unsigned) sqrt((double)(3 * x * x - 1));
        for (y = 1; y <= yroof; y++)
        {
            n = (3 * x * x) - (y * y);
            nmod = n % 12;
            if (nmod == 11 && x > y)
            {
                switchBit(n);
            }
        }
    }
}


unsigned SieveOfAtkin::next()
{
    while (mark <= limit)
    {
        mark++;

        if (getBit(mark))
        {
            unsigned out = mark;

            for (unsigned num = mark * 2; num <= limit; num += mark)
            {
                offBit(num);
            }
            return out;
        }
    }

    return 0;
}



