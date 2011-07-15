#ifndef __SLON_ENGINE_PHYSICS_CONSTRAINT_H__
#define __SLON_ENGINE_PHYSICS_CONSTRAINT_H__

#include "../Database/Serializable.h"
#include "../Utility/referenced.hpp"
#include "../Utility/Memory/aligned.hpp"
#include "RigidBody.h"
#include <sgl/Math/MatrixFunctions.hpp>
#include <string>

namespace slon {
namespace physics {

/** Generic 6DOF constraint. Implementation will choose most
 * suitable constraint for every case.
 */
class Constraint :
    public Referenced,
    public database::Serializable
{
public:
    friend class DynamicsWorld;
	friend class RigidBody;
#ifdef SLON_ENGINE_USE_BULLET
    friend class BulletConstraint;
    typedef BulletConstraint             impl_type;
    typedef boost::scoped_ptr<impl_type> impl_ptr;
#endif
	typedef boost::scoped_ptr<Motor>     motor_ptr;

public:
    struct state_desc
#ifdef SLON_ENGINE_USE_SSE
       : public aligned<0x10>
#endif
    {
        std::string     name;
        rigid_body_ptr  rigidBodies[2];     /// affected rigid bodies
        math::Matrix4r  frames[2];          /// translational matrices from constraint coordinate frame to rigid bodies coordianate frames
        math::Vector3r  linearLimits[2];    /// translational limits, set -INF, +INF for free axes
        math::Vector3r  angularLimits[2];   /// rotational limits in radians, set -INF, +INF for free axes

        state_desc(const std::string& _name = "") :
            name(_name)
        {
            rigidBodies[0]   = rigidBodies[1]   = 0;
            frames[0]        = frames[1]        = math::make_identity<real, 4>();
            linearLimits[0]  = linearLimits[1]  = math::Vector3r(0);
            angularLimits[0] = angularLimits[1] = math::Vector3r(0);
        }
    };

    enum AXIS_RESTRICTION
    {
        AXIS_FREE,
        AXIS_RESTRICTED,
        AXIS_LOCKED,
    };

	enum DOF
    {
        DOF_X_TRANS,
        DOF_Y_TRANS,
        DOF_Z_TRANS,
        DOF_X_ROT,
        DOF_Y_ROT,
        DOF_Z_ROT
    };

public:
    Constraint(const state_desc& desc = state_desc());

    // Override Serializable
    const char* serialize(database::OArchive& ar) const;
    void        deserialize(database::IArchive& ar);

    /** Get first rigid body affecting by the constraint. */
    RigidBody* getRigidBodyA() const;

    /** Get second rigid body affecting by the constraint. */
    RigidBody* getRigidBodyB() const;

    /** Get motor of the constraint per axis. 
     * @param motor - motor id.
     * @return pointer to the angular motor for specified axis or 0 if it was not created.
     */
    const Motor* getMotor(DOF motor) const;

    /** Get motor of the constraint per axis. 
     * @param motor - motor id.
     * @return pointer to the angular motor for specified axis or 0 if it was not created.
     */
    Motor* getMotor(DOF motor);

    /** Setup servo motor of the constraint. Previous motor on specified axis will be deleted.
     * @param dof - dof motor should affect.
     * @return pointer to the motor for specified axis or 0 is axis is locked.
     */
    ServoMotor* createServoMotor(DOF dof);

    /** Setup velocity motor of the constraint. Previous motor on specified axis will be deleted.
     * @param dof - dof motor should affect.
     * @return pointer to the motor for specified axis or 0 is axis is locked.
     */
    VelocityMotor* createVelocityMotor(DOF dof);

    /** Setup spring motor of the constraint. Previous motor on specified axis will be deleted.
     * @param dof - dof motor should affect.
     * @return pointer to the motor for specified axis or 0 is axis is locked.
     */
    SpringMotor* createSpringMotor(DOF dof);

    /** Get axis in parent coordinate frame.
     * @param axis - 0 - X, 1 - Y, 2 - Z.
     */
    math::Vector3r getAxis(unsigned int axis) const;

    /** Get current angle of the rotational axis or current offset of translational axis. */
    real getPosition(DOF dof) const;

    /** Check whether specified degree of freedom is locked, restricted or free.  */
    AXIS_RESTRICTION getRestriction(DOF dof) const;

    /** Get name of the constraint */
    const std::string& getName() const;

    /** Get description of the constraint. */
    const state_desc& getStateDesc() const;

    /** Get dynamics world where constraint is. */
    const DynamicsWorld* getDynamicsWorld() const;

    /** Recreate constraint from description. */
    void reset(const state_desc& desc);

	/** Get implementation object */
	impl_type* getImpl() { return impl.get(); }

	/** Get implementation object */
	const impl_type* getImpl() const { return impl.get(); }

private:
	/** Add constraint into the world, should be called by DynamicsWorld. */
	void setWorld(const dynamics_world_ptr& world);

	/** Create constraint implementation, should be called by RigidBody. */
	void instantiate();

	/** Release constraint implementation, should be called by RigidBody. */
	void release();

private:
	dynamics_world_ptr world;
	state_desc         desc;
	impl_ptr           impl;
	motor_ptr          motors[6];
};

} // namespace physics
} // namespace slon

#endif // __SLON_ENGINE_PHYSICS_CONSTRAINT_H__
