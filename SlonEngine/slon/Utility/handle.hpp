#ifndef __SLON_ENGINE_UTILITY_HANDLE_H__
#define __SLON_ENGINE_UTILITY_HANDLE_H__

namespace slon {

/** Handle for object for fast comparison and access to implementation class. */
struct handle
{
    handle(void* pObj_ = 0) 
    :   pObj(pObj_)
    {}

    bool operator == (const handle& rhs) const { return pObj == rhs.pObj; }
    bool operator != (const handle& rhs) const { return pObj != rhs.pObj; }
    bool operator  < (const handle& rhs) const { return pObj  < rhs.pObj; }

    void* pObj;
};

} // namespace slon

#endif // __SLON_ENGINE_UTILITY_HANDLE_H__