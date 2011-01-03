#ifndef __SLON_ENGINE_DATABASE_BULLET_H__
#define __SLON_ENGINE_DATABASE_BULLET_H__

#include <string>

namespace slon {

// forward
namespace physics 
{
    class PhysicsModel;
    class DynamicsWorld;
}

namespace db {
namespace detail {

/** Load collada physics scene into dynamics world.
 * @param document - document with the collada scene.
 * @param sceneModel - model where to store physics.
 * @param dynamicsWorld - dynamics world where to create the model.
 */
void loadBulletPhysicsScene( const std::string&         fileName,
                             physics::PhysicsModel&     sceneModel,
                             physics::DynamicsWorld&    dynamicsWorld );

} // namespace detail
} // namespace db
} // namespace slon

#endif // __SLON_ENGINE_DATABASE_BULLET_H__
