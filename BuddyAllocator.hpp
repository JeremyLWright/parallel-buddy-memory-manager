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
#include <boost/integer/static_log2.hpp>
#include <queue>

using std::queue;
using std::list;

/**
 * @warning This class is may not be used as a base class.
 */
template <typename T, size_t numBlocks=200>
class BuddyAllocator {
    public:
        // The enum is just a way to get the size value at compile time instead
        // of at runtime. 
        enum compileConstants {
            blockSize=sizeof(T),
            freeListOrder = ((boost::static_log2<numBlocks>::value))
        };
         
        struct Block {
            uint8_t padding[blockSize];
        };

        /* Types for the STL interface */
        typedef size_t size_type;
        typedef ptrdiff_t difference_type;
        typedef T* pointer;
        typedef T const * const_pointer;
        typedef T& reference;
        typedef T const & const_reference;
        typedef T value_type;  
 
 
        typedef T* BlockPtr;
        typedef T const * BlockConstPtr;
        template <typename U>
            struct rebind{
                typedef BuddyAllocator<T, numBlocks> other;
            };

        pointer address(reference value) const
        {
            return &value;
        }
        
        const_pointer address(const_reference value) const
        {
            return &value;
        }
      
        BuddyAllocator() throw()
        {
            memoryPool = new T[numBlocks];
            freeList[freeListOrder].push_back(memoryPool);
            for(size_t i = 0; i < freeListOrder+1; ++i)
            {
                pthread_mutex_init(freeList[i].lock);
            } 
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

        template <typename U>
            BuddyAllocator(const BuddyAllocator<U, numBlocks>&) throw()
            {
            }

        ~BuddyAllocator()
        {
            delete [] memoryPool;
        }

        // Return the maximum allocatable size
        size_type max_size() const throw()
        {
            return sizeof(sizeof(Block)*numBlocks);
        }

        //Allocate but do not initialize num elements of type T
        pointer allocate(size_type num, const_pointer hint = 0)
        {
            BlockPtr p = 0;
            allocateBlock(p, size_to_level(num));
            return static_cast<pointer>(p);
        }

        //Initialize elements of allocated storage p with value 
        void construct(pointer p, const T& value)
        {
            new(static_cast<void*>(p))T(value);
        }

        //destroy elements f  initialized storage p
        void destroy(pointer p)
        {
            p->~T();
        }

       //deallocate storage p of deleted items
       void deallocate(pointer p, size_t num)
       {
           releaseBlock(static_cast<BlockPtr>(p), size_to_level(num));
       }

    private:
        BlockPtr memoryPool;
      
        struct freeListHead {
            size_t Nrequested;
            queue<BlockPtr> waitingRequests;
            list<BlockPtr> freeBlocks;
            pthread_mutex_t lock;
        };

        freeListHead freeList[freeListOrder+1];

        inline bool buddyIsFree(BlockPtr M, BlockPtr& Buddy, size_t level)
        {
            return true;
        }

        void allocateBlock(BlockPtr& p, size_t level)
        {
            assert(level <= freeListOrder);
            bool doSplit = false;
            //lock freelist[i]
            if(freeList[level].freeBlocks.empty())
            {
                //No blocks, so wait until some are available
                freeList[level].waitingRequests.push_back(*p);
                if(freeList[level].waitingRequests.size() > freeList[level].Nrequested)
                {
                    doSplit = true;
                    freeList[level].Nrequested += 2;
                }
                //unlock freelist i
                if(doSplit)
                {
                    splitBlock(level + 1);
                    //sleep
                }
            }
            else
            {
                //Get a block from the head of the free list
                //unlock freeList[level]
            }
        }

        void releaseBlock(BlockPtr M, size_t level)
        {
            //lock freelist level
            if(freeList[level].waitingRequests > 0)
            {
                //Give to the blocked request if possible
                BlockPtr P = freeList[level].waitingRequests.front();
                freeList[level].waitingRequests.pop_front();
                //unlock freelist level
                //wakeup P and give it M
            }
            else
            {
                BlockPtr buddy = 0;
                if(buddyIsFree(M, buddy, level))
                {
                    //remove buddy from free list
                    //unlock freelist[i]
                    //combine M and buddy
                    releaseBlock(min(M,buddy), level+1);
                }
                else
                {
                    //Don't combine
                    //add M to the free list
                    //unlock freelist[i]
                }
            }
        }

        void splitBlock(size_t level)
        {
            BlockPtr p = 0;
            allocateBlock(p, level);
            BlockPtr M = p;
            BlockPtr B = p+(level_to_size(level)/2);
            //lock free list-1
            freeList[level-1].Nrequested -= 2;
            /* Satisfying the request for 2 */
            if(freeList[level-1].waitingRequests.size() > 0)
            {
                BlockPtr p = freeList[level-1].waitingRequests.front();
                freeList[level-1].waitingRequests.pop_front();
                //Wake up P and give it M
                if(freeList[level-1].waitingRequests.size() >0)
                {
                    BlockPtr q = freeList[level-1].waitingRequests.front();
                    freeList[level-1].waitingRequests.pop_front();
                    //Wake up Q and give it B
                }
                else
                {
                    freeList[level-1].freeBlocks.push_back(B); //Add B to the free list
                }
                //unlock free list-1
                //Wake up remembered process
            }
            else
            {
                //unlock free list-1
                release(level, combine(M,B));
            }

        }


};


#endif /* end of include guard: _BUDDYALLOCATOR */
