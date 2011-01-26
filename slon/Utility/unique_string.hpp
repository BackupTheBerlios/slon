#ifndef __SLON_ENGINE_ALGORITHM_UNIQUE_STRING_H__
#define __SLON_ENGINE_ALGORITHM_UNIQUE_STRING_H__

#include "singleton.hpp"
#include "referenced.hpp"
#include <boost/intrusive_ptr.hpp>
#include <boost/unordered_map.hpp>
#include <cstring>
#include <string>

namespace slon {

/** Stores unique pointer for every cstring */
class unique_string
{
private:
    struct string_holder;

    typedef boost::unordered_map<std::string, string_holder*>   string_prefix_tree;
    typedef singleton<string_prefix_tree>						prefix_tree_singleton;

    struct string_holder :
        public referenced
    {
        string_holder(const std::string& str_, prefix_tree_singleton::holder_ptr holder_) :
            str(str_),
            holder(holder_)
        {
        }

        ~string_holder()
        {
            string_prefix_tree::iterator pos = holder->find(str);
            holder->erase(pos);
        }

        std::string                         str;
        prefix_tree_singleton::holder_ptr   holder;
    };
    typedef boost::intrusive_ptr<string_holder> string_holder_ptr;

public:
    unique_string() {}

    explicit unique_string(const char* str)
    {
        if (str) {
            init(str);
        }
    }

    explicit unique_string(const std::string& str)
    {
        init(str);
    }

    bool operator == (const unique_string& rhs) const { return stringHolder == rhs.stringHolder; }
    bool operator != (const unique_string& rhs) const { return stringHolder != rhs.stringHolder; }

    /** Convert unique_string to cstring */
    operator const char* () const { return stringHolder ? stringHolder->str.c_str() : 0; }

private:
    void init(const std::string& str)
    {
        string_prefix_tree* prefixTree = prefix_tree_singleton::instance();
        string_prefix_tree::iterator iter = prefixTree->find(str);
        if ( iter != prefixTree->end() ) {
            stringHolder.reset(iter->second);
        }
        else
        {
            stringHolder.reset( new string_holder(str, prefix_tree_singleton::shared_instance()) );
			(*prefixTree)[str] = stringHolder.get();
        }
    }

private:
    string_holder_ptr stringHolder;
};

} // namespace slon

#endif // __SLON_ENGINE_ALGORITHM_UNIQUE_STRING_H__
