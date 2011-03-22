#ifndef __SLON_ENGINE_PHYSICS_FORWARD_H__
#define __SLON_ENGINE_PHYSICS_FORWARD_H__

#include "../Config.h"

namespace boost 
{
    template<typename T>
    class intrusive_ptr;
}

namespace math 
{
	template<typename T, int n, int m>
	class Matrix;

	template<typename T, int n>
	class AABB;

	template<typename T, int n>
	class Plane;

	template<typename T>
	class Quaternion;

	typedef Matrix<slon::physics::real, 4, 1>      Vector4r;
	typedef Matrix<slon::physics::real, 3, 1>      Vector3r;
	typedef Matrix<slon::physics::real, 2, 1>      Vector2r;

	typedef Matrix<slon::physics::real, 4, 4>      Matrix4x4r; 
	typedef Matrix<slon::physics::real, 4, 3>      Matrix4x3r;
	typedef Matrix<slon::physics::real, 4, 2>      Matrix4x2r;
	typedef Matrix<slon::physics::real, 3, 4>      Matrix3x4r;
	typedef Matrix<slon::physics::real, 3, 3>      Matrix3x3r;
	typedef Matrix<slon::physics::real, 3, 2>      Matrix3x2r;
	typedef Matrix<slon::physics::real, 2, 4>      Matrix2x4r;
	typedef Matrix<slon::physics::real, 2, 3>      Matrix2x3r;
	typedef Matrix<slon::physics::real, 2, 2>      Matrix2x2r;
	typedef Matrix<slon::physics::real, 4, 4>      Matrix4r;   
	typedef Matrix<slon::physics::real, 3, 3>      Matrix3r;
	typedef Matrix<slon::physics::real, 2, 2>      Matrix2r;

	typedef AABB<slon::physics::real, 2>		   AABB2r;
	typedef AABB<slon::physics::real, 3>		   AABB3r;
	typedef AABB<slon::physics::real, 4>		   AABB4r;
	typedef AABB<slon::physics::real, 3>		   AABBr;

	typedef Plane<slon::physics::real, 2>          Liner;
	typedef Plane<slon::physics::real, 3>          Plane3r;
	typedef Plane<slon::physics::real, 3>          Planer;

	typedef Quaternion<slon::physics::real>        Quaternionr;
}

namespace slon {
namespace physics {

class CollisionShape;
class Constraint;
class DynamicsWorld;
class Motor;
class PhysicsMaterial;
class PhysicsModel;
class RigidBody;
class RigidBodyTransform;
class ServoMotor;
class SpringMotor;
class VelocityMotor;

typedef boost::intrusive_ptr<CollisionShape>			collision_shape_ptr;
typedef boost::intrusive_ptr<const CollisionShape>		const_collision_shape_ptr;
typedef boost::intrusive_ptr<Constraint>				constraint_ptr;
typedef boost::intrusive_ptr<const Constraint>			const_constraint_ptr;
typedef boost::intrusive_ptr<DynamicsWorld>				dynamics_world_ptr;
typedef boost::intrusive_ptr<const DynamicsWorld>		const_dynamics_world_ptr;
typedef boost::intrusive_ptr<PhysicsMaterial>			physics_material_ptr;
typedef boost::intrusive_ptr<const PhysicsMaterial>		const_physics_material_ptr;
typedef boost::intrusive_ptr<PhysicsModel>				physics_model_ptr;
typedef boost::intrusive_ptr<const PhysicsModel>		const_physics_model_ptr;
typedef boost::intrusive_ptr<RigidBody>					rigid_body_ptr;
typedef boost::intrusive_ptr<const RigidBody>			const_rigid_body_ptr;
typedef boost::intrusive_ptr<RigidBodyTransform>        rigid_body_transform_ptr;
typedef boost::intrusive_ptr<const RigidBodyTransform>  const_rigid_body_transform_ptr;

typedef math::Matrix<real, 4, 1>   Vector4r;
typedef math::Matrix<real, 3, 1>   Vector3r;
typedef math::Matrix<real, 2, 1>   Vector2r;

typedef math::Matrix<real, 4, 4>   Matrix4x4r; 
typedef math::Matrix<real, 4, 3>   Matrix4x3r;
typedef math::Matrix<real, 4, 2>   Matrix4x2r;
typedef math::Matrix<real, 3, 4>   Matrix3x4r;
typedef math::Matrix<real, 3, 3>   Matrix3x3r;
typedef math::Matrix<real, 3, 2>   Matrix3x2r;
typedef math::Matrix<real, 2, 4>   Matrix2x4r;
typedef math::Matrix<real, 2, 3>   Matrix2x3r;
typedef math::Matrix<real, 2, 2>   Matrix2x2r;
typedef math::Matrix<real, 4, 4>   Matrix4r;   
typedef math::Matrix<real, 3, 3>   Matrix3r;
typedef math::Matrix<real, 2, 2>   Matrix2r;

typedef math::AABB<real, 2>		   AABB2r;
typedef math::AABB<real, 3>		   AABB3r;
typedef math::AABB<real, 4>		   AABB4r;
typedef math::AABB<real, 3>		   AABBr;

typedef math::Plane<real, 2>       Liner;
typedef math::Plane<real, 3>       Plane3r;
typedef math::Plane<real, 3>       Planer;

typedef math::Quaternion<real>     Quaternionr;

} // namespace physics
} // namespace slon

#endif // __SLON_ENGINE_PHYSICS_FORWARD_H__