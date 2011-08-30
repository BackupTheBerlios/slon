#ifndef __SLON_ENGINE_UTILITY_SIGNAL_HPP__
#define __SLON_ENGINE_UTILITY_SIGNAL_HPP__

#include "Algorithm/algorithm.hpp"
#include "slot.hpp"
#include <vector>

namespace slon {

/** Signal class used for storing slots. */
template<typename Signature>
class signal;

template<typename Signature>
class signal_base
{
public:
    typedef slot<Signature> slot_type;

protected:
    typedef std::vector<slot_type*>             slot_container;
    typedef typename slot_container::iterator   slot_iterator;

public:
    void connect(slot_type* slot)
    {
        slots.push_back(slot);
    }

    void disconnect(slot_type* slot)
    {
        quick_remove(slots, slot);
    }

protected:
    slot_container slots;
};

template<typename R>
class signal<R ()> :
    public signal_base<R ()>
{
private:
    typedef signal_base<R ()>                   base_type;
    typedef typename base_type::slot_iterator   slot_iterator;

public:
    void operator () (void)
    {
        for (slot_iterator i = base_type::slots.begin(); i != base_type::slots.end(); ++i)
        {
            (**i)();
        }
    }
};

template<typename R, typename T0>
class signal<R (T0)> :
    public signal_base<R (T0)>
{
private:
    typedef signal_base<R (T0)>                 base_type;
    typedef typename base_type::slot_iterator   slot_iterator;

public:
    void operator () (T0 t0)
    {
        for (slot_iterator i = base_type::slots.begin(); i != base_type::slots.end(); ++i)
        {
            (**i)(t0);
        }
    }
};

template<typename R, typename T0, typename T1>
class signal<R (T0, T1)> :
    public signal_base<R (T0, T1)>
{
private:
    typedef signal_base<R (T0, T1)>             base_type;
    typedef typename base_type::slot_iterator   slot_iterator;

public:
    void operator () (T0 t0, T1 t1)
    {
        for (slot_iterator i = base_type::slots.begin(); i != base_type::slots.end(); ++i)
        {
            (**i)(t0, t1);
        }
    }
};

template<typename R, typename T0, typename T1, typename T2>
class signal<R (T0, T1, T2)> :
    public signal_base<R (T0, T1, T2)>
{
private:
    typedef signal_base<R (T0, T1, T2)>         base_type;
    typedef typename base_type::slot_iterator   slot_iterator;

public:
    void operator () (T0 t0, T1 t1, T2 t2)
    {
        for (slot_iterator i = base_type::slots.begin(); i != base_type::slots.end(); ++i)
        {
            (**i)(t0, t1, t2);
        }
    }
};

template<typename R, typename T0, typename T1, typename T2, typename T3>
class signal<R (T0, T1, T2, T3)> :
    public signal_base<R (T0, T1, T2, T3)>
{
private:
    typedef signal_base<R (T0, T1, T2, T3)>     base_type;
    typedef typename base_type::slot_iterator   slot_iterator;

public:
    void operator () (T0 t0, T1 t1, T2 t2, T3 t3)
    {
        for (slot_iterator i = base_type::slots.begin(); i != base_type::slots.end(); ++i)
        {
            (**i)(t0, t1, t2, t3);
        }
    }
};

} // namespace slon

#endif // __SLON_ENGINE_UTILITY_SIGNAL_HPP__
