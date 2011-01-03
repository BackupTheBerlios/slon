#ifndef __SLON_ENGINE_PHYSICS_BULLET_BULLET_COMMON_H__
#define __SLON_ENGINE_PHYSICS_BULLET_BULLET_COMMON_H__

#include "../CollisionShape.h"
#include <btBulletCollisionCommon.h>
#include <sgl/Math/Matrix.hpp>

namespace slon {
namespace physics {

inline btVector3 to_bt_vec(const math::Vector3f& vec)
{
    return btVector3(vec.x, vec.y, vec.z);
}

inline btVector4 to_bt_vec(const math::Vector4f& vec)
{
    return btVector4(vec.x, vec.y, vec.z, vec.w);
}

inline btMatrix3x3 to_bt_mat(const math::Matrix3f& mat)
{
	return btMatrix3x3( mat[0][0], mat[0][1], mat[0][2],
                        mat[1][0], mat[1][1], mat[1][2],
                        mat[2][0], mat[2][1], mat[2][2] );
}

inline btTransform to_bt_mat(const math::Matrix4f& mat)
{
	btMatrix3x3 basis( mat[0][0], mat[0][1], mat[0][2],
                       mat[1][0], mat[1][1], mat[1][2],
                       mat[2][0], mat[2][1], mat[2][2] );

	return btTransform( basis, btVector3(mat[0][3], mat[1][3], mat[2][3]) );
}

inline math::Vector3f to_vec(const btVector3& vec)
{
    return math::Vector3f( vec.x(), vec.y(), vec.z() );
}

inline math::Vector4f to_vec(const btVector4& vec)
{
    return math::Vector4f( vec.x(), vec.y(), vec.z(), vec.w() );
}

inline math::Matrix4f to_mat(const btTransform& transform)
{
	const btMatrix3x3& basis  = transform.getBasis();
	const btVector3&   origin = transform.getOrigin();

	return math::make_matrix( basis[0].x(), basis[0].y(), basis[0].z(), origin.x(),
						      basis[1].x(), basis[1].y(), basis[1].z(), origin.y(),
					          basis[2].x(), basis[2].y(), basis[2].z(), origin.z(),
						      0.0f,         0.0f,         0.0f,         1.0f );
}

// create bullet collision shape from slon collision shape
inline btCollisionShape* createBtCollisionShape(const CollisionShape& collisionShape)
{
	switch ( collisionShape.getShapeType() )
	{
		case CollisionShape::PLANE:
		{
			const PlaneShape&   planeShape           = static_cast<const PlaneShape&>(collisionShape);
			btStaticPlaneShape* bulletCollisionShape = new btStaticPlaneShape( to_bt_vec(planeShape.plane.normal), planeShape.plane.distance );
            bulletCollisionShape->setUserPointer( const_cast<CollisionShape*>(&collisionShape) );
            return bulletCollisionShape;
		}

		case CollisionShape::SPHERE:
		{
			const SphereShape& sphereShape          = static_cast<const SphereShape&>(collisionShape);
			btSphereShape*     bulletCollisionShape = new btSphereShape(sphereShape.radius);
            bulletCollisionShape->setUserPointer( const_cast<CollisionShape*>(&collisionShape) );
            return bulletCollisionShape;
		}

		case CollisionShape::BOX:
		{
			const BoxShape& boxShape             = static_cast<const BoxShape&>(collisionShape);
			btBoxShape*     bulletCollisionShape = new btBoxShape( to_bt_vec(boxShape.halfExtents) );
            bulletCollisionShape->setUserPointer( const_cast<CollisionShape*>(&collisionShape) );
            return bulletCollisionShape;
		}

		case CollisionShape::CONE:
		{
			const ConeShape& coneShape            = static_cast<const ConeShape&>(collisionShape);
			btConeShapeZ*    bulletCollisionShape = new btConeShapeZ(coneShape.radius, coneShape.height);
            bulletCollisionShape->setUserPointer( const_cast<CollisionShape*>(&collisionShape) );
            return bulletCollisionShape;
		}

		case CollisionShape::CONVEX_MESH:
		{
			const ConvexShape& convexShape = static_cast<const ConvexShape&>(collisionShape);
			btConvexHullShape* bulletCollisionShape = new btConvexHullShape( &convexShape.vertices[0].x,
                                                                             convexShape.vertices.size(),
                                                                             sizeof(math::Vector3f) );
            bulletCollisionShape->setUserPointer( const_cast<CollisionShape*>(&collisionShape) );
            return bulletCollisionShape;
		}

		case CollisionShape::TRIANGLE_MESH:
		{
		    const TriangleMeshShape& triangleMeshShape = static_cast<const TriangleMeshShape&>(collisionShape);

		    btTriangleIndexVertexArray* indexVertexArray = new btTriangleIndexVertexArray( triangleMeshShape.indices.size() / 3,
                                                                                           (int*)&triangleMeshShape.indices[0],
                                                                                           3 * sizeof(unsigned),
                                                                                           triangleMeshShape.vertices.size(),
                                                                                           (btScalar*)&triangleMeshShape.vertices[0].x,
                                                                                           sizeof(math::Vector3f) );
		    math::AABBf aabb = compute_aabb( triangleMeshShape.vertices.begin(),
                                             triangleMeshShape.vertices.end() );

		    btBvhTriangleMeshShape* bulletCollisionShape = new btBvhTriangleMeshShape( indexVertexArray, true, to_bt_vec( xyz(aabb.minVec) ), to_bt_vec( xyz(aabb.maxVec) ) );
            bulletCollisionShape->setUserPointer( const_cast<CollisionShape*>(&collisionShape) );
            return bulletCollisionShape;
		}

		case CollisionShape::COMPOUND:
		{
		    const CompoundShape& compoundShape = static_cast<const CompoundShape&>(collisionShape);

            // create compound shape from several shapes
            btCompoundShape* bulletCompoundCollisionShape = new btCompoundShape();
            for( CompoundShape::const_shape_trasnform_iterator shapeIter = compoundShape.shapes.begin();
                                                               shapeIter != compoundShape.shapes.end();
                                                               ++shapeIter )
            {
                btCollisionShape* btChildCollisionShape = createBtCollisionShape(*shapeIter->shape);
                bulletCompoundCollisionShape->addChildShape( to_bt_mat(shapeIter->transform), btChildCollisionShape );
            }
            bulletCompoundCollisionShape->setUserPointer( const_cast<CollisionShape*>(&collisionShape) );

            return bulletCompoundCollisionShape;
        }

		default:
			assert(!"Can't get here");
	}

    return 0;
}

// create slon collision shape from bullet collision shape
// TODO: Add shapes
inline CollisionShape* createCollisionShape(const btCollisionShape& collisionShape)
{
	if ( const btStaticPlaneShape* btShape = dynamic_cast<const btStaticPlaneShape*>(&collisionShape) )
    {
        return new PlaneShape( math::Planef( to_vec(btShape->getPlaneNormal()), btShape->getPlaneConstant() ) );
    }
    else if ( const btSphereShape* btShape = dynamic_cast<const btSphereShape*>(&collisionShape) )
    {
        return new SphereShape( btShape->getRadius() );
    }
    else if ( const btBoxShape* btShape = dynamic_cast<const btBoxShape*>(&collisionShape) ) 
    {
        return new BoxShape( to_vec( btShape->getHalfExtentsWithoutMargin() ) );
    }
    else if ( const btConeShapeZ* btShape = dynamic_cast<const btConeShapeZ*>(&collisionShape) )
    {
        return new ConeShape( btShape->getRadius(), btShape->getHeight() );
    }

    return 0;
}

} // namespace slon
} // namespace physics

#endif // __SLON_ENGINE_PHYSICS_BULLET_BULLET_COMMON_H__
