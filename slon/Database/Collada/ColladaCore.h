#ifndef __SLON_ENGINE_DATABASE_COLLADA_CORE_H__
#define __SLON_ENGINE_DATABASE_COLLADA_CORE_H__

#include "../../Scene/MatrixTransform.h"
#include "../../Utility/Algorithm/prefix_tree.hpp"
#include "../../Utility/error.hpp"
#include "ColladaFX.h"
#include <sgl/Program.h>

namespace slon {

// forward 
namespace animation {
    class Animation;
}

namespace database {

// Forward
class collada_entity;
class collada_geometry;
class collada_source;
class collada_mesh;
class collada_node;
class collada_visual_scene;
class collada_controller;
class collada_control_element;
class collada_animation;
class collada_channel;
class collada_sampler;

// instance & library typedefs
typedef collada_library<collada_geometry>					collada_library_geometries;
typedef collada_library<collada_visual_scene>				collada_library_visual_scenes;
typedef collada_library<collada_controller>					collada_library_controllers;
typedef collada_library<collada_animation>					collada_library_animations;

typedef collada_instance<collada_geometry> 					collada_instance_geometry;
typedef collada_instance<collada_controller>				collada_instance_controller;
typedef collada_instance<collada_visual_scene>				collada_instance_visual_scene;

// ptr typedefs
typedef boost::shared_ptr<collada_entity> 					collada_entity_ptr;
typedef boost::shared_ptr<collada_geometry> 				collada_geometry_ptr;
typedef boost::shared_ptr<collada_instance_geometry>		collada_instance_geometry_ptr;
typedef boost::shared_ptr<collada_source> 					collada_source_ptr;
typedef boost::shared_ptr<collada_mesh> 					collada_mesh_ptr;
typedef boost::shared_ptr<collada_node>						collada_node_ptr;
typedef boost::shared_ptr<collada_visual_scene> 			collada_visual_scene_ptr;
typedef boost::shared_ptr<collada_controller>				collada_controller_ptr;
typedef boost::shared_ptr<collada_instance_controller>		collada_instance_controller_ptr;
typedef boost::shared_ptr<collada_control_element>			collada_control_element_ptr;
typedef boost::shared_ptr<collada_animation>				collada_animation_ptr;
typedef boost::shared_ptr<collada_channel>					collada_channel_ptr;
typedef boost::shared_ptr<collada_sampler>					collada_sampler_ptr;

// storage typedefs
typedef prefix_tree<char, collada_visual_scene_ptr>			ColladaVisualSceneStorage;
typedef prefix_tree<char, collada_geometry_ptr>				ColladaGeometryStorage;
typedef prefix_tree<char, collada_controller_ptr>			ColladaControllerStorage;

template<> struct library_elements<collada_geometry>		{ static std::string name() { return "library_geometries"; } }; 
template<> struct library_elements<collada_visual_scene>	{ static std::string name() { return "library_visual_scenes"; } };
template<> struct library_elements<collada_controller>		{ static std::string name() { return "library_controllers"; } }; 
template<> struct library_elements<collada_animation>		{ static std::string name() { return "library_animations"; } }; 

/** Represents collada element <source> for mesh.
 * TODO: Add support of different arrays(at now only float is acceptable).
 * TODO: Add support of different techniques.
 */
class collada_source
{
private:
    void loadTechniqueCommon(const ColladaDocument& document, const xmlpp::node& node);

public:
    XMLPP_ELEMENT_SERIALIZATION(collada_source, ColladaDocument);

    const std::string& getName(unsigned i) const
    {
        assert( i < nameArray.size() );
        return nameArray[i];
    }

    template<typename T, int n>
    math::Matrix<T, n, 1> getVector(unsigned i) const
    {
        math::Matrix<T, n, 1> vec;
        for (size_t j = 0; j<n; ++j) {
            vec[j] = floatArray[n*i + j];
        }

        return vec;
    }

    template<typename T, int n>
    void setVector(unsigned int i, const math::Matrix<T, n, 1>& vec)
    {
        for (size_t j = 0; j<n; ++j) {
            floatArray[n*i + j] = vec[j];
        }
    }

