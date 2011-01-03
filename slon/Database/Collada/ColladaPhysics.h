#ifndef __SLON_ENGINE_DATABASE_COLLADA_PHYSICS_H__
#define __SLON_ENGINE_DATABASE_COLLADA_PHYSICS_H__

#include "ColladaCore.h"

#ifdef SLON_ENGINE_USE_PHYSICS
namespace slon {
namespace physics {
	class DynamicsWorld;
	class PhysicsModel;
}
}
#endif

namespace slon {
namespace database {

// forward decl
class collada_physics_scene;
class collada_physics_model;
class collada_physics_material;
class collada_rigid_body;
class collada_rigid_constraint;
class collada_shape;
class collada_shape_geometry;

// instance & library typedefs
typedef collada_library<collada_physics_scene>					collada_library_physics_scenes;
typedef collada_library<collada_physics_model>					collada_library_physics_models;
typedef collada_library<collada_physics_material>				collada_library_physics_materials;

typedef collada_instance<collada_physics_scene>					collada_instance_physics_scene;
typedef collada_instance<collada_physics_model>					collada_instance_physics_model;
typedef collada_instance<collada_physics_material>				collada_instance_physics_material;
typedef collada_instance<collada_rigid_body>					collada_instance_rigid_body;
typedef collada_instance<collada_rigid_constraint>				collada_instance_rigid_constraint;

// ptr typedefs
typedef boost::shared_ptr<collada_physics_scene>				collada_physics_scene_ptr;
typedef boost::shared_ptr<collada_physics_model>				collada_physics_model_ptr;
typedef boost::shared_ptr<collada_physics_material>				collada_physics_material_ptr;
typedef boost::shared_ptr<collada_rigid_body>					collada_rigid_body_ptr;
typedef boost::shared_ptr<collada_rigid_constraint>				collada_rigid_constraint_ptr;
typedef boost::shared_ptr<collada_shape>						collada_shape_ptr;
typedef boost::shared_ptr<collada_shape_geometry>				collada_shape_geometry_ptr;

// storage typedefs
typedef prefix_tree<char, collada_physics_scene_ptr>			ColladaPhysicsSceneStorage;
typedef prefix_tree<char, collada_physics_model_ptr>			ColladaPhysicsModelStorage;
typedef prefix_tree<char, collada_physics_material_ptr>			ColladaPhysicsMaterialStorage;

template<> struct library_elements<collada_physics_scene>		{ static std::string name() { return "library_physics_scenes"; } };
template<> struct library_elements<collada_physics_model>		{ static std::string name() { return "library_physics_models"; } }; 
template<> struct library_elements<collada_physics_material>	{ static std::string name() { return "collada_physics_materials"; } };

/** Represents shape element */
class collada_shape :
    public sgl::Aligned16
{
public:
    XMLPP_ELEMENT_SERIALIZATION(collada_shape, ColladaDocument);

public:
    math::Matrix4f              transform;
    collada_shape_geometry_ptr  geometry;
};

class collada_shape_geometry :
    public sgl::Aligned16
{
public:
    enum SHAPE_TYPE
    {
        PLANE,
        SPHERE,
        BOX,
		TAPPERED_CYLINDER,
        CAPSULE,
        GEOMETRY
    };

public:
    /** Get type of the shape */
    virtual SHAPE_TYPE getShapeType() const = 0;
	
    XMLPP_ELEMENT_SERIALIZATION(collada_shape_geometry, ColladaDocument);

public:
    math::Matrix4f transform;
};

/** Represents collada geometry shape */
class collada_geometry_shape :
    public collada_shape_geometry
{
public:
    // override collada_shape
    SHAPE_TYPE getShapeType() const { return GEOMETRY; }

    XMLPP_ELEMENT_SERIALIZATION(collada_geometry_shape, ColladaDocument);

public:
    collada_instance_geometry geometryInstance;
};

/** Represents collada <sphere> element */
class collada_sphere_shape :
    public collada_shape_geometry
{
public:
    // override collada_shape
    SHAPE_TYPE getShapeType() const { return SPHERE; }

    XMLPP_ELEMENT_SERIALIZATION(collada_sphere_shape, ColladaDocument);

public:
    float radius;
};

/** Represents collada <box> element */
class collada_box_shape :
    public collada_shape_geometry
{
public:
    // override collada_shape
    SHAPE_TYPE getShapeType() const { return BOX; }

    XMLPP_ELEMENT_SERIALIZATION(collada_box_shape, ColladaDocument);

public:
    math::Vector3f halfExtents;
};

/** Represents collada <box> element */
class collada_tapered_cylinder_shape :
    public collada_shape_geometry
{
public:
    // override collada_shape
    SHAPE_TYPE getShapeType() const { return TAPPERED_CYLINDER; }

    XMLPP_ELEMENT_SERIALIZATION(collada_tapered_cylinder_shape, ColladaDocument);

public:
	math::Vector2f	lowerRadius;
	math::Vector2f	upperRadius;
	float			height;
};

/** Represents <physics_material> element */
class collada_physics_material
{
public:
    XMLPP_ELEMENT_SERIALIZATION(collada_physics_material, ColladaDocument);

public:
	std::string	id;
    float		dynamicFriction;
    float		staticFriction;
    float		restitution;
};

/** Represents <limits> element */
class collada_limits
{
public:
    XMLPP_ELEMENT_SERIALIZATION(collada_limits, ColladaDocument);

public:
    math::Vector3f  linear[2];
    math::Vector3f  swingConeTwist[2];
};

/** Represents <attachment> or <ref_attachment> element */
class collada_attachment :
    public sgl::Aligned16
{
friend class collada_instance<collada_physics_model>;
public:
    XMLPP_ELEMENT_SERIALIZATION(collada_attachment, ColladaDocument);

public:
    math::Matrix4f          transform;
    collada_rigid_body_ptr  rigidBody;

private:
    std::string rigidBodyStr;
};

/** Represents <rigid_constraint> element */
class collada_rigid_constraint :
    public sgl::Aligned16
{
public:
    /** load <technique_common> */
    void load_technique_common(const ColladaDocument& document, const xmlpp::node& node);
	
    XMLPP_ELEMENT_SERIALIZATION(collada_rigid_constraint, ColladaDocument);

public:
    std::string         sid;
    bool                enabled;
    collada_attachment  attachment;
    collada_attachment  refAttachment;
    collada_limits      limits;
};

/** Represents <instance_rigid_constraint> element */
template<>
class collada_instance<collada_rigid_constraint> :
    public collada_instance_base<collada_rigid_constraint>
{
friend class collada_instance<collada_physics_model>;
public:
    XMLPP_ELEMENT_SERIALIZATION(collada_instance_rigid_constraint, ColladaDocument);

private:
    std::string constraintStr;
    std::string body;
};

/** Represents <rigid_body> element */
class collada_rigid_body
{
public:
    typedef std::vector<collada_shape_ptr>  shape_vector;
    typedef shape_vector::iterator          shape_iterator;

private:
    void load_technique_common(const ColladaDocument& document, const xmlpp::node& node);

public:
    XMLPP_ELEMENT_SERIALIZATION(collada_rigid_body, ColladaDocument);

public:
    std::string                         sid;
    float                               mass;
    bool                                dynamic;
    shape_vector                        shapes;
    collada_instance_physics_material   materialInstance;
};

/** Represents <instance_rigid_body> element */
template<>
class collada_instance<collada_rigid_body> :
    public collada_instance_base<collada_rigid_body>
{
friend class collada_instance<collada_physics_model>;
public:
    XMLPP_ELEMENT_SERIALIZATION(collada_instance_rigid_body, ColladaDocument);

public:
    std::string target;

private:
    std::string body;
};

/** Represents <physics_model> element */
class collada_physics_model
{
public:
    typedef std::vector<collada_rigid_body_ptr>         rigid_body_vector;
    typedef rigid_body_vector::iterator                 rigid_body_iterator;

