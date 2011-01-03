#pragma once
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
class block_allocator
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
	pointer& get_node_ptr(pointer ptr)
	{
		return *reinterpret_cast<pointer*>(ptr);
	}

	void link_nodes(byte_pointer begin, byte_pointer end)
	{
		if (begin != end)
		{
			// each node points to next node
			endNode_ = begin;
			while (begin != end)
			{
				get_node_ptr(begin) = begin + blockSize_;
				begin += blockSize_;
			}
			get_node_ptr(end - blockSize_) = 0;
		}
	}

    void extend(size_type size)
    {
		byte_pointer chunk  = user_allocator::malloc( size * blockSize_ + sizeof(pointer) );
		capacity_		   += size;
		get_node_ptr(chunk) = endChunk_;
        endChunk_           = chunk;
		chunk              += sizeof(pointer);
		
		if (endNode_) {
			get_node_ptr(endNode_) = chunk;
		}
        link_nodes(chunk, chunk + size * blockSize_);
    }

	// noncopyable
	block_allocator& operator = (const block_allocator& other);

public:
    block_allocator(size_type blockSize, 
					size_type capacity = 0,
					size_type blocksPerPage = 4094)
    :   blockSize_(blockSize)
	,	capacity_(0)
	,	blocksPerPage_(blocksPerPage)
	,	endChunk_(0)
    ,   endNode_(0)
#ifdef SMALL_OBJECT_ALLOCATOR_STATISTICS
	,	numAllocations_(0)
	,	numDeallocations_(0)
	,	maxNumActiveAllocations_(0)
#endif
    {
		assert( blockSize_ >= sizeof(pointer) && blockSize_ % sizeof(pointer) == 0 && blocksPerPage_ > 0 );
		if (capacity > 0) {
			extend(capacity);
		}
    }

	block_allocator(const block_allocator& other)
    :   blockSize_(other.blockSize_)
	,	capacity_(0)
	,	blocksPerPage_(other.blocksPerPage_)
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
		assert(numDeallocations_ == numDeallocations_); // memory is free
#endif
		clear();
	}

    void clear()
    {
        while (endChunk_) 
		{
			byte_pointer chunk = (byte_pointer)endChunk_;
			endChunk_ = get_node_ptr(endChunk_);
			user_allocator::free(chunk);
		}
    }

    void deallocate(pointer node)
    {
#ifdef SMALL_OBJECT_ALLOCATOR_STATISTICS
		++numDeallocations_;
#endif
		get_node_ptr(node) = endNode_;
		endNode_ = node;
    }

	pointer allocate()
	{
#ifdef SMALL_OBJECT_ALLOCATOR_STATISTICS
		++numAllocations_;
		maxNumActiveAllocations_ = std::max(numAllocations_ - numDeallocations_, maxNumActiveAllocations_);
#endif
		if (!endNode_) {
			extend(blocksPerPage_);
		}

		pointer allocated = endNode_;
		endNode_ = get_node_ptr(endNode_); 
		return allocated;
	}

    bool		empty() const			{ return endNode_ == 0; }
	size_type	block_size() const		{ return blockSize_; }
	size_type	blocks_per_page() const	{ return blocksPerPage_; }
	size_type	page_size() const		{ return blocksPerPage_ + sizeof(pointer); }
    size_type	capacity() const		{ return capacity_; }

	void set_blocks_per_page(size_type blocksPerPage)
	{
		assert(blocksPerPage > 0);
		blocksPerPage_ = blocksPerPage;
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
    size_type	blocksPerPage_;
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