    template<typename T, int n, int m>
    math::Matrix<T, n, m> getMatrix(unsigned i) const
    {
        math::Matrix<T, n, m> matrix;
        for (size_t j = 0; j<n*m; ++j) {
            matrix(j) = floatArray[n*m*i + j];
        }

        return matrix;
    }

	void setData( const float*	data, 
				  size_t		size, 
				  size_t		stride,
				  const char**	nameArray = 0)
	{
		floatArray.resize(size);
		std::copy( data, data + size, floatArray.begin() );
	}

public:
	size_t				        stride;
    std::string                 id;
    std::vector<std::string>    nameArray;
    std::vector<float>          floatArray;
};

/** Represents collada <input> element */
class collada_input
{
friend class collada_mesh;
friend class collada_skin;
public:
    XMLPP_ELEMENT_SERIALIZATION(collada_input, ColladaDocument);

    const std::string& getName(unsigned i) const
    {
        assert( source && i < source->nameArray.size() );
        return source->nameArray[i];
    }

    template<typename T, int n>
    math::Matrix<T, n, 1> getVector(unsigned i) const
    {
        assert(source); 

        math::Matrix<T, n, 1> vec;
        for (size_t j = 0; j<n; ++j) {
            vec[j] = source->floatArray[n*i + j];
        }

        return vec;
    }

    template<typename T, int n>
    void setVector(unsigned int i, const math::Matrix<T, n, 1>& vec)
    {
        assert(source); 
        for (size_t j = 0; j<n; ++j) {
            source->floatArray[n*i + j] = vec[j];
        }
    }

    template<typename T, int n, int m>
    math::Matrix<T, n, m> getMatrix(unsigned i) const
    {
        assert(source); 

        math::Matrix<T, n, m> matrix;
        for (size_t j = 0; j<n*m; ++j) {
            matrix(j) = source->floatArray[n*m*i + j];
        }

        return matrix;
    }

	void setSemantic();

public:
	collada_source_ptr	source;
	std::string	        sourceId;
    std::string     	attributeName;
    std::string         semantic;
    int					attributeIndex;
    int					set;
	int					offset;
};

/** Represents collada primitive_elements, such as <lines>, <triangles> etc.
 * TODO: Add support of different elemetns(for now only triangles are supported)
 */
class collada_primitives
{
friend class collada_instance<collada_geometry>;
public:
	typedef std::vector<unsigned int>       int_vector;

	typedef std::vector<collada_input>		input_vector;
	typedef input_vector::iterator			input_iterator;
	typedef input_vector::const_iterator	const_input_iterator;

public:
    void load(const ColladaDocument& d, const xmlpp::element& n);
    void save(ColladaDocument& d, xmlpp::element& n) const;

public:
    sgl::PRIMITIVE_TYPE     primType;
    std::vector<int_vector>	inputIndices;  /// indices for inputs
    input_vector 			inputs;
	std::string 			material;
};

/** Represents <vertices> element */
class collada_vertices
{
public:
    XMLPP_ELEMENT_SERIALIZATION(collada_vertices, ColladaDocument);

public:
    std::vector<collada_input>  inputs;
};

/** Base class for <geometry>, <light>, <camera> elements */
class collada_entity
{
public:
	/** Type of the collada entity */
	enum ENTITY_TYPE
	{
        ANIMATION,
        CAMERA,
        CONTROLLER,
		GEOMETRY,
		LIGHT
	};

public:
	/** Get type of the object */
	virtual ENTITY_TYPE getEntityType() const = 0;

