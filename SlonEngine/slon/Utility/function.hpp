#ifndef __SLON_ENGINE_UTILITY_FUNCTION_HPP__
#define __SLON_ENGINE_UTILITY_FUNCTION_HPP__

#include <boost/shared_ptr.hpp>
#include <boost/type_traits/is_base_of.hpp>
#include "Memory/object_in_pool.hpp"
#include "slot.hpp"

namespace slon {

template<typename Signature>
class function_base;

template<typename R>
class function_base<R ()>
{
public:
    R operator () (void) { return (*slt)(); }

protected:
    ~function_base() {}

protected:
    slot<R ()>* slt;
};

template<>
class function_base<void ()>
{
public:
    void operator () (void) { (*slt)(); }

protected:
    ~function_base() {}

protected:
    slot<void ()>* slt;
};

template<typename R, typename T0>
class function_base<R (T0)>
{
public:
    R operator () (T0 t0) { return (*slt)(t0); }

protected:
    ~function_base() {}

protected:
    slot<R (T0)>* slt;
};

template<typename T0>
class function_base<void (T0)>
{
public:
    void operator () (T0 t0) { (*slt)(t0); }

protected:
    ~function_base() {}

protected:
    slot<void (T0)>* slt;
};

template<typename R, typename T0, typename T1>
class function_base<R (T0, T1)>
{
public:
    R operator () (T0 t0, T1 t1) { return (*slt)(t0, t1); }

protected:
    ~function_base() {}

protected:
    slot<R (T0, T1)>* slt;
};

template<typename T0, typename T1>
class function_base<void (T0, T1)>
{
public:
    void operator () (T0 t0, T1 t1) { (*slt)(t0, t1); }

protected:
    ~function_base() {}

protected:
    slot<void (T0, T1)>* slt;
};

template<typename R, typename T0, typename T1, typename T2>
class function_base<R (T0, T1, T2)>
{
public:
    R operator () (T0 t0, T1 t1, T2 t2) { return (*slt)(t0, t1, t2); }

protected:
    ~function_base() {}

protected:
    slot<R (T0, T1, T2)>* slt;
};

template<typename T0, typename T1, typename T2>
class function_base<void (T0, T1, T2)>
{
public:
    void operator () (T0 t0, T1 t1, T2 t2) { (*slt)(t0, t1, t2); }

protected:
    ~function_base() {}

protected:
    slot<void (T0, T1, T2)>* slt;
};

template<typename R, typename T0, typename T1, typename T2, typename T3>
class function_base<R (T0, T1, T2, T3)>
{
public:
    R operator () (T0 t0, T1 t1, T2 t2, T3 t3) { return (*slt)(t0, t1, t2, t3); }

protected:
    ~function_base() {}

protected:
    slot<R (T0, T1, T2, T3)>* slt;
};

template<typename T0, typename T1, typename T2, typename T3>
class function_base<void (T0, T1, T2, T3)>
{
public:
    void operator () (T0 t0, T1 t1, T2 t2, T3 t3) { (*slt)(t0, t1, t2, t3); }

protected:
    ~function_base() {}

protected:
    slot<void (T0, T1, T2, T3)>* slt;
};

/** Function is used to store slots on stack. */
template<typename Signature>
class function :
    public function_base<Signature>
{
private:
    template<typename Signature>
    struct no_destructor
    {
        void operator () (slot<Signature>* /*s*/) {}
    };

    class holder
    {
    public:
        virtual ~holder() {}
    };

    template<typename Signature, typename SlotDestructor>
    class ptr_holder :
        public object_in_pool<ptr_holder<Signature, SlotDestructor>, holder>
    {
    public:
        ptr_holder( slot<Signature>* s_,
                    SlotDestructor   d_ = SlotDestructor() )
        :   s(s_)
        ,   d(d_)
        {
        }

        ~ptr_holder() 
        { 
            d(s);
        }

    public:
        slot<Signature>* s;
        SlotDestructor   d;
    };

    template<typename SlotFunctor>
    class stack_holder :
        public object_in_pool<stack_holder<SlotFunctor>, holder>
    {
    public:
        stack_holder(SlotFunctor s_)
        :   s(s_)
        {
        }

    public:
        SlotFunctor s;
    };

public:
    function()
    {
        slt = 0;
    }

    function(slot<Signature>* slt_)
    {
        slt = slt_;
        hld.reset( new holder<Signature, no_destructor<Signature> >(slt_) );
    }

    template<typename SlotDestructor>
    function(slot<Signature>* slt_,
             SlotDestructor   destr)
    {
        slt = slt_;
        hld.reset( new holder<Signature, SlotDestructor>(slt_, destr) );
    }

    template<typename SlotFunctor>
    function(SlotFunctor slt_,
             typename boost::is_base_of<slot<Signature>, SlotFunctor>::type* tag = 0)
    {
        boost::shared_ptr<stack_holder<SlotFunctor> > sHld( new stack_holder<SlotFunctor>(slt_) );
        slt = sHld->s;
        hld = sHld;
    }

    slot<Signature>* get_slot() { return slt; }

private:
    boost::shared_ptr<holder> hld;
};

template<typename Signature>
function<Signature> make_function(slot<Signature>* slt)
{
    return function<Signature>(slt);
}

template<typename Signature, typename SlotDestructor>
function<Signature> make_function(slot<Signature>* slt,
                                  SlotDestructor   destr)
{
    return function<Signature>(slt, destr);
}

template<typename Signature, typename SlotFunctor>
function<Signature> make_function(SlotFunctor slt,
                                  typename boost::is_base_of<slot<Signature>, SlotFunctor>::type* tag = 0)
{
    return function<Signature>(slt);
}

} // namespace slon

#endif // __SLON_ENGINE_UTILITY_FUNCTION_HPP__
