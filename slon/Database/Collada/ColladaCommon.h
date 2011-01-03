#ifndef __SLON_ENGINE_DATABASE_COLLADA_COLLADA_COMMON_H__
#define __SLON_ENGINE_DATABASE_COLLADA_COLLADA_COMMON_H__

#include "../../Log/Logger.h"
#include "../../Utility/error.hpp"
#include "../Forward.h"
#include <boost/shared_ptr.hpp>
#include <functional>
#include <sgl/Math/MatrixFunctions.hpp>
#include <sgl/Math/Quaternion.hpp>
#include <sgl/Math/Utility.hpp>
#include <sstream>
#include <xml++/serialization/serialization.hpp>

namespace slon {
namespace database {

// Forward decl
class ColladaDocument;

// misc typedefs
typedef std::binary_function<const ColladaDocument&, const xmlpp::node&, void> 	collada_loader_function;
typedef xmlpp::generic_loader<ColladaDocument> 									collada_loader;

typedef xmlpp::generic_serializer<ColladaDocument>                              collada_serializer;

/** Error class occuring during the parsing collada document */
class collada_error :
    public slon_error
{
public:
    collada_error( log::Logger&          logger,
                   const std::string&    message,
                   log::WARNING_LEVEL    warningLevel = log::WL_ERROR ) :
        slon_error(logger, message, warningLevel)
    {}

    collada_error( log::Logger&          logger,
                   const std::string&    message,
                   const xmlpp::node&    _node,
                   log::WARNING_LEVEL    warningLevel = log::WL_ERROR ) :
        slon_error(logger, message, warningLevel),
        node(_node)
    {}

    /** Get the broken node */
    xmlpp::node& getNode() { return node; }

private:
    xmlpp::node node;
};

/** Error class occuring during visual or physics scene construction */
class construct_scene_error :
	public slon_error
{
public:
	construct_scene_error( log::Logger&    	  logger,
					       const std::string& message,
					       log::WARNING_LEVEL warningLevel = log::WL_ERROR ) :
		slon_error(logger, message, warningLevel)
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
		logger("database.COLLADA")
	{}

    XMLPP_ELEMENT_SERIALIZATION(collada_instance, ColladaDocument);

public:
	log::Logger	logger;
    std::string target;
};

template<typename Element>
struct library_elements;

/** Represents collada <library_...> node. */
template<typename Element>
class collada_library :
	public boost::noncopyable
{
public:
	typedef boost::shared_ptr<Element>			element_ptr;
	typedef boost::shared_ptr<const Element>	const_element_ptr;
	typedef prefix_tree<char, element_ptr>		element_set;

public:
	collada_library() :
		logger("database.COLLADA")
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
		xmlpp::element libraryElem( library_elements<Element>::name() );
		xmlpp::add_child(document, libraryElem);

		for (element_set::const_iterator iter  = elements.begin();
                                         iter != elements.end();
                                         ++iter)
		{
			xmlpp::element elem;
			iter->second->save(document, elem);
			xmlpp::add_child(libraryElem, elem);
		}
	}

	element_ptr get_element(const std::string& name)
	{
		element_set::iterator iter = elements.find(name);
		if (iter != elements.end()) {
			return iter->second;
		}

		return element_ptr();
	}

	XMLPP_SERIALIZATION_SPLIT_MEMBER(ColladaDocument, xmlpp::element);

public:
	log::Logger	logger;
    std::string target;
	element_set elements;
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
    warning( log::Logger&        _logger,
             const std::string& _message,
             log::WARNING_LEVEL _warningLevel = log::WL_WARNING ) :
        logger(_logger),
        message(_message),
        warningLevel(_warningLevel)
    {}

    void operator () ( const ColladaDocument&   /*document*/,
                       const xmlpp::node&       /*node*/)
    {
        logger << warningLevel << message << std::endl;
    }

    log::Logger&        logger;
    std::string         message;
    log::WARNING_LEVEL  warningLevel;
};

} // namespace database
} // namespace slon

#endif // __SLON_ENGINE_DATABASE_COLLADA_COLLADA_COMMON_H__
