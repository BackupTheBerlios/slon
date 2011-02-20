#ifndef __SLON_ENGINE_ALGORITHM_HASH_STRING_H__
#define __SLON_ENGINE_ALGORITHM_HASH_STRING_H__

#include "referenced.hpp"
#include "Memory/object_in_pool.hpp"
#include <boost/intrusive_ptr.hpp>
#include <boost/unordered_map.hpp>
#include <iostream>
#include <string>

namespace slon {

/** String optimized for faster comparizon */
template< typename Char, 
          typename Traits = std::char_traits<Char>, 
          typename Allocator = std::allocator<Char> >
class basic_hash_string
{
friend class deleter;
public:
    typedef Allocator                               allocator_type;
    typedef typename allocator_type::value_type     value_type;
    typedef typename allocator_type::size_type      size_type;
    typedef Traits                                  traits_type;

private:
    typedef std::basic_string<Char, Traits>  string; 
    typedef const Char*                      cstring;

    struct string_holder;
    typedef boost::intrusive_ptr<string_holder> string_holder_ptr;

    struct string_holder :
        public object_in_pool<string_holder, referenced>
    {
        string_holder(cstring str_)
        :   size( strlen(str_) )
        ,   hash(0)
        ,   str( const_cast<value_type*>(str_) )
        ,   allocated(false)
        ,   parent(this)
        ,   rank(0)
        {
            // calculate hash
            for (size_type i = 0; i<size; ++i) {
                boost::hash_combine(hash, str[i]);
            }
        }

        string_holder(cstring str_, size_type size_, allocator_type alloc_)
        :   alloc(alloc_)
        ,   size(size_)
        ,   hash(0)
        ,   allocated(true)
        ,   parent(this)
        ,   rank(0)
        {
            str = alloc.allocate(size);

            // copy and calculate hash
            for (size_type i = 0; i<size; ++i)
            {
                str[i] = str_[i];
                boost::hash_combine(hash, str[i]);
            }
        }

        ~string_holder()
        {
            if (allocated) {
                alloc.deallocate(str, size);
            }
        }

        string_holder* root()
        {
            if (parent == this) {
                return this;
            }

            parent = parent->root();
            return parent.get();
        }

        void merge(string_holder* other)
        {
             string_holder* xRoot = root();
             string_holder* yRoot = other->root();
             if (xRoot->rank > yRoot->rank) {
                 yRoot->parent = xRoot;
             }
             else if (xRoot != yRoot)
             {
                 xRoot->parent = yRoot;
                 if (xRoot->rank == yRoot->rank) {
                     yRoot->rank = yRoot->rank + 1;
                 }
             }
        }

        allocator_type  alloc;
        size_type       size;
        size_type       hash;
        value_type*     str;
        bool            allocated;

        // DSU
        mutable string_holder_ptr   parent; // can hold this pointer, but this is handled in ~basic_hash_string
        mutable size_t              rank;
    };
    
public:
    basic_hash_string(allocator_type alloc = allocator_type())
    :   holder(new string_holder(""))
    {}

    basic_hash_string(const basic_hash_string& other)
    :   holder(other.holder)
    {
    }

    basic_hash_string(cstring str, allocator_type alloc = allocator_type())
    :   holder(new string_holder(str, strlen(str) + 1, alloc))
    {
    }

    basic_hash_string(const string& str, allocator_type alloc = allocator_type())
    :   holder(new string_holder(str.c_str(), str.length() + 1, alloc))
    {
    }

    ~basic_hash_string()
    {
        if (holder->parent == holder && holder->use_count() == 2) {
            holder->parent.reset(); // broke recurrent link
        }
    }

    string  str() const   { return holder ? string(holder->str) : string(); }
    cstring c_str() const { return holder ? holder->str : ""; }

    /** Check if string are in the same set(using disjoint sets), if not compare them using hash, then compare per symbol. 
     * If strings are equal merge their sets.
     */
    bool operator == (const basic_hash_string& rhs) const 
    { 
        if (holder->root() == rhs.holder->root()) {
            return true;
        }

        if (holder->hash != rhs.holder->hash) {
            return false;
        }

        if ( (holder->size == rhs.holder->size) && (traits_type::compare(holder->str, rhs.holder->str, holder->size) == 0) ) 
        {
            holder->merge(rhs.holder.get());
            return true;
        }

        return false;
    }

    bool operator != (const basic_hash_string& rhs) const 
    { 
        return !(*this == rhs);
    }
    
    /** Compare strings, doesn't perform lexicographical compare, but can be used for containter predicate. */
    bool operator < (const basic_hash_string& rhs) const 
    { 
        if (holder->hash < rhs.holder->hash) {
            return true;
        }
        else if (holder->hash > rhs.holder->hash) {
            return false;
        }

        if (holder->size < rhs.holder->size) {
            return true;
        }
        else if (holder->size > rhs.holder->size) {
            return false;
        }

        if ( holder->root() == rhs.holder->root() ) {
            return false;
        }

        int cmp = traits_type::compare(holder->str, rhs.holder->str, holder->size);
        if (cmp == 0) {
            holder->merge(rhs.holder.get());
        }
            
        return cmp < 0;
    } 

    friend std::ostream& operator << (std::ostream& os, basic_hash_string str)
    {
        return os << str.c_str();
    }

    friend std::istream& operator >> (std::istream& os, basic_hash_string str)
    {
        string s; is >> s;
        (*this) = basic_hash_string(s);
        return is;
    }

private:
    mutable string_holder_ptr holder;
};

typedef basic_hash_string< char, std::char_traits<char> >       hash_string;
typedef basic_hash_string< wchar_t, std::char_traits<wchar_t> > hash_wstring;

} // namespace slon

#endif // __SLON_ENGINE_ALGORITHM_HASH_STRING_H__
