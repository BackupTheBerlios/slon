#ifndef __SLON_ENGINE_DATABASE_COLLADA_COLLADA_COMMON_H__
#define __SLON_ENGINE_DATABASE_COLLADA_COLLADA_COMMON_H__

#include <boost/shared_ptr.hpp>
#include <boost/unordered_map.hpp>
#include <functional>
#include <sgl/Math/MatrixFunctions.hpp>
#include <sgl/Math/Quaternion.hpp>
#include <sgl/Math/Utility.hpp>
#include <sstream>
#include <xml++/serialization/serialization.hpp>
#include "../../Log/Logger.h"
#include "../../Utility/error.hpp"
#include "../Forward.h"

namespace slon {
namespace database {

// Forward decl
class ColladaDocument;

// misc typedefs
typedef std::binary_function<const ColladaDocument&, const xmlpp::node&, void> 	collada_loader_function;
typedef xmlpp::generic_loader<ColladaDocument> 									collada_loader;
typedef xmlpp::generic_saver<ColladaDocument> 									collada_saver;

typedef xmlpp::generic_serializer<ColladaDocument>                              collada_serializer;

/** Error class occuring during the parsing collada document */
class collada_error :
    public slon_error
{
public:
    collada_error( const log::logger_ptr&   logger,
                   const std::string&		message,
                   log::SEVERITY			severity = log::S_ERROR ) :
        slon_error(logger, message, severity)
    {}

    collada_error( const log::logger_ptr&	logger,
                   const std::string&		message,
                   const xmlpp::node&		_node,
                   log::SEVERITY			severity = log::S_ERROR ) :
        slon_error(logger, message, severity),
        node(_node)
    {}

    /** Get the broken node */
    xmlpp::node& getNode() { return node; }

private:
    xmlpp::node node;
};

template<typename T>
class collada_optional
{
public:
    collada_optional(const T& value_ = T(), bool specified_ = false) :
        value(value_),
        specified(specified_)
    {}

    operator T () const     { return value; }
    operator bool () const  { return specified; }

    T& operator = (const T& value_)
    {
        value = value_;
        specified = true;
        return value;
    }

    template<typename Holder>
	void load(const ColladaDocument& document, const Holder& holder)
	{
		value.load(document, holder);
        specified = true;
	}

    template<typename Holder>
	void save(ColladaDocument& document, Holder& holder) const
	{
        if (specified) {
            value.save(document, holder);
        }
	}

    template<typename Holder>
    void serialize(ColladaDocument& d, Holder& n, xmlpp::s_state state)
    {
        if (state == xmlpp::LOAD) load(d, n);
        else save(d, n);
    }

public:
    T       value;
    bool    specified;
};

/** Error class occuring during visual or physics scene construction */
class construct_scene_error :
	public slon_error
{
public:
	construct_scene_error( const log::logger_ptr&   logger,
					       const std::string&		message,
					       log::SEVERITY			severity = log::S_ERROR ) :
		slon_error(logger, message, severity)
	{}
};

template<typename ElementType>
class collada_instance_base
{
public:
	typedef ElementType						element_type;
	typedef boost::shared_ptr<element_type>	element_ptr;

public:
	operator ElementType* () const { return element.get(); }

	element_type* operator -> () { return element.get(); }
	const element_type* operator -> () const { return element.get(); }

	element_type& operator * () { return *element; }
	const element_type& operator * () const { return *element; }

public:
	element_ptr element;
};

/** Represents collada <instance_...> node.
 * Gets instance element as input. Searches library for provided instance id and calls loader.
 */
template<typename ElementType>
class collada_instance :
	public collada_instance_base<ElementType>
{
public:
	typedef collada_instance_base<ElementType> base_type;

public:
	collada_instance() :
		logger( log::currentLogManager().createLogger("database.COLLADA") )
	{}

    XMLPP_ELEMENT_SERIALIZATION(collada_instance, ColladaDocument);

public:
	log::logger_ptr	logger;
    std::string		target;
};

template<typename Element>
struct library_elements;

/** Represents collada <library_...> node. */
template<typename Element>
class collada_library :
	public boost::noncopyable
{
public:
	typedef boost::shared_ptr<Element>						element_ptr;
	typedef boost::shared_ptr<const Element>				const_element_ptr;
	typedef boost::unordered_map<std::string, element_ptr>	element_set;

public:
	collada_library() :
		logger( log::currentLogManager().createLogger("database.COLLADA") )
	{}

	void load(const ColladaDocument& document, const xmlpp::element& elem)
	{
		for (xmlpp::element_iterator iter  = elem.first_child_element();
									 iter != elem.end_child_element();
									 ++iter)
		{
			element_ptr element(new Element);
			element->load(document, *iter);
            elements.insert( std::make_pair(element->id, element) );
		}
	}

	void save(ColladaDocument& document, xmlpp::element& elem) const
	{
        /*
		xmlpp::element libraryElem( library_elements<Element>::name() );
		xmlpp::add_child(document, libraryElem);

        for (typename element_set::const_iterator iter  = elements.begin();
                                                  iter != elements.end();
                                                  ++iter)
		{
			xmlpp::element elem;
			iter->second->save(document, elem);
			xmlpp::add_child(libraryElem, elem);
		}
        */
	}

	element_ptr get_element(const std::string& name)
	{
        typename element_set::iterator iter = elements.find(name);
		if (iter != elements.end()) {
			return iter->second;
		}

		return element_ptr();
	}

	XMLPP_SERIALIZATION_SPLIT_MEMBER(ColladaDocument, xmlpp::element);

public:
	log::logger_ptr	logger;
    std::string		target;
	element_set		elements;
};

template<typename value_type>
inline void extract(const xmlpp::node& node, value_type& value)
{
	std::istringstream ss( xmlpp::element(node).get_text() );
	ss >> value;
}

struct read_transform
{
public:
    read_transform(math::Matrix4f& _transform) :
        transform(_transform)
    {}

