#ifndef __SLON_ENGINE_UTILITY_REFERENCED_HPP__
#define __SLON_ENGINE_UTILITY_REFERENCED_HPP__

#include "../Config.h"
#ifdef SLON_ENGINE_USE_SSE
#   include "Memory/aligned.hpp"
#endif

namespace slon {

class referenced
#ifdef SLON_ENGINE_USE_SSE
    // Just make sure that SSE vector operations will not fail
    : public aligned<0x10>
#endif
{
public:
    referenced() :
        refCount(0)
    {}

    referenced(const referenced& /*other*/) :
        refCount(0)
    {}

    // Don't modify reference counter
    referenced& operator = (const referenced& /*other*/)
    {
        return *this;
    }

    unsigned use_count() const 
    { 
        return refCount;
    }
    
    void add_ref() const
    {
        ++refCount;
    }

    void remove_ref() const
    {
        if (--refCount == 0) {
            delete this;
        }
    }

    virtual ~referenced() {}

protected:
    mutable int refCount;
};

typedef referenced Referenced;

} // namespace slon

namespace boost {

inline void intrusive_ptr_add_ref(const slon::referenced* ref) {
    ref->add_ref();
}

inline void intrusive_ptr_release(const slon::referenced* ref) {
    ref->remove_ref();
}

} // namespace boost

#endif // __SLON_ENGINE_UTILITY_REFERENCED_HPP__
