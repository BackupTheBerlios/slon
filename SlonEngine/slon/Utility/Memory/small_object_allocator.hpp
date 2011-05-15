#pragma once

#include <boost/shared_array.hpp>
#include <limits>
#include <memory>
#include "block_allocator.h"

// some libraries may define their own new/delete
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

/// structure to specify allocator distribution
struct allocation_desc
{
	size_t	size;			///	size of the block
	size_t	count;			/// number of initially allocated elements
	size_t	blocksPerPage;	/// number of items in page
};

template<typename T>
struct small_object_allocator_traits
{
	typedef typename boost::is_fundamental<T>::type no_construct;
	typedef typename boost::is_fundamental<T>::type no_destroy;
};

/** Allocator based on google TCMalloc approach for small objects: 
 * http://goog-perftools.sourceforge.net/doc/tcmalloc.html.
 * Also may also find usefull resources on Intel TBB: 
 * http://download.intel.com/technology/itj/2007/v11i4/5-foundations/5-Foundations_for_Scalable_Multi-core_Software.pdf
 * WARNING: This allocator has state - pointers to the block allocators, they are copied in copy constructor.
 * NOTICE: Can be optimized by removing indirect usage of block allocators. But need to be extra carefull, because
 * standart rely on assumption that allocators don't have state.
 * @tparam UserAllocator - allocator for allocating large chunks of data, that will be distributed along small allocations.
 */
template< typename T,
		  typename UserAllocator = boost::default_user_allocator_new_delete >
class small_object_allocator
{
template<typename U, typename UA, typename T>
friend class small_object_allocator;
public:
	typedef UserAllocator								user_allocator;

	typedef T											value_type;
	typedef T*											pointer;
	typedef T&											reference;
	typedef const T*									const_pointer;
	typedef const T&									const_reference;
	typedef typename user_allocator::size_type			size_type;
	typedef typename user_allocator::difference_type	difference_type;

    // convert small_object_allocator<T> to small_object_allocator<U>
    template<typename U>
    struct rebind {
        typedef small_object_allocator<U, UserAllocator> other;
    };

public:	
	static const size_type		 base_num_classes = 18;				// number of allocator classes
	static const allocation_desc base_classes[base_num_classes];	// allocator classes(sizes of blocks)
	
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
		
