#ifndef __SLON_ENGINE_UTILITY_SINGLETON_HPP__
#define __SLON_ENGINE_UTILITY_SINGLETON_HPP__

#include <boost/shared_ptr.hpp>

namespace slon {

/** Template for singleton class instantiation */
template< typename T, typename PtrT = boost::shared_ptr<T> >
class singleton
{
public:
    typedef PtrT    holder_ptr;

public:
    static T* instance()
    {
        if (!inst) {
            inst = holder_ptr(new T);
        }

        return &(*inst);
    }

    static PtrT shared_instance()
    {
        if (!inst) {
            inst = holder_ptr(new T);
        }

        return inst;
    }

private:
    static PtrT inst;
};

template<typename T, typename PtrT>
PtrT singleton<T, PtrT>::inst;

} // namespace slon

#endif // __SLON_ENGINE_UTILITY_SINGLETON_HPP__
