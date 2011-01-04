#include "stdafx.h"
#include "Graphics/Renderable/Debug/DebugDrawPhysics.h"
#include "Graphics/Renderable/Debug/DebugDrawCommon.h"
#include "Physics/RigidBody.h"
#include "Physics/ServoMotor.h"
#include "Physics/SpringMotor.h"
#include "Physics/VelocityMotor.h"
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

float getArcEnd(const physics::Motor& motor, float scale)
{
    float end = 0.0f;

    if ( const physics::ServoMotor* m = dynamic_cast<const physics::ServoMotor*>(&motor) )
    {
        if ( m->enabled() ) {
            end = std::min( std::max( m->getPosition() + m->getTargetForce() * scale, m->getLoLimit() ), m->getHiLimit() );
        }
    }
    else if ( const physics::VelocityMotor* m = dynamic_cast<const physics::VelocityMotor*>(&motor) )
    {
        if ( m->enabled() ) {
            end = std::min( std::max( m->getPosition() + m->getTargetVelocity() * scale, m->getLoLimit() ), m->getHiLimit() );
        }
    }
    else if ( const physics::SpringMotor* m = dynamic_cast<const physics::SpringMotor*>(&motor) )
    {
        if ( m->enabled() ) {
            end = m->getEquilibriumPoint();
        }
    }

    return end;
}

DebugMesh& operator << (DebugMesh& mesh, const motor& m)
{
    math::Matrix4f transformA = m.mot.getConstraint()->getRigidBodyA()->getTransform();
    math::Matrix4f transformB = m.mot.getConstraint()->getRigidBodyB()->getTransform();
    math::Matrix4f transformC = transformA * m.mot.getConstraint()->getStateDesc().frames[0];

    switch ( m.mot.getType() ) 
    {
        case physics::Motor::MOTOR_X_ROT:
            if ( m.mot.getLoLimit() <= m.mot.getHiLimit() )
            {
                mesh << color(1.0f, 0.0f, 0.0f)
                     << transform(transformC)
                     << sector( m.scale * math::Vector3f(transformB[0][1], transformB[1][1], transformB[2][1]), 
                                math::Vector3f(transformA[0][0], transformA[1][0], transformA[2][0]), 
                                m.mot.getLoLimit(), 
                                m.mot.getHiLimit() )
                     << sector( m.forceSectorScale * m.scale * math::Vector3f(transformB[0][1], transformB[1][1], transformB[2][1]), 
                                math::Vector3f(transformA[0][0], transformA[1][0], transformA[2][0]), 
                                m.mot.getPosition(), 
                                getArcEnd(m.mot, m.forceScale),
                                true );
            }
            break;

        case physics::Motor::MOTOR_Y_ROT:
            if ( m.mot.getLoLimit() <= m.mot.getHiLimit() )
            {
                mesh << color(0.0f, 1.0f, 0.0f)
                     << transform(transformC)
                     << sector( m.scale * math::Vector3f(transformB[0][0], transformB[1][0], transformB[2][0]), 
                                math::Vector3f(transformB[0][1], transformB[1][1], transformB[2][1]), 
                                m.mot.getLoLimit(), 
                                m.mot.getHiLimit() )
                     << sector( m.forceSectorScale * m.scale * math::Vector3f(transformB[0][0], transformB[1][0], transformB[2][0]), 
                                math::Vector3f(transformB[0][1], transformB[1][1], transformB[2][1]), 
                                m.mot.getPosition(), 
                                getArcEnd(m.mot, m.forceScale),
                                true );
            }
            break;

        case physics::Motor::MOTOR_Z_ROT:
            if ( m.mot.getLoLimit() <= m.mot.getHiLimit() )
            {
                mesh << color(0.0f, 0.0f, 1.0f)
                     << transform(transformC)
                     << sector( m.scale * math::Vector3f(transformB[0][0], transformB[1][0], transformB[2][0]), 
                                math::Vector3f(transformB[0][2], transformB[1][2], transformB[2][2]), 
                                m.mot.getLoLimit(), 
                                m.mot.getHiLimit() )
                     << sector( m.forceSectorScale * m.scale * math::Vector3f(transformB[0][0], transformB[1][0], transformB[2][0]), 
                                math::Vector3f(transformB[0][2], transformB[1][2], transformB[2][2]), 
                                m.mot.getPosition(), 
                                getArcEnd(m.mot, m.forceScale),
                                true );
            }
            break;

        default:
            break;
    }

    return mesh;
}

DebugMesh& operator << (DebugMesh& mesh, const constraint& c)
{
    math::Matrix4f trA = c.cons.getRigidBodyA()->getTransform();
    math::Matrix4f trB = c.cons.getRigidBodyB()->getTransform();
    math::Matrix4f trC = trA * c.cons.getStateDesc().frames[0];
    mesh << transform(trC)
         << color(1.0f, 0.0f, 0.0f) << line( math::Vector3f(0.0f, 0.0f, 0.0f), c.scale * math::Vector3f(trB[0][0], trB[1][0], trB[2][0]) )
         << color(0.0f, 1.0f, 0.0f) << line( math::Vector3f(0.0f, 0.0f, 0.0f), c.scale * math::Vector3f(trB[0][1], trB[1][1], trB[2][1]) )
         << color(0.0f, 0.0f, 1.0f) << line( math::Vector3f(0.0f, 0.0f, 0.0f), c.scale * math::Vector3f(trB[0][2], trB[1][2], trB[2][2]) )
         << color(1.0f, 0.0f, 0.0f) << line( math::Vector3f(0.0f, 0.0f, 0.0f), c.scale * math::Vector3f(trA[0][0], trA[1][0], trA[2][0]) )
         << color(0.0f, 1.0f, 0.0f) << line( math::Vector3f(0.0f, 0.0f, 0.0f), c.scale * math::Vector3f(trA[0][1], trA[1][1], trA[2][1]) )
         << color(0.0f, 0.0f, 1.0f) << line( math::Vector3f(0.0f, 0.0f, 0.0f), c.scale * math::Vector3f(trA[0][2], trA[1][2], trA[2][2]) );

    if ( const physics::Motor* m = c.cons.getMotor(physics::Motor::MOTOR_X_ROT) ) {
        mesh << motor(*m, c.scale, c.forceScale, c.forceSectorScale);
    }
    if ( const physics::Motor* m = c.cons.getMotor(physics::Motor::MOTOR_Y_ROT) ) {
        mesh << motor(*m, c.scale, c.forceScale, c.forceSectorScale);
    }
    if ( const physics::Motor* m = c.cons.getMotor(physics::Motor::MOTOR_Z_ROT) ) {
        mesh << motor(*m, c.scale, c.forceScale, c.forceSectorScale);
    }

    return mesh;
}

} // namespace debug
} // namespace graphics
} // namespace slon