    virtual void operator () (const ColladaDocument&, const xmlpp::node&) = 0;

    virtual ~read_transform() {}

protected:
    math::Matrix4f& transform;
};

struct read_rotate :
    public read_transform
{
    read_rotate(math::Matrix4f& _transform) :
        read_transform(_transform)
    {}

    void operator () ( const ColladaDocument&   /*document*/,
                       const xmlpp::node&       node)
    {
        math::Vector4f rotation;
        extract(node, rotation);
        transform *= math::make_rotation(math::deg_to_rad(rotation.w), math::xyz(rotation));
    }
};

struct read_translate :
    public read_transform
{
    read_translate(math::Matrix4f& _transform) :
        read_transform(_transform)
    {}

    void operator () ( const ColladaDocument&   /*document*/,
                       const xmlpp::node&       node)
    {
        math::Vector4f translation;
        extract(node, translation);
        transform *= math::make_translation(translation.x, translation.y, translation.z);
    }
};

struct read_scale :
    public read_transform
{
    read_scale(math::Matrix4f& _transform) :
        read_transform(_transform)
    {}

    void operator () ( const ColladaDocument&   /*document*/,
                       const xmlpp::node&       node)
    {
        math::Vector4f scale;
        extract(node, scale);
        transform *= math::make_scaling(scale.x, scale.y, scale.z);
    }
};

struct read_matrix :
    public read_transform
{
    read_matrix(math::Matrix4f& _transform) :
        read_transform(_transform)
    {}

    void operator () ( const ColladaDocument&   /*document*/,
                       const xmlpp::node&       node)
    {
        math::Matrix4f matrix;
        extract(node, matrix);
        transform *= matrix;
    }
};

struct read_bool
{
    read_bool(bool& _boolValue) :
        boolValue(_boolValue)
    {}

    void operator () ( const ColladaDocument&   /*document*/,
                       const xmlpp::node&       node)
    {
        std::string boolStr;
        extract(node, boolStr);

        if (boolStr == "false") {
            boolValue = false;
        }
        else if (boolStr == "true") {
            boolValue = true;
        }
        else {
            // TODO: do something
        }
    }

    bool& boolValue;
};

struct warning
{
    warning( log::Logger&       _logger,
             const std::string& _message,
             log::SEVERITY      _severity = log::S_WARNING ) :
        logger(_logger),
        message(_message),
        severity(_severity)
    {}

    void operator () ( const ColladaDocument&   /*document*/,
                       const xmlpp::node&       /*node*/)
    {
        logger << severity << message << std::endl;
    }

    log::Logger&        logger;
    std::string         message;
    log::SEVERITY  severity;
};

} // namespace database
} // namespace slon

namespace xmlpp {

template<typename T>
struct text_serialization_policy< slon::database::collada_optional<T> >
{
    typedef element xmlpp_holder_type;

    template<typename Document>
    void load(const Document& d, const xmlpp::element& e, slon::database::collada_optional<T>& obj) 
    { 
        std::istringstream ss( e.get_text() );
        ss >> obj.value;
        
        if ( ss.fail() ) {
            throw dom_error("Can't read element value.");
        }

        obj.specified = true;
    }

    template<typename Document>
    void save(Document& d, xmlpp::element& e, const slon::database::collada_optional<T>& obj) const
    {
        std::ostringstream ss;
        if (obj) {
            ss << obj.value;
        }

        if ( ss.fail() ) {
            throw dom_error("Can't read element value.");
        }
        e.set_text( ss.str().c_str() );
    }

    bool valid(const slon::database::collada_optional<T>& val, xmlpp::s_state s) const { return val.specified || (s == xmlpp::LOAD); }
};

template<typename T, typename A>
struct is_container< sgl::vector<T, A> > : public boost::true_type {};

} // namespace xmlpp

#endif // __SLON_ENGINE_DATABASE_COLLADA_COLLADA_COMMON_H__
