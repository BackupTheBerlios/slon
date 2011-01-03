#ifndef __SLON_ENGINE_CALLBACK_H__
#define __SLON_ENGINE_CALLBACK_H__

#include "referenced.hpp"
#include <boost/function.hpp>

namespace slon {

template<typename R>
class callback0 :
    public referenced
{
public:
    virtual R operator () (void) = 0;
};

template<typename R, typename T0>
class callback1 :
    public referenced
{
public:
    virtual R operator () (T0) = 0;
};

template<typename R, typename T0, typename T1>
class callback2 :
    public referenced
{
public:
    virtual R operator () (T0, T1) = 0;
};

template<typename R, typename T0, typename T1, typename T2>
class callback3 :
    public referenced
{
public:
    virtual R operator () (T0, T1, T2) = 0;
};

template<typename R, typename T0, typename T1, typename T2, typename T3>
class callback4 :
    public referenced
{
public:
    virtual R operator () (T0, T1, T2, T3) = 0;
};

/** Callback class used for making fast callback functors. */
template<typename Signature>
class callback;

template<typename R>
class callback<R ()> :
    public callback0<R>
{};

template<typename R, typename T0>
class callback<R (T0)> :
    public callback1<R, T0>
{};

template<typename R, typename T0, typename T1>
class callback<R (T0, T1)> :
    public callback2<R,T0,T1>
{};

template<typename R, typename T0, typename T1, typename T2>
class callback<R (T0, T1, T2)> :
    public callback3<R,T0,T1,T2>
{};

template<typename R, typename T0, typename T1, typename T2, typename T3>
class callback<R (T0, T1, T2, T3)> :
    public callback4<R,T0,T1,T2,T3>
{};

} // namespace slon

#endif // __SLON_ENGINE_CALLBACK_H__
