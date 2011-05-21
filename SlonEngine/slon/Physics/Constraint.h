#ifndef __SLON_ENGINE_PHYSICS_CONSTRAINT_H__
#define __SLON_ENGINE_PHYSICS_CONSTRAINT_H__

#include <string>
#include <sgl/Math/MatrixFunctions.hpp>
#include "../Utility/referenced.hpp"
#include "../Utility/Memory/aligned.hpp"
#include "Forward.h"
#include "Motor.h"

namespace slon {
namespace physics {

// forward decl
class RigidBody;

/** Generic 6DOF constraint. Implementation will choose most
 * suitable constraint for every case.
 */
class Constraint :
    public Referenced
{
public:
    struct state_desc
#ifdef SLON_ENGINE_USE_SSE
       : public aligned<0x10>
#endif
    {
        RigidBody*      rigidBodies[2];
        math::Matrix4r  frames[2];
        math::Vector3r  linearLimits[2];
        math::Vector3r  angularLimits[2];
        std::string     name;

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

public:
    /** Get first rigid body affecting by the constraint. */
    virtual RigidBody* getRigidBodyA() const = 0;

    /** Get second rigid body affecting by the constraint. */
    virtual RigidBody* getRigidBodyB() const = 0;

    /** Get motor of the constraint per axis. 
     * @param motor - motor id.
     * @return pointer to the angular motor for specified axis or 0 if it was not created.
     */
    virtual const Motor* getMotor(Motor::TYPE motor) const = 0;

    /** Get motor of the constraint per axis. 
     * @param motor - motor id.
     * @return pointer to the angular motor for specified axis or 0 if it was not created.
     */
    virtual Motor* getMotor(Motor::TYPE motor) = 0;

    /** Setup servo motor of the constraint per axis. Previous motor on specified axis will be deleted.
     * @param motor - motor id.
     * @return pointer to the angular motor for specified axis or 0 is axis is locked.
     */
    virtual ServoMotor* createServoMotor(Motor::TYPE motor) = 0;

    /** Setup velocity motor of the constraint per axis. Previous motor on specified axis will be deleted.
     * @param motor - motor id.
     * @return pointer to the angular motor for specified axis or 0 is axis is locked.
     */
    virtual VelocityMotor* createVelocityMotor(Motor::TYPE motor) = 0;

    /** Setup spring motor of the constraint per axis. Previous motor on specified axis will be deleted.
     * @param motor - motor id.
     * @return pointer to the angular motor for specified axis or 0 is axis is locked.
     */
    virtual SpringMotor* createSpringMotor(Motor::TYPE motor) = 0;

    /** Get axis in parent coordinate frame.
     * @param axis - 0 - X, 1 - Y, 2 - Z.
     */
    virtual math::Vector3r getAxis(unsigned int axis) const = 0;

    /** Get current angle of the rotational axis. 
     * @param axis - 0 - X, 1 - Y, 2 - Z.
     */
    virtual real getRotationAngle(unsigned int axis) const = 0;

    /** Check wether specified axis is locked.
     * @param axis - 0 - X, 1 - Y, 2 - Z.
     */
    virtual AXIS_RESTRICTION getAxisRestriction(unsigned int axis) const = 0;

    /** Get name of the constraint */
    virtual const std::string& getName() const = 0;

    /** Get description of the constraint. */
    virtual const state_desc& getStateDesc() const = 0;

    /** Recreate constraint from description. */
    virtual void reset(const state_desc& desc) = 0;

    virtual ~Constraint() {}
};

// ptr typedefs
typedef boost::intrusive_ptr<Constraint>        constraint_ptr;
typedef boost::intrusive_ptr<const Constraint>  const_constraint_ptr;

} // namespace physics
} // namespace slon

#endif // __SLON_ENGINE_PHYSICS_CONSTRAINT_H__
