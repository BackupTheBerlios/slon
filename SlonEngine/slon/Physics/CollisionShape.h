#ifndef __SLON_ENGINE_PHYSICS_COLLISION_SHAPE_H__
#define __SLON_ENGINE_PHYSICS_COLLISION_SHAPE_H__

#include <boost/intrusive_ptr.hpp>
#include <sgl/Math/MatrixFunctions.hpp>
#include <sgl/Math/Plane.hpp>
#include <vector>
#include "../Utility/Algorithm/algorithm.hpp"
#include "../Utility/referenced.hpp"
#include "Forward.h"

namespace slon {
namespace physics {

/** Collision shape class */
class CollisionShape :
	public Referenced
{
public:
	enum SHAPE_TYPE
	{
		PLANE,
		SPHERE,
		BOX,
		CONE,
		CAPSULE,
        CYLINDER,
		HEIGHTFIELD,
		CONVEX_MESH,
		TRIANGLE_MESH,
		COMPOUND
	};

    /** Clone the shape */
    virtual CollisionShape* clone() const = 0;

	/** Get type of the collision shape. */
	virtual SHAPE_TYPE getShapeType() const = 0;

    /** Scale shape. Some shapes unable to handle arbitrary scaling.
     * For example, sphere.
     */
    virtual void applyScaling(const math::Vector3r& scaling) = 0;

	virtual ~CollisionShape() {}
};

typedef boost::intrusive_ptr<CollisionShape>        collision_shape_ptr;
typedef boost::intrusive_ptr<const CollisionShape>  const_collision_shape_ptr;

/** Planar collision shape */
class PlaneShape :
	public CollisionShape
{
public:
    PlaneShape(const math::Planer& plane_) :
        plane(plane_)
    {}

    // Override shape
    CollisionShape* clone() const        { return new PlaneShape(plane); }
	SHAPE_TYPE      getShapeType() const { return PLANE; }
    void            applyScaling(const math::Vector3r& /*scaling*/) { /*nothing*/ }

public:
	math::Planer plane;
};

/** Sphere collision shape */
class SphereShape :
	public CollisionShape
{
public:
    SphereShape(real _radius = 1.0f) :
        radius(_radius)
    {}

    CollisionShape* clone() const { return new SphereShape(*this); }
	SHAPE_TYPE      getShapeType() const { return SPHERE; }
    void            applyScaling(const math::Vector3r& scaling) { radius *= fabs(scaling.x + scaling.y + scaling.z) / 3; }

public:
	real radius;
};

/** Box collision shape */
class BoxShape :
	public CollisionShape
{
public:
	BoxShape() :
	    halfExtents(0)
	{}

	BoxShape(const math::Vector3r& _halfExtents) :
	    halfExtents(_halfExtents)
	{}

    CollisionShape* clone() const { return new BoxShape(*this); }
    SHAPE_TYPE      getShapeType() const { return BOX; }
    void            applyScaling(const math::Vector3r& scaling)
    {
        halfExtents *= math::Vector3r( fabs(scaling.x), fabs(scaling.y), fabs(scaling.z) );
    }

public:
	math::Vector3r halfExtents;
};

/** Cone collision shape. Up axis is Y.  */
class ConeShape :
	public CollisionShape
{
public:
	ConeShape(real _radius = 0, real _height = 0) :
	    radius(_radius),
	    height(_height)
	{}

    CollisionShape* clone() const { return new ConeShape(*this); }
	SHAPE_TYPE      getShapeType() const { return CONE; }
    void applyScaling(const math::Vector3r& scaling)
    {
        height *= fabs(scaling.z);
        radius *= fabs(scaling.x + scaling.y) / 2;
    }

public:
	real radius;
	real height;
};

/** Capsule collision shape. Up axis is Y. */
class CapsuleShape :
	public CollisionShape
{
public:
	CapsuleShape(real _radius = 0, real _height = 0) :
	    radius(_radius),
	    height(_height)
	{}

    CollisionShape* clone() const { return new CapsuleShape(*this); }
	SHAPE_TYPE      getShapeType() const { return CAPSULE; }
    void applyScaling(const math::Vector3r& scaling)
    {
        height *= fabs(scaling.y);
        radius *= fabs(scaling.x + scaling.z) / 2;
    }

public:
	real radius;	/// thickness of the cylinder
	real height;	/// height of the cylinder
};

/** Capsule collision shape. Up axis is Y. */
class CylinderShape :
	public CollisionShape
{
public:
    CylinderShape(const math::Vector3f& halfExtent_) :
	    halfExtent(halfExtent_)
	{}

    CylinderShape(real x, real y, real z) :
	    halfExtent(x, y, z)
	{}

    CollisionShape* clone() const { return new CylinderShape(*this); }
	SHAPE_TYPE      getShapeType() const { return CYLINDER; }
    void applyScaling(const math::Vector3r& scaling)
    {
        halfExtent *= math::Vector3r( fabs(scaling.x), fabs(scaling.y), fabs(scaling.z) );
    }

public:
    math::Vector3r halfExtent;
};

/** Convex hull collision shape. Collision detection behaviour
 * is undefined if convex shape is not convex). Use buildConvexHull
 * in cases you don't know definetely that the point cloud represents
 * convex hull.
 */
class ConvexShape :
	public CollisionShape
{
public:
	// Override CollisionShape
    CollisionShape* clone() const { return new ConvexShape(*this); }
	SHAPE_TYPE      getShapeType() const { return CONVEX_MESH; }
    void applyScaling(const math::Vector3r& scaling)
    {
        math::Matrix3r matScaling = math::make_matrix( scaling.x,  (real)0.0,  (real)0.0,
                                                       (real)0.0,  scaling.y,  (real)0.0,
                                                       (real)0.0,  (real)0.0,  scaling.z );
        transform_by_matrix( vertices.begin(), vertices.end(), vertices.begin(), matScaling );
    }

	/** Build convex hull of the point cloud and store it as shape.
	 * @tparam Iterator - iterator type referencing 3-component floating tuple.
	 * @param beginIter - begin iterator.
	 * @param beginIter - end iterator.
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
	std::vector<math::Vector3r>	vertices;
};

/** Arbitrary triangle mesh collision shape */
class TriangleMeshShape :
	public CollisionShape
{
public:
    CollisionShape* clone() const { return new TriangleMeshShape(*this); }
	SHAPE_TYPE      getShapeType() const { return TRIANGLE_MESH; }
    void applyScaling(const math::Vector3r& scaling)
    {
        math::Matrix3r matScaling = math::make_matrix( scaling.x,  (real)0.0,  (real)0.0,
                                                       (real)0.0,  scaling.y,  (real)0.0,
                                                       (real)0.0,  (real)0.0,  scaling.z );
        transform_by_matrix( vertices.begin(), vertices.end(), vertices.begin(), matScaling );
    }

public:
	std::vector<math::Vector3r>	vertices;
	std::vector<unsigned>		indices;
};

/** Compound collision shape*/
class CompoundShape :
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

    // Override CollisionShape
    CollisionShape* clone() const { return new CompoundShape(*this); }
    SHAPE_TYPE      getShapeType() const { return COMPOUND; }
    void applyScaling(const math::Vector3r& scaling)
    {
        for (size_t i = 0; i<shapes.size(); ++i) {
            shapes[i].shape->applyScaling(scaling);
        }
    }

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
