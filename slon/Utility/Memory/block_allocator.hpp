#ifndef __SLON_ENGINE_BLOCK_ALLOCATOR_H__
#define __SLON_ENGINE_BLOCK_ALLOCATOR_H__

#include <boost/pool/pool.hpp>

// Fuck WinAPI
#pragma push_macro("max")
#pragma push_macro("min")
#undef min
#undef max

namespace slon {

/** Allocates identical blocks of memory of specified size.
 * WARNING: This allocator has state, you need allocate memory and deallocate using single allocator instance.
 * It is not copyable, though.
 * @tparam UserAllocator - allocator for allocating bytes, must support allocate(size_t nbytes, void* hint), deallocate(void* ptr, size_t size) functions
 */
template<typename UserAllocator = boost::default_user_allocator_new_delete>
class block_allocator :
	protected UserAllocator
{
public:
    typedef UserAllocator								user_allocator;
    typedef void*										pointer;
    typedef const void*									const_pointer;
    typedef typename user_allocator::size_type			size_type;
    typedef typename user_allocator::difference_type	difference_type;

private:
    typedef char*										byte_pointer;

private:
	pointer& next_of(pointer ptr)
	{
		return *static_cast<pointer*>(ptr);
	}

	size_type& size_of(pointer ptr)
	{
		return *static_cast<size_type*>(ptr);
	}

	void link_nodes(byte_pointer begin, byte_pointer end)
	{
		if (begin != end)
		{
			// each node points to next node
			endNode_ = begin;
			while (begin != end)
			{
				next_of(begin) = begin + blockSize_;
				begin += blockSize_;
			}
			next_of(end - blockSize_) = 0;
		}
	}

    void extend(size_type size)
    {
		if (size > nextSize_) {
			nextSize_ = size;
		}

		size_type    msize  = nextSize_ * blockSize_;
		byte_pointer chunk  = user_allocator::malloc( msize + sizeof(pointer) + sizeof(size_type) );
		capacity_		   += nextSize_;
		nextSize_		  <<= 1;
		
		if (endNode_) {
			next_of(endNode_) = chunk;
		}
        link_nodes(chunk, chunk + msize);

        chunk         += msize;
		next_of(chunk) = endChunk_;
        endChunk_      = chunk;
        chunk         += sizeof(pointer);
		size_of(chunk) = msize;
    }

	// noncopyable
	block_allocator& operator = (const block_allocator& other);

public:
    block_allocator(size_type blockSize, 
					size_type capacity = 0,
					size_type nextSize = 32)
    :   blockSize_(blockSize)
	,	capacity_(capacity)
	,	nextSize_(nextSize)
	,	endChunk_(0)
    ,   endNode_(0)
#ifdef SMALL_OBJECT_ALLOCATOR_STATISTICS
	,	numAllocations_(0)
	,	numDeallocations_(0)
	,	maxNumActiveAllocations_(0)
#endif
    {
		assert( blockSize_ >= sizeof(pointer) && blockSize_ % sizeof(pointer) == 0 && nextSize > 0 );
		if (capacity > 0) {
			extend(capacity);
		}
    }

	block_allocator(const block_allocator& other)
    :   blockSize_(other.blockSize_)
	,	capacity_(0)
	,	nextSize_(other.nextSize_)
	,	endChunk_(0)
    ,   endNode_(0)
#ifdef SMALL_OBJECT_ALLOCATOR_STATISTICS
	,	numAllocations_(0)
	,	numDeallocations_(0)
	,	maxNumActiveAllocations_(0)
#endif
	{
	}

	~block_allocator()
	{
#ifdef SMALL_OBJECT_ALLOCATOR_STATISTICS
		assert(numAllocations_ == numDeallocations_); // memory is free
#endif
		clear();
	}

    void clear()
    {
        while (endChunk_) 
		{
			byte_pointer chunk = (byte_pointer)endChunk_;
			endChunk_ = next_of(chunk);
            chunk -= size_of(chunk + sizeof(pointer)); // get pointer to the beginning of the chunk
			user_allocator::free(chunk);
		}
    }

    void deallocate(pointer node)
    {
#ifdef SMALL_OBJECT_ALLOCATOR_STATISTICS
		++numDeallocations_;
#endif
		next_of(node) = endNode_;
		endNode_ = node;
    }

	pointer allocate()
	{
#ifdef SMALL_OBJECT_ALLOCATOR_STATISTICS
		++numAllocations_;
		maxNumActiveAllocations_ = std::max(numAllocations_ - numDeallocations_, maxNumActiveAllocations_);
#endif
		if (!endNode_) {
			extend(1);
		}

		pointer allocated = endNode_;
		endNode_ = next_of(endNode_); 
		return allocated;
	}

    bool		empty() const			{ return endNode_ == 0; }
	size_type	block_size() const		{ return blockSize_; }
	size_type	next_size() const		{ return nextSize_; }
	size_type	page_size() const		{ return blocksPerPage_ + sizeof(pointer); }
    size_type	capacity() const		{ return capacity_; }

	void set_next_size(size_type nextSize)
	{
		assert(nextSize > 0);
		nextSize_ = nextSize;
	}
	
#ifdef SMALL_OBJECT_ALLOCATOR_STATISTICS
	size_type	num_allocations() const				{ return numAllocations_; }
	size_type	num_deallocations() const			{ return numDeallocations_; }
	size_type	max_num_active_allocations() const	{ return maxNumActiveAllocations_; }
#endif

	// allocators are always different
	bool operator == (const block_allocator& other) const { return this == &other; }
	bool operator != (const block_allocator& other) const { return this != &other; }

private:
	size_type	blockSize_;
    size_type	capacity_;
    size_type	nextSize_;
    pointer		endChunk_;
	pointer		endNode_;
#ifdef SMALL_OBJECT_ALLOCATOR_STATISTICS
	size_type	numAllocations_;
	size_type	numDeallocations_;
	size_type	maxNumActiveAllocations_;
#endif
};

} // namespace slon

#pragma pop_macro("min")
#pragma pop_macro("max")

#endif // __SLON_ENGINE_BLOCK_ALLOCATOR_H__