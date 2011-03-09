#ifndef __SLON_ENGINE_REALM_LOCATION_H__
#define __SLON_ENGINE_REALM_LOCATION_H__

#include "../Scene/Forward.h"
#include "../Thread/Lock.h"
#include "../Utility/callback.hpp"
#include "../Utility/math.hpp"
#include "../Utility/referenced.hpp"
#include "Forward.h"

namespace slon {
namespace realm {
	
class Location :
    public Referenced
{
public:
    /** Get bounds of the hole location. */
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
		
    /** Visit objects intersecting body.
     * @param body - body which intersects objects.
     * @param cb - visitor.
     */
    virtual void visit(const body_variant& body, object_callback& cb) = 0;

    /** Visit objects intersecting body.
     * @param body - body which intersects objects.
     * @param cb - visitor.
     */
    virtual void visit(const body_variant& body, object_const_callback& cb) const = 0;
	
    /** Visit objects visible in frustum.
     * @param frustum - frustum which intersects objects.
     * @param cb - visitor.
     */
    virtual void visitVisible(const math::Frustumf& frustum, object_callback& cb) = 0;

    /** Visit objects visible in frustum.
     * @param frustum - frustum which intersects objects.
     * @param cb - visitor.
     */
    virtual void visitVisible(const math::Frustumf& frustum, object_const_callback& cb) const = 0;

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
