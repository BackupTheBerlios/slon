#ifndef __SLON_ENGINE_PHYSICS_BULLET_BULLET_COMMON_H__
#define __SLON_ENGINE_PHYSICS_BULLET_BULLET_COMMON_H__

#include "../../Math/RigidTransform.hpp"
#include "../CollisionShape.h"
#include <bullet/btBulletCollisionCommon.h>
#include <bullet/btBulletDynamicsCommon.h>

namespace slon {
namespace physics {

inline btVector3 to_bt_vec(const math::Vector3r& vec)
{
    return btVector3(vec.x, vec.y, vec.z);
}

inline btVector4 to_bt_vec(const math::Vector4r& vec)
{
    return btVector4(vec.x, vec.y, vec.z, vec.w);
}

inline btMatrix3x3 to_bt_mat(const math::Matrix3r& mat)
{
	return btMatrix3x3( mat[0][0], mat[0][1], mat[0][2],
                        mat[1][0], mat[1][1], mat[1][2],
                        mat[2][0], mat[2][1], mat[2][2] );
}

inline btTransform to_bt_mat(const math::Matrix4r& mat)
{
	btMatrix3x3 basis( mat[0][0], mat[0][1], mat[0][2],
                       mat[1][0], mat[1][1], mat[1][2],
                       mat[2][0], mat[2][1], mat[2][2] );

	return btTransform( basis, btVector3(mat[0][3], mat[1][3], mat[2][3]) );
}

inline math::Vector3r to_vec(const btVector3& vec)
{
    return math::Vector3r( vec.x(), vec.y(), vec.z() );
}

inline math::Vector4r to_vec(const btVector4& vec)
{
    return math::Vector4r( vec.x(), vec.y(), vec.z(), vec.w() );
}

inline math::RigidTransformr to_mat(const btTransform& transform)
{
	const btMatrix3x3& basis  = transform.getBasis();
	const btVector3&   origin = transform.getOrigin();

	return math::RigidTransformr( basis[0].x(), basis[0].y(), basis[0].z(), origin.x(),
						          basis[1].x(), basis[1].y(), basis[1].z(), origin.y(),
					              basis[2].x(), basis[2].y(), basis[2].z(), origin.z(),
						          (real)0.0,    (real)0.0,   (real)0.0,     (real)1.0, 
                                  -1.0f );
}

// create bullet collision shape from slon collision shape
inline btCollisionShape* createBtCollisionShape(const CollisionShape& collisionShape, real relativeMargin, real margin)
{
	switch ( collisionShape.getShapeType() )
	{
		case CollisionShape::PLANE:
		{
			const PlaneShape&   planeShape           = static_cast<const PlaneShape&>(collisionShape);
			btStaticPlaneShape* bulletCollisionShape = new btStaticPlaneShape( to_bt_vec(planeShape.plane.normal), planeShape.plane.distance );
            bulletCollisionShape->setUserPointer( const_cast<CollisionShape*>(&collisionShape) );
			bulletCollisionShape->setMargin(margin);
            return bulletCollisionShape;
		}

		case CollisionShape::SPHERE:
		{
			const SphereShape& sphereShape          = static_cast<const SphereShape&>(collisionShape);
			btSphereShape*     bulletCollisionShape = new btSphereShape(sphereShape.radius);
            bulletCollisionShape->setUserPointer( const_cast<CollisionShape*>(&collisionShape) );
			bulletCollisionShape->setMargin(sphereShape.radius*relativeMargin + margin);
            return bulletCollisionShape;
		}

		case CollisionShape::BOX:
		{
			const BoxShape& boxShape             = static_cast<const BoxShape&>(collisionShape);
			btBoxShape*     bulletCollisionShape = new btBoxShape( to_bt_vec(boxShape.halfExtent) );
            bulletCollisionShape->setUserPointer( const_cast<CollisionShape*>(&collisionShape) );
			bulletCollisionShape->setMargin(2*std::min(boxShape.halfExtent.x, std::min(boxShape.halfExtent.y, boxShape.halfExtent.z))*relativeMargin + margin);
            return bulletCollisionShape;
		}

		case CollisionShape::CAPSULE:
		{
			const CapsuleShape& capsuleShape         = static_cast<const CapsuleShape&>(collisionShape);
			btCapsuleShape*     bulletCollisionShape = new btCapsuleShape(capsuleShape.radius, capsuleShape.height);
            bulletCollisionShape->setUserPointer( const_cast<CollisionShape*>(&collisionShape) );
            bulletCollisionShape->setMargin( std::min(capsuleShape.height, capsuleShape.radius)*relativeMargin + margin );
            return bulletCollisionShape;
		}

		case CollisionShape::CONE:
		{
			const ConeShape& coneShape            = static_cast<const ConeShape&>(collisionShape);
			btConeShape*     bulletCollisionShape = new btConeShape(coneShape.radius, coneShape.height);
            bulletCollisionShape->setUserPointer( const_cast<CollisionShape*>(&collisionShape) );
            bulletCollisionShape->setMargin( std::min(coneShape.height, coneShape.radius)*relativeMargin + margin );
            return bulletCollisionShape;
		}

		case CollisionShape::CYLINDER_X:
		{
			const CylinderXShape& cylShape             = static_cast<const CylinderXShape&>(collisionShape);
			btCylinderShape*      bulletCollisionShape = new btCylinderShapeX( to_bt_vec(cylShape.halfExtent) );
            bulletCollisionShape->setUserPointer( const_cast<CollisionShape*>(&collisionShape) );
            bulletCollisionShape->setMargin( 2*std::min(cylShape.halfExtent.x, std::min(cylShape.halfExtent.y, cylShape.halfExtent.z))*relativeMargin + margin );
            return bulletCollisionShape;
		}

		case CollisionShape::CYLINDER_Y:
		{
			const CylinderYShape& cylShape             = static_cast<const CylinderYShape&>(collisionShape);
			btCylinderShape*      bulletCollisionShape = new btCylinderShape( to_bt_vec(cylShape.halfExtent) );
            bulletCollisionShape->setUserPointer( const_cast<CollisionShape*>(&collisionShape) );
            bulletCollisionShape->setMargin( 2*std::min(cylShape.halfExtent.x, std::min(cylShape.halfExtent.y, cylShape.halfExtent.z))*relativeMargin + margin );
            return bulletCollisionShape;
		}

		case CollisionShape::CYLINDER_Z:
		{
			const CylinderZShape& cylShape             = static_cast<const CylinderZShape&>(collisionShape);
			btCylinderShape*      bulletCollisionShape = new btCylinderShapeZ( to_bt_vec(cylShape.halfExtent) );
            bulletCollisionShape->setUserPointer( const_cast<CollisionShape*>(&collisionShape) );
            bulletCollisionShape->setMargin( 2*std::min(cylShape.halfExtent.x, std::min(cylShape.halfExtent.y, cylShape.halfExtent.z))*relativeMargin + margin );
            return bulletCollisionShape;
		}

		case CollisionShape::CONVEX_MESH:
		{
			const ConvexShape& convexShape = static_cast<const ConvexShape&>(collisionShape);
			btConvexHullShape* bulletCollisionShape = new btConvexHullShape( &convexShape.vertices[0].x,
                                                                             convexShape.vertices.size(),
                                                                             sizeof(math::Vector3r) );
            bulletCollisionShape->setUserPointer( const_cast<CollisionShape*>(&collisionShape) );

            // find center
            math::Vector3r center(0);
            for (size_t i = 0; i<convexShape.vertices.size(); ++i) {
                center += convexShape.vertices[i] / convexShape.vertices.size();
            }

            // find min dimension
            float minDimension = std::numeric_limits<real>::max();
            for (size_t i = 0; i<convexShape.vertices.size(); ++i) {
                minDimension = std::min(minDimension, math::length(convexShape.vertices[i] - center));
            }
			
            bulletCollisionShape->setMargin(minDimension*relativeMargin + margin);
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
                                                                                           sizeof(math::Vector3r) );
		    math::AABBr aabb = compute_aabb<real>( triangleMeshShape.vertices.begin(),
												   triangleMeshShape.vertices.end() );
            float minDimension = std::min(aabb.size().x, std::min(aabb.size().y, aabb.size().z));

		    btBvhTriangleMeshShape* bulletCollisionShape = new btBvhTriangleMeshShape( indexVertexArray, true, to_bt_vec( xyz(aabb.minVec) ), to_bt_vec( xyz(aabb.maxVec) ) );
            bulletCollisionShape->setUserPointer( const_cast<CollisionShape*>(&collisionShape) );
            bulletCollisionShape->setMargin(minDimension*relativeMargin + margin);

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
                btCollisionShape* btChildCollisionShape = createBtCollisionShape(*shapeIter->shape, relativeMargin, margin);
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
        return new PlaneShape( math::Planer( to_vec(btShape->getPlaneNormal()), btShape->getPlaneConstant() ) );
    }
    else if ( const btSphereShape* btShape = dynamic_cast<const btSphereShape*>(&collisionShape) )
    {
        return new SphereShape( btShape->getRadius() );
    }
    else if ( const btBoxShape* btShape = dynamic_cast<const btBoxShape*>(&collisionShape) ) 
    {
        return new BoxShape( to_vec( btShape->getHalfExtentsWithoutMargin() ) );
    }
    else if ( const btConeShape* btShape = dynamic_cast<const btConeShape*>(&collisionShape) )
    {
        switch ( btShape->getConeUpIndex() ) 
        {
        case 0:
            return new CompoundShape( math::Matrix4r::rotation_z(math::HALF_PI), new ConeShape(btShape->getRadius(), btShape->getHeight()) );

        case 1:
            return new ConeShape(btShape->getRadius(), btShape->getHeight());

        case 2:
            return new CompoundShape( math::Matrix4r::rotation_x(-math::HALF_PI), new ConeShape(btShape->getRadius(), btShape->getHeight()) );

        default:
            assert(!"can't get here");
        }
    }
    else if ( const btCapsuleShape* btShape = dynamic_cast<const btCapsuleShape*>(&collisionShape) )
    {
        switch ( btShape->getUpAxis() ) 
        {
        case 0:
            return new CompoundShape( math::Matrix4r::rotation_z(-math::HALF_PI), new CapsuleShape(btShape->getRadius(), btShape->getHalfHeight() * 2) );

        case 1:
            return new CapsuleShape(btShape->getRadius(), btShape->getHalfHeight() * 2);

        case 2:
            return new CompoundShape( math::Matrix4r::rotation_x(-math::HALF_PI), new CapsuleShape(btShape->getRadius(), btShape->getHalfHeight() * 2) );

        default:
            assert(!"can't get here");
        }
    }
    else if ( const btCylinderShape* btShape = dynamic_cast<const btCylinderShape*>(&collisionShape) )
    {
        math::Vector3r halfExtent = to_vec(btShape->getHalfExtentsWithoutMargin());
        switch ( btShape->getUpAxis() ) 
        {
        case 0:
            return new CylinderXShape(halfExtent);

        case 1:
            return new CylinderYShape(halfExtent);

        case 2:
            return new CylinderZShape(halfExtent);

        default:
            assert(!"can't get here");
        }
    }

    return 0;
}

} // namespace slon
} // namespace physics

#endif // __SLON_ENGINE_PHYSICS_BULLET_BULLET_COMMON_H__
