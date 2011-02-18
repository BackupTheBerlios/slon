#ifndef __SLON_ENGINE_REALM_LOCATION_H__
#define __SLON_ENGINE_REALM_LOCATION_H__

#include "../Scene/Visitors/NodeVisitor.h"
#include "../Thread/Lock.h"
#include "Object.h"
#include <sgl/Math/Frustum.hpp>
#include <sgl/Math/Ray.hpp>

namespace slon {
namespace realm {

class Location :
    public Referenced
{
public:
    enum STATE
    {
        ACTIVE,
        AWAKEN,
        SLEEPING
    };

public:
    virtual STATE getState() const = 0;

    /** Get bound of the hole location*/
    virtual const math::AABBf& getBounds() const = 0;

    /** Update object in the spatial structure if it is presented. 
     * @return true if object updated.
     */
    virtual bool update(Object* object) = 0;

    /** Remove object from the world if it is presented. 
     * @return true if object removed
     */
    virtual bool remove(Object* object) = 0;

    /** Add object to the world. Doesn't check for duplicates. */
    virtual void add(Object* object) = 0;
		
    /** Update objects intersecting the ray.
     * @param ray - ray for visiting intersected objects.
     * @param nv - updating visitor.
     */
    virtual void visit(const math::Ray3f& ray, scene::NodeVisitor& nv) = 0;

    /** Update objects intersecting the ray.
     * @param ray - ray for visiting intersected objects.
     * @param nv - updating visitor.
     */
    virtual void visit(const math::Ray3f& ray, scene::ConstNodeVisitor& nv) const = 0;

    /** Update objects in the specified area.
     * @param area - area where to update objects.
     * @param nv - updating visitor.
     */
    //virtual void visit(const math::Spheref& area, scene::NodeVisitor& nv) const = 0;

    /** Update objects in the specified area.
     * @param area - area where to update objects.
     * @param nv - updating visitor.
     */
    virtual void visit(const math::AABBf& area, scene::NodeVisitor& nv) = 0;

    /** Update objects in the specified area.
     * @param area - area where to update objects.
     * @param nv - updating visitor.
     */
    virtual void visit(const math::AABBf& area, scene::ConstNodeVisitor& nv) const = 0;

    /** Update objects in the specified area.
     * @param area - area where to update objects.
     * @param nv - updating visitor.
     */
    virtual void visit(const math::Frustumf& frustum, scene::NodeVisitor& nv) = 0;

    /** Update objects in the specified area.
     * @param area - area where to update objects.
     * @param nv - updating visitor.
     */
    virtual void visit(const math::Frustumf& frustum, scene::ConstNodeVisitor& nv) const = 0;

    /** Grant thread read access to the location.
     * @return lock object. Lock is freed wether object is deleted.
     *
    virtual thread::lock_ptr lockForReading() const = 0;*/

    /** Grant thread write access to the location.
     * @return lock object. Lock is freed wether object is deleted.
     *
    virtual thread::lock_ptr lockForWriting() = 0;*/

    virtual ~Location() {}
};

} // namespace realm
} // namesapce slon

#endif // __SLON_ENGINE_REALM_LOCATION_H__
