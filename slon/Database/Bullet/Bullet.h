#ifndef __SLON_ENGINE_DATABASE_BULLET_H__
#define __SLON_ENGINE_DATABASE_BULLET_H__

#include "../../Physics/PhysicsModel.h"
#include "../Loader.h"

namespace slon {
namespace database {
namespace detail {

class BulletLoader :
    public database::PhysicsSceneLoader
{
public:
    bool binary() const 
    { 
        return true; 
    }

    /** Load '*.bullet' scene */
    physics::physics_model_ptr load(std::istream& stream);
};
/*
class BulletSaver :
    public database::PhysicsSceneSaver
{
public:
    bool binary() const 
    { 
        return true; 
    }

    /** Saver '*.bullet' scene 
    void save(physics::physics_model_ptr item, std::ostream& sink);
};
*/
} // namespace detail
} // namespace database
} // namespace slon

#endif // __SLON_ENGINE_DATABASE_BULLET_H__
