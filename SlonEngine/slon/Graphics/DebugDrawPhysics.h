#ifndef __SLON_ENGINE_GRAPHICS_RENDERABLE_DEBUG_PHYSICS_SHAPE_DEBUG_MESH_H__
#define __SLON_ENGINE_GRAPHICS_RENDERABLE_DEBUG_PHYSICS_SHAPE_DEBUG_MESH_H__

#include "../Physics/CollisionShape.h"
#include "../Physics/Constraint.h"
#include "DebugMesh.h"

namespace slon {
namespace graphics {
namespace debug {

struct motor
{
    motor(const physics::Motor& mot_, 
          float                 scale_ = 1.0f, 
          float                 forceScale_ = 1.0f,
          float                 forceSectorScale_ = 0.8f)
    :   mot(mot_)
    ,   scale(scale_)
    ,   forceScale(forceScale_)
    ,   forceSectorScale(forceSectorScale_)
    {}

    const physics::Motor& mot;
    float                 scale;
    float                 forceScale;
    float                 forceSectorScale;
};

struct constraint
{
    constraint(const physics::Constraint& cons_, 
               float                      scale_ = 1.0f, 
               float                      forceScale_ = 1.0f,
               float                      forceSectorScale_ = 0.8f)
    :   cons(cons_)
    ,   scale(scale_)
    ,   forceScale(forceScale_)
    ,   forceSectorScale(forceSectorScale_)
    {}

    const physics::Constraint&  cons;
    float                       scale;
    float                       forceScale;
    float                       forceSectorScale;
};

/** Add collision shape to debug mesh */
DebugMesh& operator << (DebugMesh& mesh, const physics::CollisionShape& c);

/** Add box shape to debug mesh */
DebugMesh& operator << (DebugMesh& mesh, const physics::BoxShape& b);

/** Add cone shape to debug mesh */
DebugMesh& operator << (DebugMesh& mesh, const physics::ConeShape& c);

/** Add cone shape to debug mesh */
DebugMesh& operator << (DebugMesh& mesh, const physics::CylinderShape& c);

/** Add convex shape to debug mesh */
DebugMesh& operator << (DebugMesh& mesh, const physics::ConvexShape& c);

/** Add motor to debug mesh */
DebugMesh& operator << (DebugMesh& mesh, const motor& m);

/** Add constraint to debug mesh */
DebugMesh& operator << (DebugMesh& mesh, const constraint& c);

} // namespace debug
} // namespace graphics
} // namespace slon

#endif // __SLON_ENGINE_GRAPHICS_RENDERABLE_DEBUG_PHYSICS_SHAPE_DEBUG_MESH_H__
