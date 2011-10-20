/**
 * @brief Templated Binary Buddy Allocator
 *
 * @par
 * Copyright Jeremy Wright (c) 2011
 * Creative Commons Attribution-ShareAlike 3.0 Unported License.
 */
#ifndef _BUDDYALLOCATOR
#define _BUDDYALLOCATOR

#include <stdint.h>
#include <cstddef>

template <typename T, size_t numBlocks=200>
class BuddyAllocator {
    public:
        enum compileConstants {blockSize=sizeof(T)};
        
        struct Block {
            uint8_t padding[blockSize];
        };

        typedef Block* BlockPtr;

        BuddyAllocator()
        {
            memoryPool = new uint8_t[sizeof(Block)*numBlocks];
        }
    protected:
        BlockPtr memoryPool;
    private:

};


#endif /* end of include guard: _BUDDYALLOCATOR */
