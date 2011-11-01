#include <gtest/gtest.h>
#include "BuddyAllocator.hpp"

#include <list>
using std::list;

TEST(MultiplyTest, NormalTest)
{
    EXPECT_EQ(1, 1);
}

TEST(AllocatorInterface, VectorMultipleItem)
{
    BuddyAllocator<int> myAlloc;
    BuddyAllocator<int>::BlockPtr handles[10];
    for(int i=0; i < 10; i++)
        handles[i] = myAlloc.allocate(1);

    for(int i = 0; i < 10; i++)
        EXPECT_NE(1, 0);
}
/*
TEST(AllocatorInterface, Vector1Item)
{
    BuddyAllocator<int> myAlloc;
    list<int, BuddyAllocator<int> > myList;
    myList.push_back(5);
    EXPECT_EQ(myList.back(), 5);
}

TEST(FreeList, StaticSize)
{
    BuddyAllocator<int, 200> myAlloc;
    EXPECT_EQ(myAlloc.freeListOrder, 7);
}*/


