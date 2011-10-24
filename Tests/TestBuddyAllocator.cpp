#include <gtest/gtest.h>
#include "BuddyAllocator.hpp"

#include <vector>
using std::vector;

TEST(MultiplyTest, NormalTest)
{
    EXPECT_EQ(1, 1);
}

TEST(AllocatorInterface, VectorMultipleItem)
{
    BuddyAllocator<int> myAlloc;
    vector<int, BuddyAllocator<int> > myVec;
    for(int i=0; i < 10; i++)
        myVec.push_back(i);
    for(int i=0; i < 10; i++)
        EXPECT_EQ(myVec[i], i);
}
TEST(AllocatorInterface, Vector1Item)
{
    BuddyAllocator<int> myAlloc;
    vector<int, BuddyAllocator<int> > myVec;
    myVec.push_back(5);
    EXPECT_EQ(myVec.back(), 5);
}

TEST(FreeList, StaticSize)
{
    BuddyAllocator<int, 200> myAlloc;
    EXPECT_EQ(myAlloc.freeListOrder, 7);
}


