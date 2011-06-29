#ifndef __SLON_ENGINE_DATABASE_BULLET_H__
#define __SLON_ENGINE_DATABASE_BULLET_H__

#include "../../Physics/PhysicsModel.h"
#include "../Loader.h"
#include "../Saver.h"

namespace slon {
namespace database {
namespace detail {

class BulletLoader :
    public database::PhysicsSceneLoader
{
public:
    // Override database::PhysicsSceneLoader
    physics::physics_model_ptr load(filesystem::File* file);
};

class BulletSaver :
    public database::PhysicsSceneSaver
{
public:
    // Override database::PhysicsSceneSaver
    void save(physics::physics_model_ptr item, filesystem::File* file);
};

} // namespace detail
} // namespace database
} // namespace slon

#endif // __SLON_ENGINE_DATABASE_BULLET_H__
