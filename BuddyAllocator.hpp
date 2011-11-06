/**
 * @brief Parameterized Binary Buddy Allocator
 *
 * @par
 * Copyright Jeremy Wright (c) 2011
 * Creative Commons Attribution-ShareAlike 3.0 Unported License.
 */
#ifndef _BUDDYALLOCATOR
#define _BUDDYALLOCATOR

#include <stdint.h>
#include <cstddef>
#include <list>
#include <cmath>
#include <cassert>
#include <queue>
#include <pthread.h>
#include <algorithm>
#include <semaphore.h>
#include <iostream>
using namespace std;
using std::queue;
using std::list;
using std::min;
using std::find;
/**
 * @warning This class is may not be used as a base class.
 */
template <typename Block, size_t freeListOrder=7>
class BuddyAllocator {
    friend class TestBuddyAllocator;
    public:
        // The enum is just a way to get the size value at compile time instead
        // of at runtime. 
        enum compileConstants {
            blockSize=sizeof(Block)
        };
         
        typedef Block* BlockPtr;
        typedef Block const * BlockConstPtr;
        size_t numBlocks;

        BuddyAllocator()
        {
            numBlocks = static_cast<size_t>(pow(2,freeListOrder));

            memoryPool = new Block[numBlocks];
            freeList[freeListOrder].freeBlocks.push_back(memoryPool);
        }

      
        ~BuddyAllocator()
        {
        }

        // Return the maximum allocatable size
        size_t max_size() const
        {
            return sizeof(memoryPool);
        }

        size_t max_blocks() const
        {
            return  numBlocks; 
        }

        //Allocate but do not initialize num elements of type T
        BlockPtr allocate(size_t num, BlockConstPtr hint = 0)
        {
            BlockPtr p = 0;
            allocateBlock(p, size_to_level(num));
            assert(p != 0);
            return p;
        }

       //deallocate storage p of deleted items
       void deallocate(BlockPtr p, size_t num)
       {
           releaseBlock(p, size_to_level(num));
       }

    private:
       //Make the Buddy Allocator uncopyable
        template <typename U>
            BuddyAllocator(const BuddyAllocator<U, freeListOrder>&) throw()
            {
            }
 
        BuddyAllocator(BuddyAllocator const &) throw()
        {
        }

        inline size_t size_to_level(size_t size)
        {
            size_t level = log(size)/log(2);
            assert(level <= freeListOrder);
            return level;
        }

        inline size_t level_to_size(size_t level)
        {
            size_t size = pow(2, level);
            assert(level <= freeListOrder);
            return size;
        }

        BlockPtr memoryPool;

        struct MemoryRequest {
            MemoryRequest()
            {
                sem_init(&wait, 0, 1);
                request = NULL;
            }
            sem_t wait;
            BlockPtr request;
        };
      
        struct freeListHead {
            freeListHead()
            {
                Nrequested = 0;
                pthread_mutex_init(&lock, NULL);
            }
            size_t Nrequested;
            queue<MemoryRequest*> waitingRequests;
            list<BlockPtr> freeBlocks;
            pthread_mutex_t lock;
        };

        freeListHead freeList[freeListOrder+1];

        inline bool buddyIsFree(BlockPtr& M, BlockPtr& Buddy, size_t level)
        {
            ///TODO: Is there a cleaner way to do this, and avoid the casts?
            //static_cast<size_t>, doesn't work
            BlockPtr BuddySA = (BlockPtr)((size_t)M ^ level_to_size(level));
            if(find(freeList[level].freeBlocks.begin(), 
                        freeList[level].freeBlocks.end(), BuddySA) 
                    != freeList[level].freeBlocks.end())
            {
                Buddy = BuddySA;
                return true;
            }
            return false;
        }

        void lock(size_t level)
        {
            pthread_mutex_lock(&freeList[level].lock);
        }

        void unlock(size_t level)
        {
            pthread_mutex_unlock(&freeList[level].lock);
        }

        void allocateBlock(BlockPtr& p, size_t level)
        {
            if(level > freeListOrder)
            {
                //Clean up the freeList requests before we freak out
                for(size_t i = 0; i < freeListOrder; ++i)
                {
                    freeList[i].waitingRequests.pop();
                }
                throw std::bad_alloc();
            }
            lock(level);
            if(freeList[level].freeBlocks.empty())
            {
                //No blocks, so wait until some are available
                MemoryRequest selfPending;
                freeList[level].waitingRequests.push(&selfPending);
                if(freeList[level].waitingRequests.size() > freeList[level].Nrequested)
                {
                    freeList[level].Nrequested += 2;
                    unlock(level);
                    splitBlock(level + 1);
                    sem_wait(&selfPending.wait);
                    p = selfPending.request;
                }
            }
            else
            {
                p = freeList[level].freeBlocks.front();
                freeList[level].freeBlocks.pop_front();
                unlock(level);
            }
            if(p == 0)
                cerr << "Level: " << level << endl;
            assert(p != 0);
        }

        void releaseBlock(BlockPtr M, size_t level)
        {
            lock(level);
            if(freeList[level].waitingRequests.size() > 0)
            {
                //Give to the blocked request if possible
                MemoryRequest* P = freeList[level].waitingRequests.front();
                freeList[level].waitingRequests.pop();
                unlock(level);
                P->request = M;
                sem_post(&P->wait);
            }
            else
            {
                BlockPtr buddy = 0;
                if(buddyIsFree(M, buddy, level))
                {
                    //Coalesce
                    freeList[level].freeBlocks.remove(buddy);
                    unlock(level);
                    combine(M, buddy);
                    releaseBlock(min(M,buddy), level+1);
                }
                else
                {
                    //Don't combine
                    freeList[level].freeBlocks.push_back(M);
                    unlock(level);
                }
            }
        }

        BlockPtr combine(BlockPtr A, BlockPtr B)
        {
            return A;
        }

        void splitBlock(size_t level)
        {
            BlockPtr p = 0;
            allocateBlock(p, level);
            BlockPtr M = p;
            BlockPtr B = p+(level_to_size(level)/2);
            lock(level-1);
            freeList[level-1].Nrequested -= 2;
            /* Satisfying the request for 2 */
            if(freeList[level-1].waitingRequests.size() > 0)
            {
                MemoryRequest* remembered = freeList[level-1].waitingRequests.front();
                remembered->request = M;
                freeList[level-1].waitingRequests.pop();

                if(freeList[level-1].waitingRequests.size() >0)
                {
                    MemoryRequest* remembered = freeList[level-1].waitingRequests.front();
                    remembered->request = B;
                    freeList[level-1].waitingRequests.pop();
                }
                else
                {
                    freeList[level-1].freeBlocks.push_back(B); //Add B to the free list
                }
                unlock(level-1);
                sem_post(&remembered->wait);
            }
            else
            {
                cout << "I made it here" << endl;
                unlock(level-1);
                releaseBlock(combine(M,B), level);
            }

        }



};


#endif /* end of include guard: _BUDDYALLOCATOR */
