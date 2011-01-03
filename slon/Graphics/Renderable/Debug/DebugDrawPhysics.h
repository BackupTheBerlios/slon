#ifndef __SLON_ENGINE_GRAPHICS_RENDERABLE_DEBUG_PHYSICS_SHAPE_DEBUG_MESH_H__
#define __SLON_ENGINE_GRAPHICS_RENDERABLE_DEBUG_PHYSICS_SHAPE_DEBUG_MESH_H__

#include "../../../Physics/CollisionShape.h"
#include "../../../Physics/Constraint.h"
#include "../DebugMesh.h"

namespace slon {
namespace graphics {
namespace debug {

/** Add box shape to debug mesh */
DebugMesh& operator << (DebugMesh& mesh, const physics::BoxShape& b);

/** Add cone shape to debug mesh */
DebugMesh& operator << (DebugMesh& mesh, const physics::ConeShape& c);

/** Add motor to debug mesh */
DebugMesh& operator << (DebugMesh& mesh, const physics::Motor& m);

/** Add constraint to debug mesh */
DebugMesh& operator << (DebugMesh& mesh, const physics::Constraint& c);

} // namespace debug
} // namespace graphics
} // namespace slon

#endif // __SLON_ENGINE_GRAPHICS_RENDERABLE_DEBUG_PHYSICS_SHAPE_DEBUG_MESH_H__
