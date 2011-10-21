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

template <typename T, size_t numBlocks=200>
class BuddyAllocator {
    public:
        // The enum is just a way to get the size value at compile time instead
        // of at runtime. 
        enum compileConstants {blockSize=sizeof(T)};
         
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
 
 
        typedef Block* BlockPtr;
        typedef Block const * BlockConstPtr;
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
            memoryPool = new Block[numBlocks];
        }

        BuddyAllocator(BuddyAllocator const &) throw()
        {
        }

        template <typename U>
            BuddyAllocator(const BuddyAllocator<U, numBlocks>&) throw()
            {
            }

        virtual ~BuddyAllocator()
        {
        }

        // Return the maximum allocatable size
        size_type max_size() const throw()
        {
            return sizeof(sizeof(Block)*numBlocks);
        }

        //Allocate but do not initialize num elements of type T
        pointer allocate(size_type num, const_pointer hint = 0)
        {
            return static_cast<pointer>(::operator new(num*sizeof(T)));
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
           //Free memory back to the pool
       }

    protected:
        BlockPtr memoryPool;
    private:

};


#endif /* end of include guard: _BUDDYALLOCATOR */
