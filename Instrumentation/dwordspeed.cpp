/**
 * @brief Instrumentation module for the Binary Buddy Allocator.
 *
 * @par
 * Copyright Jeremy Wright (c) 2011
 * Creative Commons Attribution-ShareAlike 3.0 Unported License.
 */

#include <fstream>
#include <sstream>
#include "BuddyAllocator.hpp"
#include <stdint.h>
#include <sys/time.h>
using namespace std;



int main(int argc, const char *argv[])
{
    ofstream f("dwordspeed.m");
    timeval startTimes[5];
    timeval endTimes[5];
    double elapsed;
    for(int i = 0; i < 5; ++i)
    {
        BuddyAllocator<uint64_t> Allocator;
        gettimeofday(&startTimes[i], NULL);
        for(int j = 0; j < Allocator.max_blocks()-1; ++j)
            Allocator.allocate(1);
        gettimeofday(&endTimes[i],NULL);
    }
    f << sizeof(uint64_t);
    for(int i = 0; i < 5; ++i)
        {
            elapsed = (endTimes[i].tv_sec-startTimes[i].tv_sec) * 1000.0;
            elapsed += (endTimes[i].tv_usec -startTimes[i].tv_usec) / 1000.0;
            f << " " << elapsed;
        }
    f << endl;
    return 0;
}
