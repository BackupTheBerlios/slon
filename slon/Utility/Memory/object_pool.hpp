#pragma once

#include <boost/type_traits/has_trivial_constructor.hpp>
#include <boost/type_traits/has_trivial_destructor.hpp>
#include "block_allocator.h"
#include "if_then_else.h"

namespace slon {

template< typename T,
		  typename UserAllocator = boost::default_user_allocator_new_delete >
class object_pool :
	public boost::noncopyable
{
private:
	typedef block_allocator<UserAllocator>	block_allocator_type;

public:
	typedef T												element_type;
	typedef UserAllocator									user_allocator;
	typedef Traits											traits_type;
	typedef typename block_allocator_type::size_type		size_type;
	typedef typename block_allocator_type::difference_type	difference_type;

private:
	void my_construct(element_type* ptr, 
                      typename boost::true_type& tag)
	{
	}

	void my_construct(element_type* ptr, 
                      typename boost::false_type& tag)
	{
		ptr->T();
	}
	
	void my_destroy(element_type* ptr, 
					typename boost::true_type& tag)
	{
	}

	void my_destroy(element_type* ptr, 
					typename boost::false_type& tag)
	{
		ptr->~T();
	}

public:
	object_pool(size_type capacity     = 16,
				size_type elemsPerPage = 256)
	:	blockAllocator(sizeof(T), capacity, elemsPerPage)	
	{
	}

	element_type* construct()
	{
		element_type* p = (element_type*)blockAllocator.allocate();
        my_construct( p, boost::has_trivial_constructor<T>::type() );
		return p;
	}
	
	void destroy(T* p)
	{
        my_destroy( p, boost::has_trivial_destructor<T>::type() );
		blockAllocator.deallocate(p);
	}

	block_allocator_type& get_allocator() { return blockAllocator; }

private:
	block_allocator_type blockAllocator;
};

} // namespace slon
