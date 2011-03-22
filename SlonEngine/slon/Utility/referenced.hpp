#ifndef SLON_ENGINE_REFERENCED_H
#define SLON_ENGINE_REFERENCED_H

#include "../Config.h"
#ifdef SLON_ENGINE_USE_SSE
#include <sgl/Utility/Aligned.h>
#endif

namespace slon {

class Referenced
#ifdef SLON_ENGINE_USE_SSE
    // Just make sure classes using SSE vectors will not fail
    : public sgl::Aligned16
#endif
{
public:
    Referenced() :
        refCount(0)
    {}

    Referenced(const Referenced& /*referenced*/) :
        refCount(0)
    {}

    // Don't modify reference counter
    Referenced& operator = (const Referenced& /*rhs*/)
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

    virtual ~Referenced() {}

protected:
    mutable int refCount;
};

typedef Referenced referenced;

} // namespace slon

namespace boost {

inline void intrusive_ptr_add_ref(const slon::Referenced* referenced) {
    referenced->add_ref();
}

inline void intrusive_ptr_release(const slon::Referenced* referenced) {
    referenced->remove_ref();
}

} // namespace boost

#endif // SLON_ENGINE_REFERENCED_H
