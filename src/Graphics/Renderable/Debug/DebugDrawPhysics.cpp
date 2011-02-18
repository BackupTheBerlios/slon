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

DebugMesh& operator << (DebugMesh& mesh, const physics::CollisionShape& c)
{
    switch ( c.getShapeType() )
    {
        case physics::CollisionShape::PLANE:
        case physics::CollisionShape::SPHERE:
        case physics::CollisionShape::BOX:
            mesh << static_cast<const physics::BoxShape&>(c);
            break;

        case physics::CollisionShape::CONE:
            mesh << static_cast<const physics::ConeShape&>(c);
            break;

        case physics::CollisionShape::CAPSULE:
            break;

        case physics::CollisionShape::CYLINDER:
            mesh << static_cast<const physics::CylinderShape&>(c);
            break;

        case physics::CollisionShape::HEIGHTFIELD:
        case physics::CollisionShape::CONVEX_MESH:
        case physics::CollisionShape::TRIANGLE_MESH:
            break;

        case physics::CollisionShape::COMPOUND:
        {
            const physics::CompoundShape& cShape = static_cast<const physics::CompoundShape&>(c);
            
            math::Matrix4f baseTransform = mesh.transform;
            for (size_t i = 0; i<cShape.shapes.size(); ++i) {
                mesh << transform(baseTransform * cShape.shapes[i].transform) << *cShape.shapes[i].shape;
            }
            mesh << transform(baseTransform);

            break;
        }

        default:
            assert(!"can't get here");
    }

    return mesh;
}

DebugMesh& operator << (DebugMesh& mesh, const physics::BoxShape& b)
{
    return mesh << math::AABBf( math::Vector3f(-b.halfExtents), math::Vector3f(b.halfExtents) );
}

DebugMesh& operator << (DebugMesh& mesh, const physics::ConeShape& coneShape)
{
    return mesh << cone(coneShape.radius, coneShape.height, true);
}

DebugMesh& operator << (DebugMesh& mesh, const physics::CylinderShape& cylShape)
{
    math::Matrix4f tr = mesh.transform;
    if (cylShape.halfExtent.x != cylShape.halfExtent.z) {
        mesh << transform( tr * math::make_scaling(1.0f, 1.0f, cylShape.halfExtent.z / cylShape.halfExtent.x) );
    }
    return mesh << cylinder(cylShape.halfExtent.x, cylShape.halfExtent.y * 2.0f, true) << transform(tr);
}

float getArcEnd(const physics::Motor& motor, float scale)
{
    float end = motor.getPosition();

    if ( const physics::ServoMotor* m = dynamic_cast<const physics::ServoMotor*>(&motor) )
    {
        if ( m->enabled() ) {
            end = (float)std::min( std::max( m->getPosition() + m->getTargetForce() * scale, m->getLoLimit() ), m->getHiLimit() );
        }
    }
    else if ( const physics::VelocityMotor* m = dynamic_cast<const physics::VelocityMotor*>(&motor) )
    {
        if ( m->enabled() ) {
            end = (float)std::min( std::max( m->getPosition() + m->getTargetVelocity() * scale, m->getLoLimit() ), m->getHiLimit() );
        }
    }
    else if ( const physics::SpringMotor* m = dynamic_cast<const physics::SpringMotor*>(&motor) )
    {
        if ( m->enabled() ) {
            end = (float)m->getEquilibriumPoint();
        }
    }

    return end;
}

DebugMesh& operator << (DebugMesh& mesh, const motor& m)
{
    math::Matrix4f transformA( m.mot.getConstraint()->getRigidBodyA()->getTransform() );
    math::Matrix4f transformB( m.mot.getConstraint()->getRigidBodyB()->getTransform() );
    math::Matrix4f transformC( m.mot.getConstraint()->getRigidBodyA()->getTransform() * m.mot.getConstraint()->getStateDesc().frames[0] );

    switch ( m.mot.getType() ) 
    {
        case physics::Motor::MOTOR_X_ROT:
            if ( m.mot.getLoLimit() <= m.mot.getHiLimit() )
            {
                mesh << color(1.0f, 0.0f, 0.0f)
                     << transform(transformC)
                     << sector( m.scale * math::Vector3f(transformB[0][1], transformB[1][1], transformB[2][1]), 
                                math::Vector3f(transformA[0][0], transformA[1][0], transformA[2][0]), 
                                (float)m.mot.getLoLimit(), 
                                (float)m.mot.getHiLimit() )
                     << sector( m.forceSectorScale * m.scale * math::Vector3f(transformB[0][1], transformB[1][1], transformB[2][1]), 
                                math::Vector3f(transformA[0][0], transformA[1][0], transformA[2][0]), 
                                (float)m.mot.getPosition(), 
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
                                (float)m.mot.getLoLimit(), 
                                (float)m.mot.getHiLimit() )
                     << sector( m.forceSectorScale * m.scale * math::Vector3f(transformB[0][0], transformB[1][0], transformB[2][0]), 
                                math::Vector3f(transformB[0][1], transformB[1][1], transformB[2][1]), 
                                (float)m.mot.getPosition(), 
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
                                (float)m.mot.getLoLimit(), 
                                (float)m.mot.getHiLimit() )
                     << sector( m.forceSectorScale * m.scale * math::Vector3f(transformB[0][0], transformB[1][0], transformB[2][0]), 
                                math::Vector3f(transformB[0][2], transformB[1][2], transformB[2][2]), 
                                (float)m.mot.getPosition(), 
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
    math::Matrix4f trA( c.cons.getRigidBodyA()->getTransform() );
    math::Matrix4f trB( c.cons.getRigidBodyB()->getTransform() );
    math::Matrix4f trC( c.cons.getRigidBodyA()->getTransform() * c.cons.getStateDesc().frames[0] );
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
