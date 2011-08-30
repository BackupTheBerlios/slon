#pragma once

#include <mm_malloc.h>

namespace slon {

/** Allocate aligned data.
 * @size - size of the data forr allocaation in bytes.
 * @alignment - data alignment in bytes.
 */
inline void* align_alloc(size_t size, size_t alignment = 0x10)
{
#ifdef __GNUC__
    return _mm_malloc(size, alignment);
#else // MSVS
    return _aligned_malloc(size, alignment);
#endif
}

/** Free aligned data */
inline void align_free(void* data)
{
#ifdef __GNUC__
    _mm_free(data);
#else // MSVS
    _aligned_free(data);
#endif
}

/** Aligned allocator for pools. */
template<int align>
class aligned_allocator
{
public:
    typedef std::size_t       size_type;
    typedef std::ptrdiff_t    difference_type;

    static char* malloc(size_type size)
    { 
        return (char*)align_alloc(size, align);
    }

    static void free(void* block)
    { 
        align_free(block);
    }
};

} // namespace slon
