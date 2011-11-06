#include <gtest/gtest.h>
#include "BuddyAllocator.hpp"

#include <list>
using std::list;
class TestBuddyAllocator : public ::testing::Test {
    protected:
        virtual void SetUp()
        {
        }

        virtual void TearDown()
        {
        }

        BuddyAllocator<int> myAlloc;
        BuddyAllocator<int, 1> shortAlloc;
        virtual void invariant()
        {
             //The freeBlocks have a maximum number of elements
            for(int i = 0; i < myAlloc.freeListOrder; ++i)
            {
                EXPECT_LE(myAlloc.freeList[i].freeBlocks.size(), i);
            }
        
        }
};

TEST_F(TestBuddyAllocator, SingleItem)
{

    BuddyAllocator<int, 1>::BlockPtr NulBlockPtr = NULL;
    BuddyAllocator<int, 1>::BlockPtr i = myAlloc.allocate(1);
    ASSERT_NE(i, NulBlockPtr);

    *i = 1;
    ASSERT_EQ(*i, 1);
}

TEST_F(TestBuddyAllocator, InitialFreeBlocks)
{
    //Initially there should only be 1 free block
}

TEST_F(TestBuddyAllocator, NormalTest)
{
    invariant();
    EXPECT_EQ(1, 1);
}

TEST_F(TestBuddyAllocator, VectorMultipleItem)
{
    BuddyAllocator<int>::BlockPtr NulBlockPtr = NULL;
    int number = 128;
    invariant();
    BuddyAllocator<int>::BlockPtr handles[number];
    for(int i=0; i < number; i++)
    {
        handles[i] = myAlloc.allocate(1);
        ASSERT_NE(handles[i], NulBlockPtr);
        *(handles[i]) = i;
    }

    for(int i = 0; i < number; i++)
    {
        EXPECT_EQ(*(handles[i]), i);
    }
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


