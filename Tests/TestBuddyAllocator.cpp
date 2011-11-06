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
    EXPECT_EQ(1, 1);
}

TEST_F(TestBuddyAllocator, VectorMultipleItem)
{
    BuddyAllocator<int, 7>::BlockPtr NulBlockPtr = NULL;
    int number = 128;
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

TEST_F(TestBuddyAllocator, MaxAllocation)
{
    BuddyAllocator<int, 10> LargeAlloc;

    BuddyAllocator<int, 10>::BlockPtr NulBlockPtr = NULL;
    int number = static_cast<int>(pow(2,10));
    BuddyAllocator<int, 10>::BlockPtr handles[number];
    for(int i=0; i < number; i++)
    {
        handles[i] = LargeAlloc.allocate(1);
        ASSERT_NE(handles[i], NulBlockPtr);
        *(handles[i]) = i;
    }

    for(int i = 0; i < number; i++)
    {
        EXPECT_EQ(*(handles[i]), i);
    }
    
    EXPECT_THROW(LargeAlloc.allocate(1), std::bad_alloc);

}

TEST_F(TestBuddyAllocator, Deallocate)
{
    BuddyAllocator<int, 10> LargeAlloc;

    BuddyAllocator<int, 10>::BlockPtr NulBlockPtr = NULL;
    int number = static_cast<int>(pow(2,10));
    BuddyAllocator<int, 10>::BlockPtr handles[number];
    for(int i=0; i < number; i++)
    {
        handles[i] = LargeAlloc.allocate(1);
        ASSERT_NE(handles[i], NulBlockPtr);
        *(handles[i]) = i;
    }

    for(int i = 0; i < number; i++)
    {
        EXPECT_EQ(*(handles[i]), i);
    }
    
    EXPECT_THROW(LargeAlloc.allocate(1), std::bad_alloc); //Max out the allocator

    //Release 1 block
    LargeAlloc.deallocate(handles[0], 1);
    EXPECT_THROW(LargeAlloc.allocate(1), std::bad_alloc); //Max out the allocator

}