    void my_construct(element_type* ptr, 
                      const element_type& val)
	{
		ptr->T(val);
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

private:
	typedef block_allocator<user_allocator>				block_allocator_type;
	typedef boost::shared_array<block_allocator_type>	block_allocator_array;

	struct allocator_deallocator
	{
		allocator_deallocator(size_t numAllocators)
		:	numAllocators_(numAllocators)
		{
		}

		void operator () (block_allocator_type* ptr)
		{
			for (auto pA = ptr; pA != ptr + numAllocators_; ++pA) {
				pA->~block_allocator_type();
			}
			user_allocator::free((char*)ptr);
		}

		size_t numAllocators_;
	};

	
	template<typename Iterator>
	void setup_allocators(Iterator begin, Iterator end)
	{
		numAllocators_ = std::distance(begin, end);
		allocators_.reset( (block_allocator_type*)user_allocator::malloc(numAllocators_ * sizeof(block_allocator_type)), 
						   allocator_deallocator(numAllocators_) );

		block_allocator_type* pA = allocators_.get();
		for (Iterator it = begin; it != end; ++it, ++pA) {
			new(pA) block_allocator_type(it->size * sizeof(T), it->count, it->blocksPerPage);
		}
	}

private:
	// better don't mess with copy
	small_object_allocator& operator = (const small_object_allocator& other);

	block_allocator_type* begin_allocator() { return allocators_.get(); }
	block_allocator_type* end_allocator()   { return allocators_.get() + numAllocators_; }

public:
	small_object_allocator()
#ifdef SMALL_OBJECT_ALLOCATOR_STATISTICS
	:	numExcessAllocations_(0)
	,	minAllocationSize_( std::numeric_limits<size_type>::max() )
	,	maxAllocationSize_(0)
#endif 
	{
		setup_allocators(base_classes, base_classes + base_num_classes);
	}
	
	/** Create allocator with custom block size distribution. 
	 * @tparam Iterator - iterator addressing allocation descriptions (block_size, capacity) for block allocators. Range must be ordered in
	 * ascending order by block_size.
	 * @see allocation_desc
	 */
	template<typename Iterator>
	small_object_allocator(Iterator begin, Iterator end)
#ifdef SMALL_OBJECT_ALLOCATOR_STATISTICS
	:	numExcessAllocations_(0)
	,	minAllocationSize_( std::numeric_limits<size_type>::max() )
	,	maxAllocationSize_(0)
#endif 
	{
		setup_allocators(begin, end);
	}

	small_object_allocator(const small_object_allocator& other)
	:	numAllocators_(other.numAllocators_)
	,	allocators_(other.allocators_)
#ifdef SMALL_OBJECT_ALLOCATOR_STATISTICS
	,	numExcessAllocations_(0)
	,	minAllocationSize_( std::numeric_limits<size_type>::max() )
	,	maxAllocationSize_(0)
#endif 
	{
	}

	~small_object_allocator()
	{
		// for debug convenience
	}

	template<typename U, typename P>
	explicit small_object_allocator(const small_object_allocator<U, P>& other)
	:	numAllocators_(other.numAllocators_)
	,	allocators_(other.allocators_)
#ifdef SMALL_OBJECT_ALLOCATOR_STATISTICS
	,	numExcessAllocations_(0)
	,	minAllocationSize_( std::numeric_limits<size_type>::max() )
	,	maxAllocationSize_(0)
#endif 
	{
	}

	size_type max_size() const
	{
		return std::numeric_limits<size_type>::max();
	}
		
	void construct(pointer p)
	{
        my_construct( p, boost::type_traits::has_trivial_constructor<T>::type() );
	}

	void construct(pointer p, const value_type& val)
	{
		new (p) T(val);
	}
	
	void destroy(pointer p)
	{
		my_destroy( p, boost::type_traits::has_trivial_destructor<T>::type() );
	}

	pointer allocate(size_type count, const_pointer hint = 0)
	{
#ifdef SMALL_OBJECT_ALLOCATOR_STATISTICS
		minAllocationSize_ = (std::min)(minAllocationSize_, count);
		maxAllocationSize_ = (std::max)(maxAllocationSize_, count);
#endif 
		// find allocator class
		block_allocator_type* iter = std::lower_bound( begin_allocator(),
													   end_allocator(),
													   count * sizeof(value_type),
													   [] (const block_allocator_type& a, size_type s) { return a.block_size() < s; } );

		if ( iter != end_allocator() ) {
			return (pointer)iter->allocate(); // allocate block of sufficient size
		}

#ifdef SMALL_OBJECT_ALLOCATOR_STATISTICS
		++numExcessAllocations_;
#endif
		// allocate using default allocation policy
		return (pointer)user_allocator::malloc(count * sizeof(value_type));
	}

	void deallocate(pointer ptr, size_type count)
	{
		block_allocator_type* iter = std::lower_bound( begin_allocator(),
													   end_allocator(),
													   count * sizeof(value_type),
													   [] (const block_allocator_type& a, size_type s) { return a.block_size() < s; } );
		
		if ( iter != end_allocator() ) {
			iter->deallocate(ptr);
		}
		else 
		{
			// deallocate using default allocation policy
			user_allocator::free((char*)ptr);
		}
	}

	bool operator == (const small_object_allocator& other)
	{
		return (numAllocators_ == other.numAllocators_) && (allocators_ == other.allocators_);
	}

	bool operator != (const small_object_allocator& other)
	{
		return !(*this == other);
	}

	size_t num_block_allocators() const
	{
		return numAllocators_;
	}
	
	block_allocator_type& get_block_allocator(size_t i)
	{
		assert(i < numAllocators_);
		return allocators_[i];
	}

	const block_allocator_type& get_block_allocator(size_t i) const
	{
		assert(i < numAllocators_);
		return allocators_[i];
	}
	
#ifdef SMALL_OBJECT_ALLOCATOR_STATISTICS
	size_type num_excess_allocations() const	{ return numExcessAllocations_; }
	size_type min_allocation() const			{ return minAllocationSize_ == std::numeric_limits<size_type>::max() ? 0 : minAllocationSize_; }
	size_type max_allocation() const			{ return maxAllocationSize_; }
#endif

private:
	size_t					numAllocators_;
	block_allocator_array	allocators_;

public:
#ifdef SMALL_OBJECT_ALLOCATOR_STATISTICS
	size_type	numExcessAllocations_;
	size_type	maxAllocationSize_;
	size_type	minAllocationSize_;
#endif
};

// default allocator classes (block size, capacity)
template<typename T, typename UserAllocator, typename Traits>
const allocation_desc small_object_allocator<T, UserAllocator, Traits>::base_classes[small_object_allocator<T, UserAllocator, Traits>::base_num_classes] =
{
	{4,   16, 128},
	{8,   16, 128},
	{12,  8,  64},
	{16,  8,  64},
	{24,  8,  64},
	{32,  4,  32},
	{40,  4,  32},
	{48,  4,  32},
	{56,  2,  16},
	{64,  2,  16},
	{80,  2,  16},
	{96,  2,  16},
	{112, 1,  8},
	{128, 1,  8},
	{160, 1,  8},
	{192, 1,  8},
	{224, 1,  4},
	{256, 1,  4}
};

} // namespace slon

#ifdef _POP_NEW_MACRO
#   pragma pop_macro("new")
#endif
#ifdef _POP_DELETE_MACRO
#   pragma pop_macro("delete")
#endif