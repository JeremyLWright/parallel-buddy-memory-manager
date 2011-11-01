template <typename T>
class BuddySTLInterface {
    public:
        /* Types for the STL interface */
        typedef size_t size_type;
        typedef ptrdiff_t difference_type;
        typedef T* pointer;
        typedef T const * const_pointer;
        typedef T& reference;
        typedef T const & const_reference;
        typedef T value_type;  
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

    
    
    
    private:
 
 
