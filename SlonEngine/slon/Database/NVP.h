#ifndef __SLON_ENGINE_DATABASE_NVP_H__
#define __SLON_ENGINE_DATABASE_NVP_H__

#include "../Utility/if_then_else.hpp"
#include "Forward.h"
#include <boost/type_traits/is_fundamental.hpp>

// forward stl types
namespace std 
{
    template<typename T, typename A>
    class vector;

    template<typename T, typename A>
    class list;
        
    template<typename K, typename T, typename C, typename A>
    class map;

    template<typename K, typename T, typename C, typename A>
    class multimap;

    template<typename T, typename C, typename A>
    class set;

    template<typename T, typename C, typename A>
    class multiset;
} // namespace stl

// forward boost types
namespace boost
{
    template<typename K, typename T, typename H, typename C, typename A>
    class unordered_map;

    template<typename K, typename T, typename H, typename C, typename A>
    class unordered_multimap;

    template<typename K, typename H, typename P, typename A>
    class unordered_set;

    template<typename K, typename H, typename P, typename A>
    class unordered_multiset;
} // namespace boost

namespace slon {
namespace database {

/** Base class for name value pairs - objects which store some named data (e.g. class field). */
class NVP
{
public:
    NVP(const std::string& name_)
    :   name(name_)
    {}

    /** Save/Read NVP data to/from arhive. */
    virtual void accept(Archive& ar) = 0;

    /** Get NVP name. */
    virtual const std::string& getName() const { return name; } 

    virtual ~NVP() {}

protected:
    std::string name;
};

// declare container traits to simplify specialization
namespace traits {

    struct unknown_tag {};
    struct builtin_tag {};
    struct pointer_tag {};
    struct container_tag {};

    // general traits
    template <typename T>
    struct type_traits
    {
        typedef if_then_else< boost::is_fundamental<T>::value, 
                              builtin_tag, 
                              unknown_tag >::type   type_tag;
    };

    // pointer traits
    template <typename T>
    struct type_traits<T*>
    {
        typedef pointer_tag type_tag;
    };

    template <typename T>
    struct type_traits< boost::shared_ptr<T> >
    {
        typedef pointer_tag type_tag;
    };

    template <typename T>
    struct type_traits< boost::intrusive_ptr<T> >
    {
        typedef pointer_tag type_tag;
    };

    // container traits
    struct array_tag            {};
    struct have_push_back_tag   {};
    struct have_insert_tag      {};

    template<typename T, typename A>
    struct type_traits< std::vector<T, A> >
    {
        typedef container_tag       type_tag;
        typedef array_tag           container_insert_tag;
    };

    template<typename T, typename A>
    struct type_traits< std::list<T, A> >
    {
        typedef container_tag       type_tag;
        typedef have_push_back_tag  container_insert_tag;
    };

    template<typename T, typename A>
    struct type_traits< std::deque<T, A> >
    {
        typedef container_tag       type_tag;
        typedef have_push_back_tag  container_insert_tag;
    };

    template<typename K, typename T, typename C, typename A>
    struct type_traits< std::map<K, T, C, A> >
    {
        typedef container_tag       type_tag;
        typedef have_insert_tag     container_insert_tag;
    };

    template<typename K, typename T, typename C, typename A>
    struct type_traits< std::multimap<K, T, C, A> >
    {
        typedef container_tag       type_tag;
        typedef have_insert_tag     container_insert_tag;
    };

    template<typename K, typename T, typename H, typename C, typename A>
    struct type_traits< boost::unordered_map<K, T, H, C, A> >
    {
        typedef container_tag       type_tag;
        typedef have_insert_tag     container_insert_tag;
    };

    template<typename K, typename T, typename H, typename C, typename A>
    struct type_traits< boost::unordered_multimap<K, T, H, C, A> >
    {
        typedef container_tag       type_tag;
        typedef have_insert_tag     container_insert_tag;
    };

    template<typename T, typename C, typename A>
    struct type_traits< std::set<T, C, A> >
    {
        typedef container_tag       type_tag;
        typedef have_insert_tag     container_insert_tag;
    };

    template<typename T, typename C, typename A>
    struct type_traits< std::multiset<T, C, A> >
    {
        typedef container_tag       type_tag;
        typedef have_insert_tag     container_insert_tag;
    };

    template<typename K, typename H, typename P, typename A>
    struct type_traits< boost::unordered_set<K, H, P, A> >
    {
        typedef container_tag       type_tag;
        typedef have_insert_tag     container_insert_tag;
    };

