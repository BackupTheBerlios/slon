#ifndef __SLON_ENGINE_PHYSICS_COLLISION_OBJECT_H__
#define __SLON_ENGINE_PHYSICS_COLLISION_OBJECT_H__

#include "../Utility/referenced.hpp"
#include <boost/signals.hpp>
#include <sgl/Math/Matrix.hpp>

namespace slon {
namespace physics {

// forward
class CollisionObject;
class CollisionShape;
class DynamicsWorld;

/** Struct for retreiving contact information */
struct Contact
{
    CollisionObject*    collisionObjects[2];
    math::Vector3f*     contacts;
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

/** Rigid body in the phyics world */
class CollisionObject :
    public Referenced
{
public:
    typedef boost::function<void (const Contact& c)>    contact_handler;
    typedef boost::signals::connection                  connection_type;

    enum COLLISION_TYPE
    {
        CT_GHOST,      /// Ghost objects only register collisions.
        CT_RIGID_BODY  /// Interacts with other world objects. @see RigidBody
    };

public:
    /** Get behaviour of the object collision. */
    virtual COLLISION_TYPE getCollisionType() const = 0;

    /** Get objects collision shape. */
    virtual const CollisionShape* getCollisionShape() const = 0;

    /** Get dynamics world where object is created. */
    virtual const DynamicsWorld& getDynamicsWorld() const = 0;

    /** Get name of the objecty */
    virtual const std::string& getName() const = 0;

    /** Get local to world transform matrix of the object. */
    virtual math::Matrix4f getTransform() const = 0;

    /** Set world transform for the object. Works only for ghost objects and kinematic rigid bodies */
    virtual void setTransform(const math::Matrix4f& transform) = 0;

    /** Connect callback for handling contact occuring. Callback is called for
     * every appearing contact pair.
     * @param handler - contact handler.
     */
    virtual connection_type connectContactAppearCallback(const contact_handler& handler) = 0;

    /** Connect callback for handling contact dissapearing. Callback is called for every
     * dissapearing contact.
     * @param handler - contact handler.
     */
    virtual connection_type connectContactDissapearCallback(const contact_handler& handler) = 0;

    /** Call appearing contact handlers. This function is called from DynamicsWorld
     * during update.
     */
    virtual void handleAppearingContact(const Contact& contact) = 0;

    /** Call dissapearing contact handlers. This function is called from DynamicsWorld
     * during update.
     */
    virtual void handleDissappearingContact(const Contact& contact) = 0;

    virtual ~CollisionObject() {}
};

// ptr typedef
typedef boost::intrusive_ptr<CollisionObject>           collision_object_ptr;
typedef boost::intrusive_ptr<const CollisionObject>     const_collision_object_ptr;

} // namespace physics
} // namespace slon

#endif // __SLON_ENGINE_PHYSICS_COLLISION_OBJECT_H__
