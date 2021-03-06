#ifndef __SLON_ENGINE_PHYSICS_COLLISION_OBJECT_H__
#define __SLON_ENGINE_PHYSICS_COLLISION_OBJECT_H__

#include "../Database/Serializable.h"
#include "../Math/RigidTransform.hpp"
#include "../Utility/referenced.hpp"
#include "../Utility/signal.hpp"
#include "Forward.h"

namespace slon {
namespace physics {

// forward
class CollisionObject;
class CollisionShape;
class DynamicsWorld;

/** Struct for retrieving contact information */
struct Contact
{
    CollisionObject*    collisionObjects[2];
    math::Vector3r*     contacts;
    size_t              numContacts;

    Contact() :
        contacts(0),
        numContacts(0)
    {
        collisionObjects[0] = 0;
        collisionObjects[1] = 0;
    }

    Contact(CollisionObject* a, CollisionObject* b) :
        contacts(0),
        numContacts(0)
    {
        collisionObjects[0] = a;
        collisionObjects[1] = b;
    }
};

#ifdef SLON_ENGINE_USE_BULLET
    class BulletCollisionObject;
#endif

/** Rigid body in the phyics world */
class SLON_PUBLIC CollisionObject :
    public Referenced,
    public database::Serializable
{
private:
#ifdef SLON_ENGINE_USE_BULLET
    friend class BulletCollisionObject;
	typedef BulletCollisionObject impl_type;
#endif
public:
    typedef slot<void (const math::RigidTransformr&)>           transform_handler;
    typedef slot<void (const Contact&)>                         contact_handler;

    typedef signal<void (const math::RigidTransformr&)>         transform_signal;
    typedef signal_base<void (const math::RigidTransformr&)>    transform_signal_base;
    typedef signal<void (const Contact&)>                       contact_signal;
    typedef signal_base<void (const Contact&)>                  contact_signal_base;

    enum COLLISION_TYPE
    {
        CT_GHOST,      /// Ghost objects only register collisions.
        CT_RIGID_BODY  /// Interacts with other world objects. @see RigidBody
    };

public:
    /** Get behaviour of the object collision. */
    virtual COLLISION_TYPE getType() const = 0;

    /** Get objects collision shape. */
    virtual const CollisionShape* getCollisionShape() const = 0;

    /** Get dynamics world where object is created. */
    virtual const DynamicsWorld* getDynamicsWorld() const = 0;

    /** Get name of the object. */
    virtual const std::string& getName() const = 0;

    /** Get internal transformation matrix pointer, so other objects can quickly track collision objects movements. 
     * This pointer is NULL if implementation object is not created (e.g. object is not in the world).
     * This pointer may change if reset function called.
     */
    virtual const math::RigidTransformr* getTransformPointer() const = 0;

    /** Get local to world transform matrix of the object. */
    virtual const math::RigidTransformr& getTransform() const = 0;

    /** Set world transform for the object. Works only for ghost objects and kinematic rigid bodies */
    virtual void setTransform(const math::RigidTransformr& transform) = 0;

    /** Get signal for connecting transform handlers. */
    transform_signal_base& getTransformSignal() { return transformSignal; }

    /** Get signal for connecting contact appear handlers. */
    contact_signal_base& getContactAppearSignal() { return contactAppearSignal; }

    /** Get signal for connecting contact dissapear handlers. */
    contact_signal_base& getContactDissapearSignal() { return contactDissapearSignal; }

	/** Get implementation object. */
    impl_type* getImpl();

	/** Get implementation object. */
    const impl_type* getImpl() const;

    virtual ~CollisionObject() {}

protected:
    transform_signal    transformSignal;
    contact_signal      contactAppearSignal;
    contact_signal      contactDissapearSignal;
};

// ptr typedef
typedef boost::intrusive_ptr<CollisionObject>           collision_object_ptr;
typedef boost::intrusive_ptr<const CollisionObject>     const_collision_object_ptr;

} // namespace physics
} // namespace slon

#endif // __SLON_ENGINE_PHYSICS_COLLISION_OBJECT_H__
