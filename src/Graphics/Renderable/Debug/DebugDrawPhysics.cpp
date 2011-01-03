#include "stdafx.h"
#include "Graphics/Renderable/Debug/DebugDrawPhysics.h"
#include "Graphics/Renderable/Debug/DebugDrawCommon.h"
#include "Physics/RigidBody.h"
#include <sgl/Math/Utility.hpp>

namespace slon {
namespace graphics {
namespace debug {

DebugMesh& operator << (DebugMesh& mesh, const physics::BoxShape& b)
{
    return mesh << math::AABBf(-b.halfExtents, b.halfExtents);
}

DebugMesh& operator << (DebugMesh& mesh, const physics::ConeShape& coneShape)
{
    const size_t num_cone_vertices = 20;

    // make tip
    DebugMesh coneMesh;
    coneMesh.vertices.resize(num_cone_vertices);
    coneMesh.indices.resize(num_cone_vertices * 6);

    // make cap
    coneMesh.vertices[0] = math::Vector3f(0.0f, 0.0f, coneShape.height * 0.5f);
    for (size_t i = 1; i<num_cone_vertices; ++i)
    {
        float angle = math::PI2 * static_cast<float>(i) / (num_cone_vertices - 1);
        float x     = coneShape.radius * cos(angle);
        float y     = coneShape.radius * sin(angle);

        coneMesh.vertices[i] = math::Vector3f(x, y, -coneShape.height * 0.5f);
    }

    for (size_t i = 1; i<num_cone_vertices; ++i)
    {
        coneMesh.indices[i*6]     = 0;
        coneMesh.indices[i*6 + 1] = i;
        coneMesh.indices[i*6 + 2] = i;
        coneMesh.indices[i*6 + 3] = i+1;
        coneMesh.indices[i*6 + 4] = i+1;
        coneMesh.indices[i*6 + 5] = 0;
    }
    coneMesh.indices[num_cone_vertices * 6 - 3] = 1;
    coneMesh.indices[num_cone_vertices * 6 - 2] = 1;

    // make subset
    coneMesh.pushPrimitive(sgl::LINES, num_cone_vertices * 6);

    // dirty
    return mesh << coneMesh;
}

DebugMesh& operator << (DebugMesh& mesh, const physics::Motor& m)
{
    math::Matrix4f transformA = m.getConstraint()->getRigidBodyA()->getTransform();
    math::Matrix4f transformB = m.getConstraint()->getRigidBodyB()->getTransform();
    math::Matrix4f transformC = transformA * m.getConstraint()->getStateDesc().frames[0];

    switch ( m.getType() ) 
    {
        case physics::Motor::MOTOR_X_ROT:
        {
            math::Vector3f base = math::Vector3f(0.0f, 0.0f, 0.0f);
            math::Vector3f tip  = base + m.getConstraint()->getAxis(1);
/*
            mesh << transform(transformA)
                 << line(base, tip)
                 << transform(transformB)
                 << line(base, tip);
*/
            if ( m.getLoLimit() <= m.getHiLimit() )
            {
                mesh << transform(transformA)
                     << sector( m.getConstraint()->getAxis(1), m.getConstraint()->getAxis(0), m.getLoLimit(), m.getHiLimit() );
            }
            break;
        }

        default:
            break;
    }

    return mesh;
}

DebugMesh& operator << (DebugMesh& mesh, const physics::Constraint& c)
{
    if ( const physics::Motor* m = c.getMotor(physics::Motor::MOTOR_X_ROT) ) {
        mesh << color(1.0f, 0.0f, 0.0f) << *m;
    }
    if ( const physics::Motor* m = c.getMotor(physics::Motor::MOTOR_Y_ROT) ) {
        mesh << color(0.0f, 1.0f, 0.0f) << *m;
    }
    if ( const physics::Motor* m = c.getMotor(physics::Motor::MOTOR_Z_ROT) ) {
        mesh << color(0.0f, 0.0f, 1.0f) << *m;
    }

    return mesh;
}

} // namespace debug
} // namespace graphics
} // namespace slon
