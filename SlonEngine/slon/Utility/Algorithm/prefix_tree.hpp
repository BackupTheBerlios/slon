#ifndef __SLON_ENGINE_UTILITY_PREFIX_TREE_HPP__
#define __SLON_ENGINE_UTILITY_PREFIX_TREE_HPP__

#include <boost/array.hpp>
#include <boost/integer_traits.hpp>
#include <boost/iterator/iterator_facade.hpp>
#include <boost/noncopyable.hpp>
#include <boost/scoped_array.hpp>
#include <boost/scoped_ptr.hpp>
#include <algorithm>
#include <iterator>
#include <limits>
#include <vector>
#include <utility>

namespace slon {

template< typename T, typename Allocator = std::allocator<T> >
class memory_mapper :
    public boost::noncopyable
{
public:
    typedef memory_mapper<T, Allocator>     this_type;

    typedef T		    value_type;
	typedef Allocator	allocator_type;
    typedef size_t	    size_type;

private:
    typedef std::vector<value_type*>        value_ptr_vector;

    struct value_chunk
    {
        size_type       size;
        value_type*     data;
        value_chunk*    prev;

        value_chunk(size_type    size_, 
                    value_type*  data_,
                    value_chunk* prev_)
        :   size(size_)
        ,   data(data_)
        ,   prev(prev_)
        {}
    };

private:
    static size_type size_hint(size_type size)
    {
        return size * 3 / 2 + 10;
    }

    void make_unoccupied(value_type* begin, value_type* end)
    {
        for (; begin != end; ++begin) {
            unoccupied_.push_back(begin);
        }
    }

    value_chunk* extend(value_chunk* prev, size_type size)
    {
        value_type* data = allocator_.allocate(size, prev ? prev->data : 0);
        make_unoccupied(data, data + size);
            
        // FIXME: May leak data if new throws
        return new value_chunk(size, data, prev);
    }

public:
    memory_mapper(size_type             capacity_  = 0,
                  const allocator_type& allocator_ = allocator_type() )
    :   capacity_(capacity_)
    ,   end_(0)
    ,   allocator_(allocator_)
    {
    }

    ~memory_mapper()
    {
        clear();
    }

    void reserve(size_type newCapacity)
    {
        if (capacity_ >= newCapacity) {
            return;
        }

        end_      = extend(end_, newCapacity - capacity_);
        capacity_ = newCapacity;
    }

    void clear()
    {
        while (end_) 
        {
            allocator_.deallocate(end_->data, end_->size);                
            value_chunk* p = end_->prev;
            delete end_;
            end_ = p;
        }
    }

	value_type* construct(const value_type& clone = value_type())
	{
        if ( unoccupied_.empty() ) {
            reserve( size_hint(capacity_) );
        }

        value_type* position = unoccupied_.back();
        allocator_.construct(position, clone);
        unoccupied_.pop_back();

        return position;
	}

	void destroy(value_type* position)
	{
        assert( !empty() );
        allocator_.destroy(position);
        unoccupied_.push_back(position);
	}

    bool      empty() const    { return size() == 0; }
    size_type size() const     { return capacity_ - unoccupied_.size(); }
    size_type capacity() const { return capacity_; }

private:
    size_type           capacity_;
    value_chunk*        end_;
    allocator_type      allocator_;
    value_ptr_vector    unoccupied_;
};

template<typename T>
struct label_traits
{
    static const T min = boost::integer_traits<T>::const_min;
    static const T max = boost::integer_traits<T>::const_max;
};

template< typename LabelType,
          typename MappedType,
		  typename Compare = std::less<LabelType>,
          typename LabelTraits = label_traits<LabelType>,
		  typename Allocator = std::allocator< std::pair< const std::basic_string<LabelType>, MappedType > > >
class prefix_tree :
    public boost::noncopyable
{
public:
    typedef prefix_tree<LabelType, 
						MappedType,
						Compare,
						LabelTraits,
						Allocator>                  this_type;

    typedef LabelType								label_type;
    typedef LabelTraits								label_traits;
    typedef std::basic_string<label_type>			key_type;
	typedef MappedType								mapped_type;
    typedef std::pair<const key_type, mapped_type>	value_type;
	typedef Allocator								allocator_type;
    typedef size_t									size_type;

    typedef value_type&                             reference;
    typedef const value_type&                       const_reference;

private:
    static const size_t value_count = label_traits::max - label_traits::min + 1;

    struct  prefix;
    typedef boost::array<prefix*, value_count>	        prefix_ptr_array;

    typedef memory_mapper<value_type, allocator_type>   element_mapper_type;
    typedef memory_mapper<prefix>                       prefix_mapper_type;

    class prefix
    {
	public:
        prefix()
        :   value(0)
        ,   parent(0)
        ,   numChildren(0)
        {
            std::fill(children.begin(), children.end(), (prefix*)0);
		}

        void swap(prefix &other)
        {
            std::swap(value, other.value);
            label.swap(other.label);
            std::swap(parent, other.parent);
            std::swap(numChildren, other.numChildren);
            children.swap(other.children);
        }

		~prefix()
		{
			if (parent) 
			{
				--parent->numChildren;
				parent->children[label[0] - label_traits::min] = 0;
			}
		}

	public:
		key_type	        label;
		value_type*	        value;
        prefix*             parent;
        size_t              numChildren;
        prefix_ptr_array    children;
    };

public:
    /** DFS iterator over prefix tree */
    class iterator :
        public boost::iterator_facade< iterator,
                                       value_type,
                                       boost::bidirectional_traversal_tag >
    {
    public:
        iterator()
        :   t(0)
		,   p(0)
        {}

        explicit iterator(prefix_tree* t_, prefix* p_)
		:   t(t_)
        ,   p(p_)
        {}

    private:
        friend class boost::iterator_core_access;

        void increment() { p = t->next_leaf(p); }
        void decrement() { p = t->prev_leaf(p); }

        bool equal(iterator other) const
        {
			assert(t == other.t && "iterators are not compatible");
            return p == other.p;
        }

        value_type& dereference() const { return *p->value; }

    public:
		prefix_tree* t;
        prefix*      p;
    };

    /** DFS const iterator over prefix tree */
    class const_iterator :
        public boost::iterator_facade< const_iterator,
                                       const value_type,
                                       boost::bidirectional_traversal_tag >
    {
    public:
        const_iterator()
        :   t(0)
		,   p(0)
        {}

        explicit const_iterator(const prefix_tree* t_, const prefix* p_)
		:   t(t_)
        ,   p(p_)
        {}

    private:
        friend class boost::iterator_core_access;

        void increment() { p = t->next_leaf(p); }
        void decrement() { p = t->prev_leaf(p); }

        bool equal(const_iterator other) const
        {
			assert(t == other.t && "iterators are not compatible");
            return p == other.p;
        }

        const value_type& dereference() const { return *p->value; }

    public:
		const prefix_tree* t;
        const prefix*      p;
    };

private:
    template<typename LabelIterator>
    int compare_label(prefix*           p,
                      LabelIterator&    beginLabel,
                      LabelIterator&    endLabel)
    {
        size_t i;
        for (i = 0; i<p->label.size() && beginLabel != endLabel; ++i, ++beginLabel)
        {
            if (p->label[i] != *beginLabel) {
                return i;
            }
        }

        return (i == p->label.size() && beginLabel == endLabel) ? -1 : i;
    }

    static size_t wrap(label_type label)
    {
        return label - label_traits::min;
    }

	template<typename PrefixIterator>
    typename std::iterator_traits<PrefixIterator>::value_type first_non_empty(PrefixIterator begin, PrefixIterator end)
    {
        while ( begin != end && !(*begin) ) {
            ++begin;
        }

        return (begin == end) ? 0 : *begin;
    }

    prefix* next_leaf(prefix* p)
    {
        prefix* next = 0;
        while (p->parent && !next)
        {
            next = first_non_empty( p->parent->children.begin() + wrap(p->label[0]) + 1,
                                    p->parent->children.begin() + value_count );
            p    = p->parent;
        }

        while ( next && !next->value )
        {
            next = first_non_empty( next->children.begin(),
                                    next->children.begin() + value_count );
        }

        return next;
    }
	
    const prefix* next_leaf(const prefix* p) const
    {
        return const_cast<prefix_tree*>(this)->next_leaf( const_cast<prefix*>(p) );
    }

    prefix* prev_leaf(prefix* p)
    {
        prefix* prev = 0;
        while (p->parent && !prev)
        {
            prev = first_non_empty( p->parent->children.get(),
                                    p->parent->children.get() + wrap(p->label[0]) );
            p    = p->parent;
        }

        while ( prev && !prev->isLeaf )
        {
            prev = first_non_empty( std::reverse_iterator<prefix*>(prev->children.get() + value_count),
                                    std::reverse_iterator<prefix*>(prev->children.get()) );
        }

        return prev;
    }
	
    const prefix* prev_leaf(const prefix* p) const
    {
        return const_cast<prefix_tree*>(this)->prev_leaf( const_cast<prefix*>(p) );
    }

    prefix* make_child_prefix(prefix*            p,
							  const key_type&	 label)
    {
		assert( !label.empty() );

        size_t child_id               = wrap(label[0]);
        p->children[child_id]         = prefixMapper.construct();
        p->children[child_id]->parent = p;
        p->children[child_id]->label  = label;
        ++p->numChildren;

        return p->children[child_id];
    }

    void split_prefix(prefix* p, int split)
    {
		size_t s_id = wrap(p->label[split]);

        prefix* inserted      = prefixMapper.construct();
		inserted->label       = key_type( p->label.begin() + split, p->label.end() );
		inserted->parent      = p;
		inserted->value       = p->value;
		inserted->numChildren = p->numChildren;
		p->label              = key_type( p->label.begin(), p->label.begin() + split );
		p->value              = 0;
		p->numChildren        = 1;
		p->children.swap(inserted->children);
		p->children[s_id]     = inserted;

		// redirect children
		for (size_type i = 0; i<value_count; ++i)
		{
			if (inserted->children[i]) {
				inserted->children[i]->parent = inserted;
			}
		}
    }

    void erase_prefix(prefix* p)
    {
        assert(p);

		if (p->value) 
		{
            valueMapper.destroy(p->value);
			p->value = 0;
		}

        if (p->numChildren == 0)
        {
            if (prefix* parent = p->parent)
            {
                prefixMapper.destroy(p);

                if (parent->numChildren == 0 && !parent->value) {
                    erase_prefix(parent);
                }
            }
			else 
			{
				assert(prefixMapper.size() == 1);
                prefixMapper.destroy(p);
                root = 0;
			}
        }
        else if (p->numChildren == 1)
        {
            prefix* child = first_non_empty( p->children.begin(), p->children.end() );

            // concat label & copy content from child
            p->children.swap(child->children);
			p->label      += child->label;
            p->numChildren = child->numChildren;
			p->value       = child->value;
			child->parent  = 0;

            // redirect children
            for (size_type i = 0; i<value_count; ++i) 
            {
                if (p->children[i]) {
                    p->children[i]->parent = p;
                }
            }

			prefixMapper.destroy(child);
        }
    }

    template<typename LabelIterator>
    prefix* find_nearest_prefix(LabelIterator beginLabel,
                                LabelIterator endLabel)
    {
		if (!root) {
			return 0;
		}

        prefix* prev = root;
        prefix* p = root;
        int     diff;
        while (true)
        {
            diff = compare_label(p, beginLabel, endLabel);
            if (diff == -1 && beginLabel == endLabel) {
                return p;
            }
            else if (diff == (int)p->label.size())
            {
                p = p->children[wrap(*beginLabel)];
                if (!p) {
                    return prev;
                }
            }
            else {
                return prev;
            }
            prev = p;
        }

        return prev;
    }

    template<typename LabelIterator>
    prefix* find_prefix(LabelIterator beginLabel,
                        LabelIterator endLabel)
    {
		if ( prefixMapper.empty() ) {
			return 0;
		}

        prefix* p = root;
        int     diff;
        while (p)
        {
            diff = compare_label(p, beginLabel, endLabel);
            if (diff == -1 && beginLabel == endLabel) {
                return p;
            }
            else if (diff == (int)p->label.size()) {
                p = p->children[wrap(*beginLabel)];
            } 
            else {
                return 0;
            }
        }

        return p;
    }

public:
    prefix_tree( const allocator_type& allocator = allocator_type() )
    :   root(0)
	,	valueMapper(0, allocator)
    {
    }

    /** Find nearest leaf element in the prefix tree. E.g. you are looking for "C:/Files/file1.txt" key, but
     * prefix tree only have element with "C:/Files" key, so it will be returned.
     * @tparam KeyType - label sequence type.
     * @return iterator to found element or end iterator.
     */
    iterator find_nearest(const key_type& key)
    {
        prefix* p = find_nearest_prefix(key.begin(), key.end());
        if (p && p->value) {
            return iterator(this, p);
        }

        return iterator(this, 0);
    }

    /** Find nearest leaf element in the prefix tree. E.g. you are looking for "C:/Files/file1.txt" key, but
     * prefix tree only have element with "C:/Files" key, so it will be returned.
     * @tparam KeyType - label sequence type.
     * @return iterator to found element or end iterator.
     */
    const_iterator find_nearest(const key_type& key) const
    {
        prefix* p = find_nearest_prefix(key.begin(), key.end());
        if (p && p->value) {
            return const_iterator(this, p);
        }

        return const_iterator(this, 0);
    }

    /** Find element in the prefix tree. */
    iterator find(const key_type& key)
    {
        prefix* p = find_prefix(key.begin(), key.end());
        if (p && p->value) {
            return iterator(this, p);
        }

        return iterator(this, 0);
    }

    /** Find element in the prefix tree.*/
    const_iterator find(const key_type& key) const
    {
        prefix* p = const_cast<this_type*>(this)->find_prefix(key.begin(), key.end());
        if (p && p->value) {
            return const_iterator(p);
        }

        return const_iterator(this, 0);
    }

    /** Get first element in the prefix tree. */
    iterator begin() { return iterator(this, root); }

    /** Get first element in the prefix tree. */
    const_iterator begin() const { return const_iterator(this, root); }

    /** Get iterator addressing element after last in the prefix tree. */
    iterator end() { return iterator(this, 0); }

    /** Get iterator addressing element after last in the prefix tree. */
    const_iterator end() const { return const_iterator(this, 0); }

    /** Insert element in the prefix tree. */
    std::pair<iterator, bool> insert(value_type value)
    {
		// insert value into root
		if ( prefixMapper.empty() )
		{
            root        = prefixMapper.construct();
			root->label = value.first;
            root->value = valueMapper.construct(value);
			return std::pair<iterator, bool>(iterator(this, root), true);
		}

        // find prefix nearest to requested for insertion
		prefix* p    = root;
        int     diff = -1;

        typename key_type::const_iterator beginLabel = value.first.begin();
        typename key_type::const_iterator endLabel   = value.first.end();
        while (!value.first.empty() && p)
        {
            diff = compare_label(p, beginLabel, endLabel);
            if (diff == (int)p->label.size() && p->children[wrap(*beginLabel)]) {
                p = p->children[wrap(*beginLabel)];
            }
            else {
                break;
            }
        }

        // prefix is in tree
        if (beginLabel == endLabel)
        {
            if (diff == -1)
            {
                if (p->value) {
                    return std::pair<iterator, bool>(iterator(this, p), false);
                }
                else
                {
					p->value = valueMapper.construct(value);
                    return std::pair<iterator, bool>(iterator(this, p), true);
                }
            }
            else
            {
                split_prefix(p, diff);
				p->value = valueMapper.construct(value);
                return std::pair<iterator, bool>(iterator(this, p), true);
            }
        }
        else if ( diff != (int)p->label.size() ) {
            split_prefix(p, diff);
        }

        // allocate new prefix
        p = make_child_prefix( p, key_type(beginLabel, endLabel) );
        p->value = valueMapper.construct(value);
        return std::pair<iterator, bool>(iterator(this, p), true);
    }

    /** Remove element. */
    void erase(iterator position)
    {
        assert(position.p && position.p->value);
        erase_prefix(position.p);
    }
	
    /** Remove element. */
    void erase(const key_type& key)
    {
        iterator it = find(key);
        if ( it != end() ) {
            erase(it);
        }
    }

    /** Remove all elements from the prefix tree. */
    void clear()
    {
        root = 0;
		prefixMapper.clear();
        valueMapper.clear();
    }

    /** Swap content of prefix tree with other. */
    void swap(this_type& other) throw()
    {
		prefixMapper.swap(other.prefixMapper);
        valueMapper.swap(other.valueMapper);
    }

private:
    prefix*             root;
    prefix_mapper_type	prefixMapper;
    element_mapper_type valueMapper;
};

} // namespace slon

#endif // __SLON_ENGINE_UTILITY_PREFIX_TREE_HPP__
