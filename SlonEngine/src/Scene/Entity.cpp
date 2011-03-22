#include "stdafx.h"
#include "Log/LogVisitor.h"
#include "Scene/Entity.h"

using namespace slon;
using namespace scene;

void Entity::accept(log::LogVisitor& visitor) const
{
    if ( visitor.getLogger() )
    {
        visitor << "Entity";
        if ( getName() != "" ) {
            visitor << " '" << getName() << "'";
        }
        visitor << " {}\n";
    }
}