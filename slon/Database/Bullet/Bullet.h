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
    bool binary() const { return true; }
    physics::physics_model_ptr load(std::istream& stream);
};

class BulletSaver :
    public database::PhysicsSceneSaver
{
public:
    // Override database::PhysicsSceneSaver
    bool binary() const { return true; }
    void save(physics::physics_model_ptr item, std::ostream& sink);
};

} // namespace detail
} // namespace database
} // namespace slon

#endif // __SLON_ENGINE_DATABASE_BULLET_H__
