#include "stdafx.h"
#include "Thread/Detail/ThreadManager.h"

namespace slon {
namespace thread {
namespace detail {

ThreadManager::ThreadManager()
{
    mainThreadId = boost::this_thread::get_id();
}

THREAD_SEMANTIC ThreadManager::getCurrentThreadSemantic() const
{
    boost::thread::id currentThreadId = boost::this_thread::get_id();
    if (currentThreadId == mainThreadId) {
        return MAIN_THREAD;
    }

    for (int i = 0; i != MAX_THREAD_SEMANTIC; ++i)
    {
        if ( threads[i].get_id() == currentThreadId ) {
            return THREAD_SEMANTIC(i);
        }
    }

    return UNKNOWN_THREAD;
}

void ThreadManager::delegateToThread(THREAD_SEMANTIC thread, const void_function& function)
{
    assert( thread == MAIN_THREAD || thread >= 0 && thread < MAX_THREAD_SEMANTIC );

    // We are in main thread, so perform function
    if (thread == MAIN_THREAD)
    {
        if ( mainThreadId == boost::this_thread::get_id() ) 
        {
            function();
            return;
        }

        boost::unique_lock<boost::mutex> lock(mainThreadDelegateMutex);
        mainThreadDelegates.push_back(function);
        while ( !mainThreadDelegates.empty() ) {
            mainThreadDelegateCondition.wait(lock);
        }
    }
    else 
    {
        if ( threads[thread].get_id() == boost::this_thread::get_id() )
        {
            function();
            return;
        }

        boost::unique_lock<boost::mutex> lock(delegateMutexes[thread]);
        delegates[thread].push_back(function);
        while ( !delegates[thread].empty() ) {
            delegateConditions[thread].wait(lock);
        }
    }
}

bool ThreadManager::performDelayedFunctions(THREAD_SEMANTIC thread)
{
    assert( thread == MAIN_THREAD || thread >= 0 && thread < MAX_THREAD_SEMANTIC );

    bool result;
    if (thread == MAIN_THREAD)
    {
        mainThreadDelegateMutex.lock();
        {
            result = !mainThreadDelegates.empty();
            std::for_each( mainThreadDelegates.begin(), 
                           mainThreadDelegates.end(), 
                           mem_fn( &void_function::operator() ) );

            mainThreadDelegates.clear();
            mainThreadDelegateCondition.notify_all();
        }
        mainThreadDelegateMutex.unlock();
    }
    else
    {
        delegateMutexes[thread].lock();
        {
            result = !delegates[thread].empty();
            std::for_each( delegates[thread].begin(), 
                           delegates[thread].end(), 
                           mem_fn( &void_function::operator() ) );

            delegates[thread].clear();
            delegateConditions[thread].notify_all();
        }
        delegateMutexes[thread].unlock();
    }

    return result;
}

void ThreadManager::startThread(THREAD_SEMANTIC semantic, const void_function& function)
{
    assert(semantic >= 0 && semantic < MAX_THREAD_SEMANTIC);
    boost::thread thread(function);
    threads[semantic].swap(thread);
}

void ThreadManager::joinThread(THREAD_SEMANTIC semantic)
{
    assert(semantic >= 0 && semantic < MAX_THREAD_SEMANTIC);
    threads[semantic].join();
}
/*
void ThreadManager::clearDelegates()
{
    mainThreadDelegateMutex.lock();
    delegates.clear();
    condition.notify_all();
    mainThreadDelegateMutex.unlock();
}
*/
} // namespace detail
} // namespace thread
} // namespace slon