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
        BuddyAllocator()
        {
            memoryPool = new uint8_t[blockSize*numBlocks];
        }
    protected:
        uint8_t* memoryPool;
    private:

};


#endif /* end of include guard: _BUDDYALLOCATOR */
