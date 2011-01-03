#ifndef __SLON_ENGINE_THREAD_THREAD_MANAGER_H__
#define __SLON_ENGINE_THREAD_THREAD_MANAGER_H__

#include <boost/function.hpp>

namespace slon {

// Forward decl
class Engine;

namespace thread {

/** All threads in the Engine */
enum THREAD_SEMANTIC
{
    UNKNOWN_THREAD      = -2,
    MAIN_THREAD         = -1,
    SIMULATION_THREAD   =  0,
    RESOURCE_THREAD     =  1,
    NETWORK_THREAD      =  2,
    MAX_THREAD_SEMANTIC =  3
};

class ThreadManager
{
public:
    typedef boost::function<void ()> void_function;

    template<typename Result>
    struct function_with_result :
        std::unary_function<void, Result>
    {
        function_with_result(const boost::function<Result ()>& _function) :
            function(_function)
        {}

        void operator () () { result = function(); }

        boost::function<Result ()>  function;
        Result                      result;
    };

public:
    /** Get current thread semantic */
    virtual THREAD_SEMANTIC getCurrentThreadSemantic() const = 0;

    /** Delegate function call to the specified thread, this can be usefull
     * to delegate some graphics functions, wich must be implemented
     * in the main thread.
     * @param thread - thread where to delegate function.
     * @param function - function for performing.
     */
    virtual void delegateToThread(THREAD_SEMANTIC thread, const void_function& function) = 0;

    /** Delegate function call to the specified thread and wait for result.
     * @param thread - thread where to delegate function.
     * @param function - function for performing.
     */
    template<typename Result>
    Result delegateToThread(THREAD_SEMANTIC thread, const boost::function<Result ()>& function)
    {
        function_with_result<Result> functionWithResult(function);
        delegateToThread( thread, boost::ref(functionWithResult) );
        return functionWithResult.result;
    }
};

/** Get current thread manager used by engine */
ThreadManager& currentThreadManager();

} // namespace thread
} // namespace slon

#endif // __SLON_ENGINE_THREAD_THREAD_MANAGER_H__
