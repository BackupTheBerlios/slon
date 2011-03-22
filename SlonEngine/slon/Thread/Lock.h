#ifndef __SLON_ENGINE_THREAD_LOCK_H__
#define __SLON_ENGINE_THREAD_LOCK_H__

#include <boost/shared_ptr.hpp>

namespace slon {
namespace thread {

/** Abstract lock class for wrapping different boost lock types */
struct lock 
{ 
    virtual ~lock() {} 
};

template<typename T>
class typed_lock :
    public lock
{
public:
    typed_lock(T* _lock) :
        lock(_lock)
    {}

private:
    boost::shared_ptr<T> lock;
};

typedef boost::shared_ptr<lock> lock_ptr;

template<typename T>
inline boost::shared_ptr< typed_lock<T> > create_lock(T* _lock)
{
    return boost::shared_ptr< typed_lock<T> >( new typed_lock<T>(_lock) );
}

} // namespace thread
} // namespace slon

#endif // __SLON_ENGINE_THREAD_LOCK_H__