	virtual ~collada_entity() {}
};

/** Represents <geometry> element */
class collada_geometry :
	public collada_entity
{
public:
	enum GEOMETRY_TYPE
	{
		MESH,
		CONVEX_MESH
	};

	typedef std::vector<collada_primitives> 	primitives_vector;
	typedef primitives_vector::iterator			primitives_iterator;

public:
    virtual XMLPP_ELEMENT_SERIALIZATION(collada_geometry, ColladaDocument);

	/** Get type of the geometry */
	virtual GEOMETRY_TYPE getGeometryType() const = 0;

	// Override collada_entity
	virtual ENTITY_TYPE getEntityType() const { return GEOMETRY; }

	/** Get iterator addressing first primitives element */
	virtual primitives_iterator firstPrimitivesElement() = 0;

	/** Get iterator addressing end primitives element */
	virtual primitives_iterator endPrimitivesElement() = 0;

	virtual ~collada_geometry() {}

public:
    std::string id;
    std::string name;
};

/** Represents <instance_geometry> element */
template<>
class collada_instance<collada_geometry>
{
public:
	// override geometry
    void load(const ColladaDocument& document, const xmlpp::element& elem);
    void save(ColladaDocument& document, xmlpp::element& elem) const {}

public:
	collada_geometry_ptr 		geometry;
	collada_bind_material_ptr 	material;
};

/** Represents collada <mesh> element */
class collada_mesh :
	public collada_geometry
{
public:
	// Override geometry
	GEOMETRY_TYPE getGeometryType() const { return MESH; }
	primitives_iterator firstPrimitivesElement() { return primitives.begin(); }
	primitives_iterator endPrimitivesElement() { return primitives.end(); }
    collada_source_ptr getSourceById(const std::string& sourceId);

    // Override entity
	XMLPP_ELEMENT_SERIALIZATION(collada_mesh, ColladaDocument);

public:
    std::vector<collada_source_ptr>    sources;
    std::vector<collada_primitives>    primitives;
    collada_vertices				   vertices;
};

/** Represents collada <convex_mesh> element */
class collada_convex_mesh :
    public collada_geometry
{
public:
    // Override geometry
    GEOMETRY_TYPE getGeometryType() const { return CONVEX_MESH; }
    primitives_iterator firstPrimitivesElement() { return mesh->firstPrimitivesElement(); }
    primitives_iterator endPrimitivesElement() { return mesh->endPrimitivesElement(); }

    // Override entity
    void load(const ColladaDocument& document, const xmlpp::element& elem);
    void save(ColladaDocument& document, xmlpp::element& elem) const;

public:
    bool                isConvexHullOfMesh;
    collada_mesh_ptr    mesh;
};

template<>
void collada_library<collada_geometry>::load(const ColladaDocument& document, const xmlpp::element& elem)
{
	element_ptr		   geometry;
	collada_serializer serializer;
    serializer &= xmlpp::make_nvp( "mesh",          xmlpp::as_element<collada_mesh>(geometry) );
	serializer &= xmlpp::make_nvp( "convex_mesh",   xmlpp::as_element<collada_convex_mesh>(geometry) );
    //serializer &= xmlpp::make_nvp( "spline",        warning(logger, "Usupported <geometry> type: spline") );

	for (xmlpp::element_iterator iter  = elem.first_child_element();
								 iter != elem.end_child_element();
								 ++iter)
	{
		serializer.load(document, *iter);
		elements.insert( element_set::value_type(geometry->id, geometry) );
	}
}

/** Represents <node> element */
class collada_node :
    public sgl::Aligned16
{
public:
	enum TYPE
	{
		NODE,
		JOINT
	};

public:
	typedef std::vector<collada_instance_geometry_ptr>		geometry_instance_vector;
	typedef geometry_instance_vector::iterator				geometry_instance_iterator;

	typedef std::vector<collada_instance_controller_ptr>	controller_instance_vector;
	typedef controller_instance_vector::iterator			controller_instance_iterator;

	typedef std::vector<collada_node_ptr>	node_vector;
	typedef node_vector::iterator			node_iterator;

public:
	collada_node() :
		transform( math::make_identity<float, 4>() ),
        type(NODE)
	{}

    XMLPP_ELEMENT_SERIALIZATION(collada_node, ColladaDocument);

public:
	std::string					id;
    std::string					sid;
    std::string					name;
    TYPE						type;
    math::Matrix4f				transform;
	node_vector					children;
	geometry_instance_vector	geometries;
	controller_instance_vector	controllers;
};

/** Represents <collada_visual_scene> element */
class collada_visual_scene
{
public:
	typedef std::vector<collada_node_ptr>	node_vector;
	typedef node_vector::iterator		    node_iterator;

public:
    XMLPP_ELEMENT_SERIALIZATION(collada_visual_scene, ColladaDocument);

public:
	std::string	id;
	std::string name;
    node_vector	nodes;
};

/** Represents <joints> element */
class collada_joints
{
public:
	typedef std::vector<collada_input>		input_vector;
	typedef input_vector::iterator			input_iterator;
	typedef input_vector::const_iterator	const_input_iterator;

public:
    XMLPP_ELEMENT_SERIALIZATION(collada_joints, ColladaDocument);

public:
    input_vector inputs;
};

/** Represents <vertex_weights> element */
class collada_vertex_weights
{
public:
    XMLPP_ELEMENT_SERIALIZATION(collada_vertex_weights, ColladaDocument);

public:
    std::vector<collada_input>  inputs;
    std::vector<unsigned>       vcount;
    std::vector<unsigned>       v;
};

/** Represents <controller> element */
class collada_controller :
    public collada_entity
{
public:
	/** Get type of the object */
    ENTITY_TYPE getEntityType() const { return CONTROLLER; }

    XMLPP_ELEMENT_SERIALIZATION(collada_controller, ColladaDocument);

public:
    std::string                 id;
    std::string                 name;
    collada_control_element_ptr controlElement;
};

/** Represents <instance_geometry> element */
template<>
class collada_instance<collada_controller> :
    public collada_entity
{
public:
	/** Get type of the object */
    ENTITY_TYPE getEntityType() const { return CONTROLLER; }

    void load(const ColladaDocument& document, const xmlpp::element& elem);
    void save(ColladaDocument& document, xmlpp::element& elem) const {}

public:
    std::string                 url;
	collada_controller_ptr      controller;
    std::string                 skeleton;
	collada_bind_material_ptr   material;
};

/** Base class for control elements */
class collada_control_element :
    public sgl::Aligned16
{
public:
    enum CONTROLLER_TYPE
    {
        SKIN,
        MORPH
    };

public:
    virtual CONTROLLER_TYPE getControllerType() const = 0;

    virtual void load(const ColladaDocument& document, const xmlpp::element& elem) = 0;
    virtual void save(ColladaDocument& document, xmlpp::element& elem) const = 0;
    virtual ~collada_control_element() {}
};

/** Represents <skin> element */
class collada_skin :
    public collada_control_element
{
public:
    XMLPP_ELEMENT_SERIALIZATION(collada_skin, ColladaDocument);

    CONTROLLER_TYPE getControllerType() const { return SKIN; }
    collada_source_ptr getSourceById(const std::string& sourceId);

public:
    math::Matrix4f                  bindShapeMatrix;
    std::vector<collada_source_ptr> sources;
    collada_joints                  joints;
    collada_vertex_weights          weights;
    collada_geometry_ptr            source;
    mutable size_t                  numJoints;

private:
    std::string                     sourceURI;
};

/** Represents <sampler> element */
class collada_sampler
{
public:
	typedef std::vector<collada_input>  input_vector;
    typedef input_vector::iterator      input_iterator;

public:
    XMLPP_ELEMENT_SERIALIZATION(collada_sampler, ColladaDocument);

public:
    std::string  id;
    input_vector inputs;
};

/** Represents <channel> element */
class collada_channel
{
public:
    XMLPP_ELEMENT_SERIALIZATION(collada_channel, ColladaDocument);

    std::string get_target_node() const;
    std::string get_target_transform() const;

public:
    std::string source;
    std::string target;
};

/** Represents <animation> element */
class collada_animation
{
public:
	typedef std::vector<collada_animation>  animation_vector;
	typedef std::vector<collada_source>     source_vector;
	typedef std::vector<collada_sampler>    sampler_vector;
	typedef std::vector<collada_channel>    channel_vector;

    typedef animation_vector::iterator      animation_iterator;
    typedef source_vector::iterator         source_iterator;
    typedef sampler_vector::iterator        sampler_iterator;
    typedef channel_vector::iterator        channel_iterator;

    source_iterator get_source(const std::string& id);
    sampler_iterator get_sampler(const std::string& id);

public:
    XMLPP_ELEMENT_SERIALIZATION(collada_animation, ColladaDocument);
    
public:
    std::string         id;
    std::string         name;
    animation_vector    animations;
    source_vector       sources;
    sampler_vector      samplers;
    channel_vector      channels;
};

} // namespace database
} // namespace slon

#endif // __SLON_ENGINE_DATABASE_COLLADA_PHYSICS_H_
