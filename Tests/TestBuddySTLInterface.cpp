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
    list<int, BuddyAllocator<int> > myList;
    for(int i=0; i < 10; i++)
        myList.push_back(i);
    int j = 0;
    for(list<int>::const_iterator i = myList.begin(); 
            i != myList.end(); 
            ++i, j++)
        EXPECT_EQ(*i, j);
}
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
}


