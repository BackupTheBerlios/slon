#ifndef __SLON_ENGINE_REALM_OBJECT_H__
#define __SLON_ENGINE_REALM_OBJECT_H__

#include "../Config.h"
#include "../Physics/Forward.h"
#include "../Scene/Forward.h"
#include "../Utility/referenced.hpp"
#include "Forward.h"
#include <sgl/Math/AABB.hpp>

namespace slon {
namespace realm {
	
/** Single scene object */
class Object :
    public Referenced
{
public:
    /** Check whether object is dynamic. */
    virtual bool isDynamic() const = 0;

	/** Setup dynamic flag for the object. */
	virtual void toggleDynamic(bool toggle) = 0;

    /** Get AABB of the object. */
    virtual const math::AABBf& getBounds() const = 0;
	
    /** Get root node of the object scene graph. */
    virtual scene::Node* getRoot() = 0;

    /** Get root node of the object scene graph. */
    virtual const scene::Node* getRoot() const = 0;

	/** Set root node of the object scene graph. */
	virtual void setRoot(scene::Node* root) = 0;

    /** Traverse object scene graph */
    virtual void traverse(scene::NodeVisitor& nv) = 0;

    /** Traverse object scene graph */
    virtual void traverse(scene::ConstNodeVisitor& nv) const = 0;
	
    /** Get location where object is located */
    virtual Location* getLocation() = 0;

    /** Get location where object is located */
    virtual const Location* getLocation() const = 0;

    virtual bool isInWorld() const = 0;

#ifdef SLON_ENGINE_USE_PHYSICS
    /** Get physics model of the object. */
    virtual physics::PhysicsModel* getPhysicsModel() = 0;

    /** Get physics model of the object. */
    virtual const physics::PhysicsModel* getPhysicsModel() const = 0;

	/** Set physics model for the object. */
    virtual void setPhysicsModel(physics::PhysicsModel* physicsModel) = 0;
#endif

    virtual ~Object() {}
};

} // namespace realm
} // namespace slon

#endif // __SLON_ENGINE_REALM_OBJECT_H__
