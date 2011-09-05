#ifndef __SLON_ENGINE_SCENE_TRANSFORM_H__
#define __SLON_ENGINE_SCENE_TRANSFORM_H__

#include <sgl/Math/Matrix.hpp>
#include "Group.h"

namespace slon {
namespace scene {

/** All childs of the Transform node are in local space */
class SLON_PUBLIC Transform :
    public Group
{
friend class TransformVisitor;
public:
    Transform( const hash_string& name = hash_string() );
	    
	// Override Serializable
    const char* serialize(database::OArchive& ar) const;
    void        deserialize(database::IArchive& ar);

    // Override Node
    TYPE getNodeType() const { return TRANSFORM; }
    void accept(log::LogVisitor& visitor) const;

    /** Get node transformation matrix
     * @return node transformation matrix
     */
    virtual const math::Matrix4f& getTransform() const = 0;

    /** Get node inverse transformation matrix
     * @return matrix that transforms node from world space to local space
     */
    virtual const math::Matrix4f& getInverseTransform() const = 0;

    /** Absolute transform know their localToWorld and worldToLocal transforms and are not affected by parent transformation nodes. */
    virtual bool isAbsolute() const = 0;

    /** Get last modification time stamp. */
    virtual unsigned int getModifiedCount() const { return modifiedCount; }

    /** Get last traverse time stamp. */
    virtual unsigned int getTransformTraverseStamp() const { return traverseStamp; }

    /** Get world to local space transformation matrix
     * @return cached transform from last traverse.
     *  If no traverse were called result is undefined
     */
    virtual const math::Matrix4f& getWorldToLocal() const { return worldToLocal; }

    /** Get local to world space transformation matrix
     * @return cached transform from last traverse
     *  If no traverse were called result is undefined
     */
    virtual const math::Matrix4f& getLocalToWorld() const { return localToWorld; }

    virtual ~Transform() {}

protected:
	/** Mark for traverse by transform visitor. */
	void update();

public:    
    // cached transforms
    math::Matrix4f  worldToLocal;
    math::Matrix4f  localToWorld;
    unsigned int    traverseStamp;
	unsigned int	modifiedCount;
};

} // namespace scene
} // namespace slon

#endif // __SLON_ENGINE_SCENE_TRANSFORM_H__
