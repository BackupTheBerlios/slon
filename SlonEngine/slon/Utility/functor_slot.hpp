#ifndef __SLON_ENGINE_UTILITY_FUNCTOR_SLOT_HPP__
#define __SLON_ENGINE_UTILITY_FUNCTOR_SLOT_HPP__

#include <boost/type_traits/is_base_of.hpp>
#include <functional>
#include "slot.hpp"

namespace slon {

/** Member function slot type. */
template<typename Functor, typename Signature>
class functor_adaptor;

template<typename Functor, typename R>
class functor_adaptor<Functor, R()> :
    public slot<R ()>
{
public:
    functor_adaptor(Functor func_)
    :   func(func_)
    {}

    R operator () (void)
    {
        return func();
    }

private:
    Functor func;
};

template<typename Functor>
class functor_adaptor<Functor, void ()> :
    public slot<void ()>
{
public:
    functor_adaptor(Functor func_)
    :   func(func_)
    {}

    void operator () (void)
    {
        func();
    }

private:
    Functor func;
};

template<typename Functor, typename R, typename T0>
class functor_adaptor<Functor, R (T0)> :
    public slot<R (T0)>
{
public:
    functor_adaptor(Functor func_)
    :   func(func_)
    {}

    R operator () (T0 t0)
    {
        return func(t0);
    }

private:
    Functor func;
};

template<typename Functor, typename T0>
class functor_adaptor<Functor, void (T0)> :
    public slot<void (T0)>
{
public:
    functor_adaptor(Functor func_)
    :   func(func_)
    {}

    void operator () (T0 t0)
    {
        func(t0);
    }

private:
    Functor func;
};

template<typename Functor, typename R, typename T0, typename T1>
class functor_adaptor<Functor, R (T0, T1)> :
    public slot<R (T0, T1)>
{
public:
    functor_adaptor(Functor func_)
    :   func(func_)
    {}

    R operator () (T0 t0, T1 t1)
    {
        return func(t0, t1);
    }

private:
    Functor func;
};

template<typename Functor, typename T0, typename T1>
class functor_adaptor<Functor, void (T0, T1)> :
    public slot<void (T0, T1)>
{
public:
    functor_adaptor(Functor func_)
    :   func(func_)
    {}

    void operator () (T0 t0, T1 t1)
    {
        func(t0, t1);
    }

private:
    Functor func;
};

template<typename Functor, typename R, typename T0, typename T1, typename T2>
class functor_adaptor<Functor, R (T0, T1, T2)> :
    public slot<R (T0, T1, T2)>
{
public:
    functor_adaptor(Functor func_)
    :   func(func_)
    {}

    R operator () (T0 t0, T1 t1, T2 t2)
    {
        return func(t0, t1, t2);
    }

private:
    Functor func;
};

template<typename Functor, typename T0, typename T1, typename T2>
class functor_adaptor<Functor, void (T0, T1, T2)> :
    public slot<void (T0, T1, T2)>
{
public:
    functor_adaptor(Functor func_)
    :   func(func_)
    {}

    void operator () (T0 t0, T1 t1, T2 t2)
    {
        func(t0, t1, t2);
    }

private:
    Functor func;
};

template<typename Functor, typename R, typename T0, typename T1, typename T2, typename T3>
class functor_adaptor<Functor, R (T0, T1, T2, T3)> :
    public slot<R (T0, T1, T2, T3)>
{
public:
    functor_adaptor(Functor func_)
    :   func(func_)
    {}

    R operator () (T0 t0, T1 t1, T2 t2, T3 t3)
    {
        return func(t0, t1, t2, t3);
    }

private:
    Functor func;
};

template<typename Functor, typename T0, typename T1, typename T2, typename T3>
class functor_adaptor<Functor, void (T0, T1, T2, T3)> :
    public slot<void (T0, T1, T2, T3)>
{
public:
    functor_adaptor(Functor func_)
    :   func(func_)
    {}

    void operator () (T0 t0, T1 t1, T2 t2, T3 t3)
    {
        func(t0, t1, t2, t3);
    }

private:
    Functor func;
};

template<typename Signature, typename Functor>
functor_adaptor<Functor, Signature> make_slot(Functor func)
{
    return functor_adaptor<Functor, Signature>(func);
}

} // namespace slon

#endif // __SLON_ENGINE_UTILITY_FUNCTOR_SLOT_HPP__
