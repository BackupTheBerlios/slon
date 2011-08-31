#ifndef __SLON_ENGINE_UTILITY_CONNECTION_HPP__
#define __SLON_ENGINE_UTILITY_CONNECTION_HPP__

#include <boost/scoped_ptr.hpp>
#include <boost/type_traits/is_base_of.hpp>
#include "function.hpp"
#include "signal.hpp"

namespace slon {

/** Connection class automatically disconnects slot from signal in destructor. */
class connection :
    public boost::noncopyable
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
        ptr_holder( signal_base<Signature>* sig_,
                    slot<Signature>*        slt_,
                    SlotDestructor          des_ = SlotDestructor() )
        :   sig(sig_)
        ,   slt(slt_)
        ,   des(des_)
        {
            assert(sig && slt);
            sig->connect(slt);
        }

        ~ptr_holder() 
        { 
            sig->disconnect(slt);
            des(slt);
        }

    public:
        signal_base<Signature>* sig;
        slot<Signature>*        slt;
        SlotDestructor          des;
    };

    template<typename Signature, typename Slot>
    class stack_holder :
        public object_in_pool<stack_holder<Signature, Slot>, holder>
    {
    public:
        stack_holder(signal_base<Signature>* sig_,
                     Slot                    slt_)
        :   sig(sig_)
        ,   slt(slt_)
        {
            assert(sig);
            sig->connect(&slt);
        }

        ~stack_holder() 
        { 
            sig->disconnect(&slt);
        }

    public:
        signal_base<Signature>*  sig;
        Slot                     slt;
    };

    template<typename Signature>
    class function_holder :
        public object_in_pool<function_holder<Signature>, holder>
    {
    public:
        function_holder(signal_base<Signature>* sig_,
                        function<Signature>     func_)
        :   sig(sig_)
        ,   func(func_)
        {
            assert(sig);
            sig->connect( func.get_slot() );
        }

        ~function_holder() 
        { 
            sig->disconnect( func.get_slot() );
        }

    public:
        signal_base<Signature>*  sig;
        function<Signature>      func;
    };

public:
    connection()
    {
    }

    template<typename Signature>
    connection(signal_base<Signature>& sig,
               slot<Signature>*        slt)
    {
        reset(sig, slt);
    }

    template<typename Signature, typename SlotDestructor>
    connection(signal_base<Signature>& sig, 
               slot<Signature>*        slt,
               SlotDestructor          destr)
    {
        reset(sig, slt, destr);
    }

    template<typename Signature, typename Slot>
    connection(signal_base<Signature>& sig, 
               Slot                    slt,
               typename boost::is_base_of<slot<Signature>, Slot>::type* tag = 0)
    {
        reset(sig, slt);
    }

    template<typename Signature>
    connection(signal_base<Signature>& sig,
               function<Signature>     func)
    {
        reset(sig, func);
    }

    void reset()
    {
        hld.reset();
    }

    template<typename Signature>
    void reset(signal_base<Signature>& sig, 
               slot<Signature>*        slt)
    {
        hld.reset( new ptr_holder<Signature, no_destructor<Signature> >(&sig, slt) );
    }

    template<typename Signature, typename SlotDestructor>
    void reset(signal_base<Signature>& sig, 
               slot<Signature>*        slt,
               SlotDestructor          destr)
    {
        hld.reset( new ptr_holder<Signature, SlotDestructor>(&sig, slt, destr) );
    }

    template<typename Signature, typename Slot>
    void reset(signal_base<Signature>& sig,
               Slot                    slt,
               typename boost::is_base_of<slot<Signature>, Slot>::type* tag = 0)
    {
        hld.reset( new stack_holder<Signature, Slot>(&sig, slt) );
    }

    template<typename Signature>
    void reset(signal_base<Signature>& sig,
               function<Signature>     func)
    {
        hld.reset( new function_holder<Signature>(&sig, func) );
    }

private:
    boost::scoped_ptr<holder> hld;
};

} // namespace slon

#endif // __SLON_ENGINE_UTILITY_CONNECTION_HPP__
