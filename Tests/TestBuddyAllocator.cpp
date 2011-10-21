#include <gtest/gtest.h>
#include "BuddyAllocator.hpp"

#include <vector>
using std::vector;

TEST(MultiplyTest, NormalTest)
{
    EXPECT_EQ(1, 1);
}

TEST(SanityCheck, BlockSize)
{
    BuddyAllocator<int> myAlloc;
    vector<int, BuddyAllocator<int> > myVec;
    myVec.push_back(5);
    EXPECT_EQ(myVec.back(), 5);
}


