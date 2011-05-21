#ifndef __SLON_ENGINE_UTILITY_MEMORY_ALIGNED_HPP__
#define __SLON_ENGINE_UTILITY_MEMORY_ALIGNED_HPP__

#include <cassert>
#include <stdexcept>
#include "../../Config.h"
#include "aligned_allocator.hpp"

#ifdef new
#   pragma push_macro("new")
#   undef new
#   define _POP_NEW_MACRO
#endif

#ifdef delete
#   pragma push_macro("delete")
#   undef delete
#   define _POP_DELETE_MACRO
#endif

namespace slon {

/** Overloads operator new and delete for aligned classes. */
template<int align>
class aligned
{
public:
    void* operator new(size_t size)
    {
    #ifdef SLON_ENGINE_FORCE_DEBUG_NEW
        void* pData = debug_aligned_alloc(size, align);
    #else
        void* pData = align_alloc(size, align);
    #endif
        if (!pData) {
            throw std::bad_alloc();
        }

        return pData;
    }

    void* operator new [](size_t size)
    {
    #ifdef SLON_ENGINE_FORCE_DEBUG_NEW
        void* pData = debug_aligned_alloc(size, align);
    #else
        void* pData = align_alloc(size, align);
    #endif
        if (!pData) {
            throw std::bad_alloc();
        }

        return pData;
    }

    void* operator new(size_t /*size*/, void* where)
    {
        assert( reinterpret_cast<size_t>(where) % align == 0 && "Data is not properly aligned!" );
        return where;
    }

    void operator delete(void* pData)
    {
        if (pData) 
        {
        #ifdef SLON_ENGINE_FORCE_DEBUG_NEW
            debug_aligned_free(pData);
        #else        
            align_free(pData);
        #endif
        }
    }

    void operator delete [](void* pData)
    {
        if (pData) 
        {
        #ifdef SLON_ENGINE_FORCE_DEBUG_NEW
            debug_aligned_free(pData);
        #else        
            align_free(pData);
        #endif
        }
    }

    // actually we don't need storing delete, but MSVS produces warning
    void operator delete(void* /*pData*/, void* /*where*/) {}

protected:
    // Do not delete using ptr to Aligned
    inline ~aligned() {}
};

} // namespace slon

#ifdef _POP_NEW_MACRO
#   pragma pop_macro("new")
#endif
#ifdef _POP_DELETE_MACRO
#   pragma pop_macro("delete")
#endif

#endif // __SLON_ENGINE_UTILITY_MEMORY_ALIGNED_HPP__