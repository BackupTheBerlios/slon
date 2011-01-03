#include "stdafx.h"
#include "Scene/Visitors/CullVisitor.h"

using namespace slon;
using namespace scene;

void CullVisitor::visitEntity(Entity& entity)
{/*
    using namespace graphics;

    switch( entity.getEntityType() )
    {
        case Entity::GEODE:
        {
            if (renderPass) {
                static_cast<Geode&>(entity).gatherRenderables(*renderPass);
            }
            break;
        }

        case Entity::CAMERA:
        {
            break;
        }

        case Entity::LIGHT:
        {
            if (renderer) {
                renderer->handleLight( &static_cast<Light&>(entity) );
            }
            break;
        }

        default:
            break;
    }*/
}

void CullVisitor::clear()
{
    renderables.clear();
    lights.clear();
}
