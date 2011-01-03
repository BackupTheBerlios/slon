#include "stdafx.h"
#include "Engine.h"

namespace slon {
namespace realm {

World* currentWorld()
{
    return Engine::Instance()->getWorld();
}

} // namespace realm
} // namespace slon