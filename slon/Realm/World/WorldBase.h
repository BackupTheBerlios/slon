#ifndef __SLON_ENGINE_REALM_WORLD_WORLD_BASE_H__
#define __SLON_ENGINE_REALM_WORLD_WORLD_BASE_H__

#include "../World.h"
#define NOMINMAX // thread may include windows.h
#include <boost/thread/shared_mutex.hpp>

namespace slon {
namespace realm {

/** Shared functional for world classes.
 */
class WorldBase :
    public World
{
public:
    // Override World
    thread::lock_ptr lockForReading() const;
    thread::lock_ptr lockForWriting();

protected:
    virtual ~WorldBase() {}

private:
    mutable boost::shared_mutex accessMutex;
};

} // namespace realm
} // namespace slon

#endif // __SLON_ENGINE_REALM_WORLD_WORLD_BASE_H__