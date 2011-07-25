#ifndef __SLON_ENGINE_UTILITY_SLOT_HPP__
#define __SLON_ENGINE_UTILITY_SLOT_HPP__

namespace slon {

/** Slot(callback, oversver) class used for making fast callback functors. */
template<typename Signature>
class slot;

template<typename R>
class slot<R ()>
{
public:
    virtual R operator () () = 0;

    virtual ~slot() {}
};

template<typename R, typename T0>
class slot<R (T0)>
{
public:
    virtual R operator () (T0) = 0;

    virtual ~slot() {}
};

template<typename R, typename T0, typename T1>
class slot<R (T0, T1)>
{
public:
    virtual R operator () (T0, T1) = 0;

    virtual ~slot() {}
};

template<typename R, typename T0, typename T1, typename T2>
class slot<R (T0, T1, T2)>
{
public:
    virtual R operator () (T0, T1, T2) = 0;

    virtual ~slot() {}
};

template<typename R, typename T0, typename T1, typename T2, typename T3>
class slot<R (T0, T1, T2, T3)>
{
public:
    virtual R operator () (T0, T1, T2, T3) = 0;

    virtual ~slot() {}
};

} // namespace slon

#endif // __SLON_ENGINE_UTILITY_SLOT_HPP__