    template<typename K, typename H, typename P, typename A>
    struct type_traits< boost::unordered_multiset<K, H, P, A> >
    {
        typedef container_tag       type_tag;
        typedef have_insert_tag     container_insert_tag;
    };

} // namespace traits

/** NVP to serialize builtin types. */
template<typename T>
class BuiltinNVP
:   public NVP
{
public:
    typedef T value_type;

public:
    BuiltinNVP(const std::string& name_, value_type& value_)
    :   name(name_)
    ,   value(value_)
    {}

    void accept(Archive& ar)
    {
        ar.beginChunk( name.c_str() );
        ar & value;
        ar.endChunk();
    }

protected:
    value_type& value;
};

/** NVP to serialize strings. */
template<typename CharT>
class StringNVP
:   public NVP
{
public:
    typedef CharT                       char_type;
    typedef std::basic_string<CharT>    string_type;

public:
    StringNVP(const std::string& name_, string_type& str_)
    :   name(name_)
    ,   str(str_)
    {}

    void accept(IArchive& ar)
    {
        IArchive::chunk_info info;
        if ( ar.openChunk(name, info) && info.isLeaf )
        {
            str.resize(info.size);
            ar.readString(&str[0], info.size);
            ar.closeChunk();
        }
        else {
            throw serialization_error(log::logger_ptr(), "Trying to read string from invalid chunk.");
        }
    }

    void accept(OArchive& ar)
    {
        ar.openChunk( name.c_str() );
        ar.writeString( str.data(), str.size() );
        ar.closeChunk();
    }

protected:
    string_type& str;
};

/** NVP to serialize container types. */
template<typename Container>
class ContainerNVP
:   public NVP
{
public:
    typedef Container container_type;

public:
    ContainerNVP(const std::string& name_, container_type& container_)
    :   name(name_)
    ,   container(container_)
    {}

    void accept(Archive& ar)
    {
        accept<traits::container_traits<container_type>::tag>(ar);
    }

protected:
    template<typename Tag>
    void accept(Archieve& ar);

    template<>
    void accept<traits::array_tag>(IArchieve& ar)
    {
        IArchive::chunk_info info;
        if ( ar.openChunk(name, info) && info.isLeaf )
        {
            str.resize(info.size);
            ar.readString(&str[0], info.size);
            ar.closeChunk();
        }
        else {
            throw serialization_error(log::logger_ptr(), "Trying to read string from invalid chunk.");
        }
        if ( ar.openChunk(name, info) && info.isLeaf)
        {
        }
        else {
        }

        if ( ar.isOutput() )
        {
            for (iterator it = container.begin(); it != container.end(); ++it) {
                ar & (*it);
            }
        }
        else 
        {
            std::back_insert_iterator<Contaiter> it(container);
            while ( !ar.endChunk() ) {
                ar & (*it++);
            }
        }
        ar.endChunk();
    }

    template<>
    void accept<traits::have_push_back_tag>(Archieve& ar)
    {
        ar.beginChunk( name.c_str() );
        if ( ar.isOutput() )
        {
            for (iterator it = container.begin(); it != container.end(); ++it) {
                ar & (*it);
            }
        }
        else 
        {
            std::back_insert_iterator<Contaiter> it(container);
            while ( !ar.endChunk() ) {
                ar & (*it++);
            }
        }
        ar.endChunk();
    }

    template<>
    void accept<traits::have_insert_tag>(Archieve& ar)
    {
        ar.beginChunk( name.c_str() );
        if ( ar.isOutput() )
        {
            for (iterator it = container.begin(); it != container.end(); ++it) {
                ar & (*it);
            }
        }
        else 
        {
            std::insert_iterator<Contaiter> it(container);
            while ( !ar.endChunk() ) {
                ar & (*it++);
            }
        }
        ar.endChunk();
    }

protected:
    container_type& container;
};

/** NVP to serialize serializables. */
template<typename PtrT>
class SerializableNVP
:   public NVP
{
public:
    typedef PtrT pointer_type; 

public:
    SerializableNVP(const std::string& name_, pointer_type& serializable_)
    :   name(name_)
    ,   serializable(serializable_)
    {}

    void accept(OArchive& ar)
    {
        if (serializable)
        {
            if ( int refId = ar.registerReference(&*serializable) ) 
            {
                ar.beginChunk(name, refId);
                serializable->serialize( ar, ar.getVersion() );
                ar.endChunk();
            }
            else {
                ar.writeReference(&*serializable);
            }
        }
    }

    void accept(IArchive& ar)
    {
        serializable = pointer_type( ar.readReference(name) );
        if ( !serializable && ar.beginChunk(name) ) {
            serializable->serialize( ar, ar.getVersion() );
        }
        else {
            throw serializer_error(log::logger_ptr(), "Current chunk is not compatible with requested type");
        }
    }

protected:
    Serializable*& serializable;
};

template<typename PtrT, typename T>
class PointerNVP
:   public NVP
{
public:
    typedef T    value_type;
    typedef PtrT pointer_type;

public:
    PointerNVP(const std::string& name_, pointer_type& ptr_)
    :   name(name_)
    ,   ptr(ptr_)
    {}

    void accept(OArchive& ar)
    {
        if (ptr)
        {
            if ( ar.registerReference(&*ptr) ) 
            {
                ar.beginChunk(name);
                ar << (*ptr);
                ar.endChunk();
            }
            else {
                ar.writeReference(&*ptr);
            }
        }
    }

    void accept(IArchive& ar)
    {
        ptr = pointer_type( reinterpret_cast<value_type*>( ar.readReference(name) ) );
        if ( !ptr && ar.beginChunk(name) ) 
        {
            ptr = pointer_type(new T);
            ar
        }
        else {
            throw serializer_error(log::logger_ptr(), "Current chunk is not compatible with requested type");
        }
    }

protected:
    pointer_type& ptr;
};

} // namespace database
} // namespace slon

#endif // __SLON_ENGINE_DATABASE_NVP_H__
