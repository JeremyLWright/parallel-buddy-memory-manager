/**
 * @brief Unit Tests for BuddyAllocator
 *
 * @par
 * Copyright Jeremy Wright (c) 2011
 * Creative Commons Attribution-ShareAlike 3.0 Unported License.
 */
#include <gtest/gtest.h>
#include "BuddyAllocator.hpp"
#include <list>
#include <fstream>
#include <boost/timer/timer.hpp>
using std::list;
class Instrument : public ::testing::Test {
    protected:
        virtual void SetUp()
        {
        }

        virtual void TearDown()
        {
        }

        BuddyAllocator<int> myAlloc;
};

TEST_F(Instrument, AllocationsPerSecond)
{
    ofstream f("BuddyAllocsPerSecond.m");
    for(int i = 0; i < 5; ++i)
    {
        BuddyAllocator<int, 15> myAlloc;
        boost::timer::cpu_timer t;
        t.start();
        for(int j = 0; j < myAlloc.max_blocks()-1; ++j)
            myAlloc.allocate(1);
        t.stop();
        f << myAlloc.max_blocks() << " ";
        f << t.format(6, "%w");
        f << endl;
    }
    EXPECT_EQ(1,1);
}

TEST_F(Instrument, BoolAllocationsPerSecond)
{
    ofstream f("BuddyBoolAllocsPerSecond.m");
    for(int i = 0; i < 5; ++i)
    {
        BuddyAllocator<bool, 15> myAlloc;
        boost::timer::cpu_timer t;
        t.start();
        for(int j = 0; j < myAlloc.max_blocks()-1; ++j)
            myAlloc.allocate(1);
        t.stop();
        f << myAlloc.max_blocks() << " ";
        f << t.format(6, "%w");
        f << endl;
    }
    EXPECT_EQ(1,1);
}

TEST_F(Instrument, PlatformSpeed)
{
    ofstream f("PlatformSpeed.m");
    int* thing[32768];
    for(int i = 0; i < 5; ++i)
    {
        boost::timer::cpu_timer t;
        t.start();
        for(int j = 0; j < 32768; ++j)
            thing[j] = new int;
        t.stop();
        f << 32768 << " ";
        f << t.format(6, "%w");
        f << endl;
    }
    EXPECT_EQ(1,1);

}




