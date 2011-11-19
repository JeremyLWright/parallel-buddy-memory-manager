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
#include <sstream>
using namespace std;
using std::queue;
using std::list;
using std::min;
using std::find;
#ifdef SPIN_LOCK
#pragma message "Building with Spin Locks."
#endif
namespace {

    inline void plock(pthread_spinlock_t* lock)
    {
        pthread_spin_lock(lock);
    }

    inline void plock(pthread_mutex_t* lock)
    {
        pthread_mutex_lock(lock);
    }

    inline void punlock(pthread_spinlock_t* lock)
    {
        pthread_spin_unlock(lock);
    }

    inline void punlock(pthread_mutex_t* lock)
    {
        pthread_mutex_unlock(lock);
    }

    void pinit(pthread_mutex_t* lock)
    {
        pthread_mutex_init(lock, NULL);
    }

    void pinit(pthread_spinlock_t* lock)
    {
        pthread_spin_init(lock, NULL);
    }
}

/**
 * @warning This class is may not be used as a base class.
 * This classes allocates 2^freeListOrder*sizeof(Block) bytes to be allocated
 * across multiple processes. This allocator is threadsafe.
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
        freeList[freeListOrder].addFreeBlock(memoryPool);
    }


    ~BuddyAllocator()
    {
    }

    // Return the maximum allocatable size
    size_t max_size() const
    {
        return blockSize*numBlocks;
    }

    // Return the maximum number of blocks.
    size_t max_blocks() const
    {
        return numBlocks; 
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
#ifdef INSTRUMENT
    void getRequestsHighWater(vector<int>& requests)
    {
        for(int i = 0; i < freeListOrder; ++i)
        {
            requests.push_back(freeList[i].pendingRequestsMax);
        }
    }
#endif
    private:
    //Make the Buddy Allocator uncopyable
    template <typename U>
        BuddyAllocator(const BuddyAllocator<U, freeListOrder>&) 
        {
        }
    /**
     * Copying the Buddy Allocator doesn't make sense. The pointers are
     * intimately tied to the caller.
     */ 
    BuddyAllocator(BuddyAllocator const &)
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

    class MemoryRequest {
        public:
            MemoryRequest()
            {
                pinit(&pendingRequest);
                request = NULL;
            }
            void putBlock(BlockPtr block)
            {
                assert(block != 0);
                request = block;
                punlock(&pendingRequest);
            }

            BlockPtr getBlock()
            {
                plock(&pendingRequest);

                if(request == 0) ///TODO Why did this need a double lock?
                {
                    plock(&pendingRequest);
                }
                assert(request != 0);
                return request;
            }

        private:
#ifdef SPIN_LOCK
            pthread_spinlock_t pendingRequest;
#else
            pthread_mutex_t pendingRequest;
#endif
            BlockPtr request;
    };

    class freeListHead {
        public:
            freeListHead()
            {
                Nrequested = 0;
                pinit(&levelLock);
                pinit(&freeBlockLock);
                pinit(&waitingRequestsLock);
#ifdef INSTRUMENT
                pendingRequestsMax = 0;
#endif
            }
            size_t Nrequested;
            void addRequest(MemoryRequest* request)
            {
                plock(&waitingRequestsLock);
                waitingRequests.push(request);
                punlock(&waitingRequestsLock);
            }

            MemoryRequest* getRequest()
            {
                plock(&waitingRequestsLock);
#ifdef INSTRUMENT
                if(pendingRequestsMax < waitingRequests.size())
                    pendingRequestsMax = waitingRequests.size();
#endif
                MemoryRequest* request = waitingRequests.front();
                waitingRequests.pop();
                punlock(&waitingRequestsLock);
                return request;
            }

            size_t pendingRequests()
            {
                plock(&waitingRequestsLock);

                size_t requests = waitingRequests.size();
                punlock(&waitingRequestsLock);
                return requests;

            }

            BlockPtr getFreeBlock()
            {
                plock(&freeBlockLock);
                BlockPtr freeBlock = freeBlocks.front();
                freeBlocks.pop_front();
                punlock(&freeBlockLock);
                assert(freeBlock != 0);
                return freeBlock;
            }

            void removeFreeBlock(BlockPtr block)
            {
                plock(&freeBlockLock);
                freeBlocks.remove(block);
                punlock(&freeBlockLock);
            }

            bool freeBlockIsEmpty()
            {
                plock(&freeBlockLock);
                bool empty = freeBlocks.empty();
                punlock(&freeBlockLock);

                return empty;

            }

            bool findFreeBlock(BlockPtr freeBlock)
            {
                bool found = false;
                plock(&freeBlockLock);
                if(find(freeBlocks.begin(), freeBlocks.end(), freeBlock) 
                        != freeBlocks.end())
                {
                    found = true;
                }
                found = false;
                punlock(&freeBlockLock);
                return found;

            }

            void addFreeBlock(BlockPtr freeBlock)
            {
                assert(freeBlock != 0);
                plock(&freeBlockLock);
                {
                    freeBlocks.push_back(freeBlock);
                }
                punlock(&freeBlockLock);
            }  
            void lock()
            {
                plock(&levelLock);
            }

            void unlock()
            {
                punlock(&levelLock);
            }

#ifdef INSTRUMENT
            size_t pendingRequestsMax;
#endif
        private:
            queue<MemoryRequest*> waitingRequests;
            list<BlockPtr> freeBlocks;
#ifdef SPIN_LOCK
            pthread_spinlock_t levelLock;
            pthread_spinlock_t freeBlockLock; 
            pthread_spinlock_t waitingRequestsLock;
#else
            pthread_mutex_t levelLock;
            pthread_mutex_t freeBlockLock; 
            pthread_mutex_t waitingRequestsLock;
#endif
    };

    freeListHead freeList[freeListOrder+1];

    bool buddyIsFree(BlockPtr& M, BlockPtr& Buddy, size_t level)
    {
        ///TODO: Is there a cleaner way to do this, and avoid the casts?
        //static_cast<size_t>, doesn't work
        BlockPtr BuddySA = (BlockPtr)((size_t)M ^ level_to_size(level));
        if(freeList[level].findFreeBlock(BuddySA))
        {
            Buddy = BuddySA;
            return true;
        }
        return false;
    }


    void allocateBlock(BlockPtr& p, size_t level)
    {
        bool dosplit = false;
        if(level > freeListOrder)
        {
            //Clean up the freeList requests before we freak out
            for(size_t i = 0; i < freeListOrder; ++i)
            {
                freeList[i].getRequest();
            }
            throw std::bad_alloc();
        }
        freeList[level].lock();
        if(freeList[level].freeBlockIsEmpty())
        {
            //No blocks, so wait until some are available
            MemoryRequest selfPending;
            freeList[level].addRequest(&selfPending);
            if(freeList[level].pendingRequests() > freeList[level].Nrequested)
            {
                dosplit = true;
                freeList[level].Nrequested += 2;
            }
            freeList[level].unlock();
            if(dosplit)
                splitBlock(level + 1);
            p = selfPending.getBlock();
        }
        else
        {
            p = freeList[level].getFreeBlock();
            freeList[level].unlock();
        }
        assert(p != 0);
    }

    void releaseBlock(BlockPtr M, size_t level)
    {
        freeList[level].lock();
        if(freeList[level].pendingRequests() > 0)
        {
            //Give to the blocked request if possible
            MemoryRequest* P = freeList[level].getRequest();
            freeList[level].unlock();
            P->putBlock(M);
        }
        else
        {
            BlockPtr buddy = 0;
            if(buddyIsFree(M, buddy, level))
            {
                //Coalesce
                freeList[level].removeFreeBlock(buddy);
                freeList[level].unlock();
                combine(M, buddy);
                releaseBlock(min(M,buddy), level+1);
            }
            else
            {
                //Don't combine
                freeList[level].addFreeBlock(M);
                freeList[level].unlock();
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
        freeList[level-1].lock();
        freeList[level-1].Nrequested -= 2;
        /* Satisfying the request for 2 */
        if(freeList[level-1].pendingRequests() > 0)
        {
            freeList[level-1].getRequest()->putBlock(M);

            if(freeList[level-1].pendingRequests() > 0)
            {
                freeList[level-1].getRequest()->putBlock(B);
            }
            else
            {
                freeList[level-1].addFreeBlock(B); //Add B to the free list
            }
            freeList[level-1].unlock();
        }
        else
        {
            cout << "I made it here" << endl;
            freeList[level-1].unlock();
            releaseBlock(combine(M,B), level);
        }

    }



};


#endif /* end of include guard: _BUDDYALLOCATOR */
