#include "../Config.h"

namespace boost 
{
    template<typename T>
    class intrusive_ptr;
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
class ServoMotor;
class SpringMotor;
class VelocityMotor;

typedef boost::intrusive_ptr<CollisionShape>        collision_shape_ptr;
typedef boost::intrusive_ptr<const CollisionShape>  const_collision_shape_ptr;
typedef boost::intrusive_ptr<Constraint>            constraint_ptr;
typedef boost::intrusive_ptr<const Constraint>      const_constraint_ptr;
typedef boost::intrusive_ptr<DynamicsWorld>         dynamics_world_ptr;
typedef boost::intrusive_ptr<const DynamicsWorld>   const_dynamics_world_ptr;
typedef boost::intrusive_ptr<PhysicsMaterial>       physics_material_ptr;
typedef boost::intrusive_ptr<const PhysicsMaterial> const_physics_material_ptr;
typedef boost::intrusive_ptr<PhysicsModel>          physics_model_ptr;
typedef boost::intrusive_ptr<const PhysicsModel>    const_physics_model_ptr;
typedef boost::intrusive_ptr<RigidBody>             rigid_body_ptr;
typedef boost::intrusive_ptr<const RigidBody>       const_rigid_body_ptr;

} // namespace physics
} // namespace slon