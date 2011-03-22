#ifndef __SLON_ENGINE_THREAD_DETAIL_THREAD_MANAGER_H__
#define __SLON_ENGINE_THREAD_DETAIL_THREAD_MANAGER_H__

#include "../Lock.h"
#include "../ThreadManager.h"
#define NOMINMAX // thread may include windows.h
#include <boost/function.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition_variable.hpp>
#include <vector>

namespace slon {

// Forward decl
class Engine;

namespace thread {
namespace detail {

class ThreadManager :
    public thread::ThreadManager
{
public:
    typedef std::vector<void_function>  delegate_vector;

public:
    ThreadManager();

    // Override ThreadManager
    THREAD_SEMANTIC getCurrentThreadSemantic() const;

    void delegateToThread(THREAD_SEMANTIC thread, const void_function& function);


    // Called by Engine
    bool performDelayedFunctions(THREAD_SEMANTIC thread);
    void startThread(THREAD_SEMANTIC semantic, const void_function& function);
    void joinThread(THREAD_SEMANTIC semantic);

private:
    boost::thread               threads[MAX_THREAD_SEMANTIC];
    boost::thread::id           mainThreadId;

    // functions delegated in the threads
    boost::mutex                mainThreadDelegateMutex;
    boost::condition_variable   mainThreadDelegateCondition;
    delegate_vector             mainThreadDelegates;

    boost::mutex                delegateMutexes[MAX_THREAD_SEMANTIC];
    boost::condition_variable   delegateConditions[MAX_THREAD_SEMANTIC];
    delegate_vector             delegates[MAX_THREAD_SEMANTIC];
};

} // namespace detail
} // namespace thread
} // namespace slon

#endif // __SLON_ENGINE_THREAD_DETAIL_THREAD_MANAGER_H__