    typedef std::vector<collada_rigid_constraint_ptr>   rigid_constraint_vector;
    typedef rigid_constraint_vector::iterator           rigid_constraint_iterator;

public:
    XMLPP_ELEMENT_SERIALIZATION(collada_physics_model, ColladaDocument);

public:
	std::string				 id;
    rigid_body_vector        rigidBodies;
    rigid_constraint_vector  rigidConstraints;
};

/** Represents <instance_physics_model> element */
template<>
class collada_instance<collada_physics_model> :
    public collada_instance_base<collada_physics_model>
{
public:
    typedef std::vector<collada_instance_rigid_body>        instance_rigid_body_vector;
    typedef instance_rigid_body_vector::iterator            instance_rigid_body_iterator;

    typedef std::vector<collada_instance_rigid_constraint>  instance_rigid_constraint_vector;
    typedef instance_rigid_constraint_vector::iterator      instance_rigid_constraint_iterator;

public:
    void load(const ColladaDocument& document, const xmlpp::element& elem);
    void save(ColladaDocument& document, xmlpp::element& elem) const {}

public:
    instance_rigid_body_vector          rigidBodyInstances;
    instance_rigid_constraint_vector    rigidConstraintInstances;

private:
    std::string url;
};

/** Represents <physics_scene> element */
class collada_physics_scene
{
public:
    typedef std::vector<collada_instance_physics_model>     instance_physics_model_vector;
    typedef instance_physics_model_vector::iterator         instance_physics_model_iterator;

public:
    XMLPP_ELEMENT_SERIALIZATION(collada_physics_scene, ColladaDocument);

public:
	std::string						id;
    instance_physics_model_vector	physicsModelInstances;
};

} // namespace database
} // namespace slon

#endif // __SLON_ENGINE_DATABASE_COLLADA_PHYSICS_H__
