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
#if USE_BIG_LOCK
            pthread_mutex_init(&bigLock, 0);
#endif
            memoryPool = new Block[numBlocks];
            freeList[freeListOrder].addFreeBlock(memoryPool);
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
#if USE_BIG_LOCK
        pthread_mutex_t bigLock;
#endif
        //Allocate but do not initialize num elements of type T
        BlockPtr allocate(size_t num, BlockConstPtr hint = 0)
        {
            BlockPtr p = 0;
#if USE_BIG_LOCK
            pthread_mutex_lock(&bigLock);
            try
            {
#endif
            allocateBlock(p, size_to_level(num));
#if USE_BIG_LOCK
            } catch(std::bad_alloc& e)
            {

                pthread_mutex_unlock(&bigLock);
                throw e;
            }
            pthread_mutex_unlock(&bigLock);
#endif
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
            typedef MemoryRequest* Ptr;
            MemoryRequest()
            {
                sem_init(&wait, 0, 0);
                request = NULL;
            }
            sem_t wait;
            BlockPtr request;
        };
        
        class freeListHead {
            public:
                freeListHead()
                {
                    Nrequested = 0;
                    pthread_mutex_init(&levelLock, NULL);
                    pthread_mutex_init(&freeBlockLock, NULL);
                    pthread_mutex_init(&waitingRequestsLock, NULL);
                }
                size_t Nrequested;
                void addRequest(MemoryRequest* request)
                {
                    pthread_mutex_lock(&waitingRequestsLock);
                    waitingRequests.push(request);
                    pthread_mutex_unlock(&waitingRequestsLock);
                }

                MemoryRequest* getRequest()
                {
                    pthread_mutex_lock(&waitingRequestsLock);
                    MemoryRequest* request = waitingRequests.front();
                    waitingRequests.pop();
                    pthread_mutex_unlock(&waitingRequestsLock);
                    return request;
                }

                size_t pendingRequests()
                {
                    pthread_mutex_lock(&waitingRequestsLock);
                    size_t requests = waitingRequests.size();
                    pthread_mutex_unlock(&waitingRequestsLock);
                    return requests;

                }

                BlockPtr getFreeBlock()
                {
                    pthread_mutex_lock(&freeBlockLock);
                    BlockPtr freeBlock = freeBlocks.front();
                    freeBlocks.pop_front();
                    pthread_mutex_unlock(&freeBlockLock);
                    assert(freeBlock != 0);
                    return freeBlock;
                }

                void removeFreeBlock(BlockPtr block)
                {
                    pthread_mutex_lock(&freeBlockLock);
                    freeBlocks.remove(block);
                    pthread_mutex_unlock(&freeBlockLock);
                }

                bool freeBlockIsEmpty()
                {
                    pthread_mutex_lock(&freeBlockLock);
                    bool empty = freeBlocks.empty();
                    pthread_mutex_unlock(&freeBlockLock);
                    return empty;

                }

                bool findFreeBlock(BlockPtr freeBlock)
                {
                    bool found = false;
                    pthread_mutex_lock(&freeBlockLock);
                    if(find(freeBlocks.begin(), freeBlocks.end(), freeBlock) 
                            != freeBlocks.end())
                    {
                        found = true;
                    }
                    found = false;
                    pthread_mutex_unlock(&freeBlockLock);
                    return found;

                }

                void addFreeBlock(BlockPtr freeBlock)
                {
                    assert(freeBlock != 0);
                    pthread_mutex_lock(&freeBlockLock);
                    {
                        freeBlocks.push_back(freeBlock);
                    }
                    pthread_mutex_unlock(&freeBlockLock);
                }  
                void lock()
                {
                    pthread_mutex_lock(&levelLock);
                }

                void unlock()
                {
                    pthread_mutex_unlock(&levelLock);
                }

            private:
                queue<MemoryRequest*> waitingRequests;
                list<BlockPtr> freeBlocks;
                pthread_mutex_t levelLock;
                pthread_mutex_t freeBlockLock; 
                pthread_mutex_t waitingRequestsLock;
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
                    freeList[level].Nrequested += 2;
                    freeList[level].unlock();
                    splitBlock(level + 1);
                    sem_wait(&selfPending.wait);
                    p = selfPending.request;
                }
            }
            else
            {
                p = freeList[level].getFreeBlock();
#if 1
                pthread_t id = pthread_self();
                stringstream s;
                s << "{ " << __LINE__ << "[" << id << "] Level: " << level << " "<< p << "}" << endl;
                cerr << s.str();
#endif
                freeList[level].unlock();
            }
#ifndef NDEBUG
            if(p == 0)
            {
                pthread_t id = pthread_self();
                stringstream s;
                s << "{ " << __LINE__ << "[" << id << "] Level: " << level << " "<< p << "}" << endl;
                cerr << s.str();
            }
#endif
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
                P->request = M;
                sem_post(&P->wait);
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
                MemoryRequest* remembered = freeList[level-1].getRequest();
                remembered->request = M;

                if(freeList[level-1].pendingRequests() > 0)
                {
                    MemoryRequest* remembered = freeList[level-1].getRequest();
                    remembered->request = B;
                }
                else
                {
                    freeList[level-1].addFreeBlock(B); //Add B to the free list
                }
                freeList[level-1].unlock();
                sem_post(&remembered->wait);
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
