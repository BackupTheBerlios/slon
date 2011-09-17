#ifndef __SLON_ENGINE_PHYSICS_COLLISION_SHAPE_H__
#define __SLON_ENGINE_PHYSICS_COLLISION_SHAPE_H__

#include <boost/intrusive_ptr.hpp>
#include <sgl/Math/Containers.hpp>
#include <sgl/Math/MatrixFunctions.hpp>
#include <sgl/Math/Plane.hpp>
#include <vector>
#include "../Database/Serializable.h"
#include "../Utility/Algorithm/algorithm.hpp"
#include "../Utility/referenced.hpp"
#include "Forward.h"

namespace slon {
namespace physics {

/** Collision shape class */
class SLON_PUBLIC CollisionShape :
	public Referenced,
    public database::Serializable
{
public:
	enum SHAPE_TYPE
	{
		PLANE,
		SPHERE,
		BOX,
		CONE,
		CAPSULE,
        CYLINDER_X,
        CYLINDER_Y,
        CYLINDER_Z,
		HEIGHTFIELD,
		CONVEX_MESH,
		TRIANGLE_MESH,
		COMPOUND
	};

    /** Clone the shape */
    virtual CollisionShape* clone() const = 0;

	/** Get type of the collision shape. */
	virtual SHAPE_TYPE getShapeType() const = 0;

	virtual ~CollisionShape() {}
};

typedef boost::intrusive_ptr<CollisionShape>        collision_shape_ptr;
typedef boost::intrusive_ptr<const CollisionShape>  const_collision_shape_ptr;

/** Planar collision shape */
class SLON_PUBLIC PlaneShape :
	public CollisionShape
{
public:
    explicit PlaneShape(const math::Planer& plane_ = math::Planer()) :
        plane(plane_)
    {}
    PlaneShape(real A, real B, real C, real D) :
        plane(A, B, C, D)
    {}
    PlaneShape(const math::Vector3r& normal, real D) :
        plane(normal, D)
    {}
    PlaneShape(const math::Vector3r& pA, const math::Vector3r& pB, const math::Vector3r& pC) :
        plane(pA, pB, pC)
    {}

    // Override Serializable
    const char* serialize(database::OArchive& ar) const;
    void        deserialize(database::IArchive& ar);

    // Override CollisionShape
    CollisionShape* clone() const        { return new PlaneShape(plane); }
	SHAPE_TYPE      getShapeType() const { return PLANE; }

public:
	math::Planer plane;
};

/** Sphere collision shape */
class SLON_PUBLIC SphereShape :
	public CollisionShape
{
public:
    SphereShape(real _radius = 1.0f) :
        radius(_radius)
    {}

    // Override Serializable
    const char* serialize(database::OArchive& ar) const;
    void        deserialize(database::IArchive& ar);

    // Override CollisionShape
    CollisionShape* clone() const        { return new SphereShape(*this); }
	SHAPE_TYPE      getShapeType() const { return SPHERE; }

public:
	real radius;
};

/** Box collision shape */
class SLON_PUBLIC BoxShape :
	public CollisionShape
{
public:
	BoxShape() :
	    halfExtent(0)
	{}

	BoxShape(float halfx, float halfy, float halfz) :
	    halfExtent(halfx, halfy, halfz)
	{}

	BoxShape(const math::Vector3r& _halfExtent) :
	    halfExtent(_halfExtent)
	{}

    // Override Serializable
    const char* serialize(database::OArchive& ar) const;
    void        deserialize(database::IArchive& ar);

    // Override CollisionShape
    CollisionShape* clone() const        { return new BoxShape(*this); }
    SHAPE_TYPE      getShapeType() const { return BOX; }

public:
	math::Vector3r halfExtent;
};

/** Cone collision shape. Up axis is Y.  */
class SLON_PUBLIC ConeShape :
	public CollisionShape
{
public:
	ConeShape(real _radius = 0, real _height = 0) :
	    radius(_radius),
	    height(_height)
	{}

    // Override Serializable
    const char* serialize(database::OArchive& ar) const;
    void        deserialize(database::IArchive& ar);

    // Override CollisionShape
    CollisionShape* clone() const        { return new ConeShape(*this); }
	SHAPE_TYPE      getShapeType() const { return CONE; }

public:
	real radius;
	real height;
};

/** Capsule collision shape. Up axis is Y. */
class SLON_PUBLIC CapsuleShape :
	public CollisionShape
{
public:
	CapsuleShape(real _radius = 0, real _height = 0) :
	    radius(_radius),
	    height(_height)
	{}

    // Override Serializable
    const char* serialize(database::OArchive& ar) const;
    void        deserialize(database::IArchive& ar);

    // Override CollisionShape
    CollisionShape* clone() const        { return new CapsuleShape(*this); }
	SHAPE_TYPE      getShapeType() const { return CAPSULE; }

public:
	real radius;	/// thickness of the cylinder
	real height;	/// height of the cylinder
};

/** Cylinder collision shape. Up axis is X. */
class SLON_PUBLIC CylinderXShape :
	public CollisionShape
{
public:
    CylinderXShape() :
	    halfExtent(0.0f)
	{}

    CylinderXShape(const math::Vector3f& halfExtent_) :
	    halfExtent(halfExtent_)
	{}

    CylinderXShape(real x, real y, real z) :
	    halfExtent(x, y, z)
	{}

    // Override Serializable
    const char* serialize(database::OArchive& ar) const;
    void        deserialize(database::IArchive& ar);

    // Override CollisionShape
    CollisionShape* clone() const        { return new CylinderXShape(*this); }
	SHAPE_TYPE      getShapeType() const { return CYLINDER_X; }

public:
    math::Vector3r halfExtent;
};

/** Cylinder collision shape. Up axis is Y. */
class SLON_PUBLIC CylinderYShape :
	public CollisionShape
{
public:
    CylinderYShape() :
	    halfExtent(0.0f)
	{}

    CylinderYShape(const math::Vector3f& halfExtent_) :
	    halfExtent(halfExtent_)
	{}

    CylinderYShape(real x, real y, real z) :
	    halfExtent(x, y, z)
	{}

    // Override Serializable
    const char* serialize(database::OArchive& ar) const;
    void        deserialize(database::IArchive& ar);

    // Override CollisionShape
    CollisionShape* clone() const        { return new CylinderYShape(*this); }
	SHAPE_TYPE      getShapeType() const { return CYLINDER_Y; }

public:
    math::Vector3r halfExtent;
};

/** Cylinder collision shape. Up axis is Z. */
class SLON_PUBLIC CylinderZShape :
	public CollisionShape
{
public:
    CylinderZShape() :
	    halfExtent(0.0f)
	{}

    CylinderZShape(const math::Vector3f& halfExtent_) :
	    halfExtent(halfExtent_)
	{}

    CylinderZShape(real x, real y, real z) :
	    halfExtent(x, y, z)
	{}

    // Override Serializable
    const char* serialize(database::OArchive& ar) const;
    void        deserialize(database::IArchive& ar);

    // Override CollisionShape
    CollisionShape* clone() const        { return new CylinderZShape(*this); }
	SHAPE_TYPE      getShapeType() const { return CYLINDER_Z; }

public:
    math::Vector3r halfExtent;
};

/** Convex hull collision shape. Collision detection behaviour
 * is undefined if convex shape is not convex). Use buildConvexHull
 * in cases you don't know definetely that the point cloud represents
 * convex hull.
 */
class SLON_PUBLIC ConvexShape :
	public CollisionShape
{
public:
    // Override Serializable
    const char* serialize(database::OArchive& ar) const;
    void        deserialize(database::IArchive& ar);

	// Override CollisionShape
    CollisionShape* clone() const { return new ConvexShape(*this); }
	SHAPE_TYPE      getShapeType() const { return CONVEX_MESH; }

	/** Build convex hull of the point cloud and store it as shape.
	 * @param vertices - vertices of point cloud.
	 */
	void buildConvexHull(const math::vector_of_vector3f& vertices)
	{
	    buildConvexHull( vertices.begin(), vertices.end() );
	}

	/** Build convex hull of the point cloud and store it as shape.
	 * @tparam Iterator - iterator type referencing 3-component floating tuple.
	 * @param beginIter - iterator addressing first vertex in point cloud.
	 * @param beginIter - iterator addressing end vertex in point cloud.
	 * @see build_convex_hull
	 */
	template<typename Iterator>
	void buildConvexHull(const Iterator& beginIter, const Iterator& enditer)
	{
	    vertices.clear();
        // TODO: Build convex hull
        std::copy( beginIter, enditer, std::back_inserter(vertices) );
	    //build_convex_hull( beginIter, endIter, vertices.begin() );
	}

public:
    math::vector_of_vector3f vertices;
};

/** Arbitrary triangle mesh collision shape */
class SLON_PUBLIC TriangleMeshShape :
	public CollisionShape
{
public:
    // Override Serializable
    const char* serialize(database::OArchive& ar) const;
    void        deserialize(database::IArchive& ar);

    // Override CollisionShape
    CollisionShape* clone() const        { return new TriangleMeshShape(*this); }
	SHAPE_TYPE      getShapeType() const { return TRIANGLE_MESH; }


public:
	std::vector<math::Vector3r>	vertices;
	std::vector<unsigned>		indices;
};

/** Compound collision shape*/
class SLON_PUBLIC CompoundShape :
    public CollisionShape
{
public:
    typedef boost::intrusive_ptr<CollisionShape>    collision_shape_ptr;

    struct shape_transform
    {
        math::Matrix4r      transform;
        collision_shape_ptr shape;
    };

    typedef sgl::vector< shape_transform, sgl::aligned_allocator<shape_transform> > shape_transform_vector;
    typedef shape_transform_vector::iterator                                        shape_transform_iterator;
    typedef shape_transform_vector::const_iterator                                  const_shape_trasnform_iterator;

public:
    CompoundShape() {}
    CompoundShape(const math::Matrix4r& transform,
                  CollisionShape*       collisionShape) 
    {
        addShape(transform, collisionShape);
    }

    // Override Serializable
    const char* serialize(database::OArchive& ar) const;
    void        deserialize(database::IArchive& ar);

    // Override CollisionShape
    CollisionShape* clone() const        { return new CompoundShape(*this); }
    SHAPE_TYPE      getShapeType() const { return COMPOUND; }

    /** Add shape with transform
     * @param transform - transform of the shape. Scaling doesn't applied.
     * @param collisionShape - collision shape. Do not allocate on stack.
     */
    void addShape( const math::Matrix4r& transform,
                   CollisionShape*       collisionShape )
    {
        shape_transform shapeTransform;
        shapeTransform.transform = transform;
        shapeTransform.shape.reset(collisionShape);
        shapes.push_back(shapeTransform);
    }

public:
    shape_transform_vector shapes;
};

} // namespace physics
} // namespace slon

#endif // __SLON_ENGINE_PHYSICS_COLLISION_SHAPE_H__
