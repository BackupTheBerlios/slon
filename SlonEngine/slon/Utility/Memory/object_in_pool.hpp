#ifndef __SLON_ENGINE_OBJECT_IN_POOL_H__
#define __SLON_ENGINE_OBJECT_IN_POOL_H__

#include <boost/shared_ptr.hpp>
#include "../../Config.h"
#include "object_pool.hpp"

#ifdef new
#   pragma push_macro("new")
#   undef new
#   define _POP_NEW_MACRO
#endif

#ifdef delete
#   pragma push_macro("delete")
#   undef delete
#   define _POP_DELETE_MACRO
#endif

namespace slon {

template<size_t Size, typename UserAllocator>
class singleton_pool :
    public block_allocator<UserAllocator>
{
public:
    static const size_t element_size = Size;

	typedef block_allocator<UserAllocator>	                block_allocator_type;
	typedef UserAllocator									user_allocator;
	typedef typename block_allocator_type::size_type		size_type;
	typedef typename block_allocator_type::difference_type	difference_type;

private:
    class removal_guard
    {
    public:
        removal_guard(singleton_pool* pool_)
        :   pool(pool_)
        {
            pool->add_ref();
        }

        ~removal_guard()
        {
            pool->remove_ref();
        }

    private:
        singleton_pool* pool;
    };

public:
	singleton_pool( size_type capacity = 8,
				    size_type nextSize = 16 )
    :   block_allocator_type(element_size, capacity, nextSize)
    ,   refCount(0)
    {
    }

    ~singleton_pool()
    {
        assert(refCount == 0);
    }

    void add_ref()
    {
        ++refCount;
    }

    void remove_ref()
    {
        if (--refCount == 0) {
            delete this;
        }
    }

    void* allocate() 
    {
        add_ref();
        return block_allocator_type::allocate();
    }

    void deallocate(void* ptr)
    {
        block_allocator_type::deallocate(ptr);
        remove_ref();
    }

    static singleton_pool* instance() 
    {
        if (!inst) 
        {
            inst = new singleton_pool();
            guard.reset(new removal_guard(inst));
        }

        return inst;
    }

private:
    size_type                               refCount;
    static singleton_pool*                  inst;
    static boost::shared_ptr<removal_guard> guard; // guards pool from frequent allocation/deallocation
};

template<size_t Size, typename UserAllocator>
singleton_pool<Size, UserAllocator>* singleton_pool<Size, UserAllocator>::inst = 0;

template<size_t Size, typename UserAllocator>
boost::shared_ptr<typename singleton_pool<Size, UserAllocator>::removal_guard> singleton_pool<Size, UserAllocator>::guard;

#ifdef SLON_ENGINE_DISABLE_OBJECT_POOL

template<typename T, typename Base = void, typename Allocator = boost::default_user_allocator_new_delete>
class object_in_pool :
	public Base
{
public:
	object_in_pool() {}

	template<typename T0>
	object_in_pool(T0 a0) : Base(a0) {}

	template<typename T0, typename T1>
	object_in_pool(T0 a0, T1 a1) : Base(a0, a1) {}

	template<typename T0, typename T1, typename T2>
	object_in_pool(T0 a0, T1 a1, T2 a2) : Base(a0, a1, a2) {}

	template<typename T0, typename T1, typename T2, typename T3>
	object_in_pool(T0 a0, T1 a1, T2 a2, T3 a3) : Base(a0, a1, a2, a3) {}
};

template<typename T, typename Allocator>
class object_in_pool<T, void, Allocator>
{
};

#else // SLON_ENGINE_DISABLE_OBJECT_POOL

/** Overload default new/delete operator to allocate object in pool */
template<typename T, typename Base = void, typename Allocator = boost::default_user_allocator_new_delete>
class object_in_pool :
	public Base
{
public:
	object_in_pool() {}

	template<typename T0>
	object_in_pool(T0 a0) : Base(a0) {}

	template<typename T0, typename T1>
	object_in_pool(T0 a0, T1 a1) : Base(a0, a1) {}

	template<typename T0, typename T1, typename T2>
	object_in_pool(T0 a0, T1 a1, T2 a2) : Base(a0, a1, a2) {}

	template<typename T0, typename T1, typename T2, typename T3>
	object_in_pool(T0 a0, T1 a1, T2 a2, T3 a3) : Base(a0, a1, a2, a3) {}

	void* operator new (size_t size)
	{
		assert( size == sizeof(T) );
        return singleton_pool<sizeof(T), Allocator>::instance()->allocate();
	}

	void operator delete (void* ptr)
	{
		singleton_pool<sizeof(T), Allocator>::instance()->deallocate(ptr);
	}
};

template<typename T, typename Allocator>
class object_in_pool<T, void, Allocator>
{/*
private:
    typedef singleton_pool<sizeof(T), Allocator> singleton_pool_type;

public:
	void* operator new (size_t size)
	{
		assert( size == sizeof(T) );
        return singleton_pool<sizeof(T), Allocator>::instance()->allocate();
	}

	void operator delete (void* ptr)
	{
		singleton_pool<sizeof(T), Allocator>::instance()->deallocate(ptr);
    }*/
};

#endif // !SLON_ENGINE_DISABLE_OBJECT_POOL

} // namespace slon

#ifdef _POP_NEW_MACRO
#   pragma pop_macro("new")
#endif
#ifdef _POP_DELETE_MACRO
#   pragma pop_macro("delete")
#endif

#endif // __SLON_ENGINE_OBJECT_IN_POOL_H__
