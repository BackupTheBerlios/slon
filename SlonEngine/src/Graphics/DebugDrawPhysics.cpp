#include "stdafx.h"
#include "Graphics/DebugDrawPhysics.h"
#include "Graphics/DebugDrawCommon.h"
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

        case physics::CollisionShape::CYLINDER_X:
            mesh << static_cast<const physics::CylinderXShape&>(c);
            break;

        case physics::CollisionShape::CYLINDER_Y:
            mesh << static_cast<const physics::CylinderYShape&>(c);
            break;

        case physics::CollisionShape::CYLINDER_Z:
            mesh << static_cast<const physics::CylinderZShape&>(c);
            break;

        case physics::CollisionShape::HEIGHTFIELD:
            break;

        case physics::CollisionShape::CONVEX_MESH:
            mesh << static_cast<const physics::ConvexShape&>(c);
            break;

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
    return mesh << math::AABBf( math::Vector3f(-b.halfExtent), math::Vector3f(b.halfExtent) );
}

DebugMesh& operator << (DebugMesh& mesh, const physics::ConeShape& coneShape)
{
    return mesh << cone(coneShape.radius, coneShape.height, true);
}

DebugMesh& operator << (DebugMesh& mesh, const physics::CylinderXShape& cylShape)
{
    math::Matrix4f tr = mesh.transform;
    if (cylShape.halfExtent.x != cylShape.halfExtent.z) 
    {
        mesh << transform( tr 
                           * math::make_scaling(1.0f, 1.0f, cylShape.halfExtent.z / cylShape.halfExtent.y) 
                           * math::make_rotation_z(math::HALF_PI) );
    }
    return mesh << cylinder(cylShape.halfExtent.y, cylShape.halfExtent.x * 2.0f, true) << transform(tr);
}

DebugMesh& operator << (DebugMesh& mesh, const physics::CylinderYShape& cylShape)
{
    math::Matrix4f tr = mesh.transform;
    if (cylShape.halfExtent.x != cylShape.halfExtent.z) {
        mesh << transform( tr * math::make_scaling(1.0f, 1.0f, cylShape.halfExtent.z / cylShape.halfExtent.x) );
    }
    return mesh << cylinder(cylShape.halfExtent.x, cylShape.halfExtent.y * 2.0f, true) << transform(tr);
}

DebugMesh& operator << (DebugMesh& mesh, const physics::CylinderZShape& cylShape)
{
    math::Matrix4f tr = mesh.transform;
    if (cylShape.halfExtent.x != cylShape.halfExtent.z) 
    {
        mesh << transform( tr 
                           * math::make_scaling(1.0f, cylShape.halfExtent.y / cylShape.halfExtent.x, 1.0f) 
                           * math::make_rotation_x(math::HALF_PI) );
    }
    return mesh << cylinder(cylShape.halfExtent.x, cylShape.halfExtent.z * 2.0f, true) << transform(tr);
}

DebugMesh& operator << (DebugMesh& mesh, const physics::ConvexShape& c)
{
    unsigned offset = (unsigned)mesh.vertices.size();
    for (size_t i = 0; i<c.vertices.size(); ++i) 
    {
        mesh.vertices.push_back(c.vertices[i]);
        mesh.indices.push_back(offset + i);
    }
    mesh.pushPrimitive(sgl::POINTS, c.vertices.size());
    mesh.geometryDirty = true;

    return mesh;
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
    math::Matrix4f transformC( m.mot.getConstraint()->getRigidBodyA()->getTransform() * m.mot.getConstraint()->getDesc().frames[0] );

    switch ( m.mot.getType() ) 
    {
        case physics::Constraint::DOF_X_ROT:
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

        case physics::Constraint::DOF_Y_ROT:
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

        case physics::Constraint::DOF_Z_ROT:
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
    math::Matrix4f trA( c.cons.getRigidBodyA()->getTransform() * c.cons.getDesc().frames[0] );
    math::Matrix4f trB( c.cons.getRigidBodyB()->getTransform() * c.cons.getDesc().frames[1] );

    mesh << transform(trA)
         << color(1.0f, 0.0f, 0.0f) << line( math::Vector3f(0.0f, 0.0f, 0.0f), c.scale * math::Vector3f(1.0f, 0.0f, 0.0f) )
         << color(0.0f, 1.0f, 0.0f) << line( math::Vector3f(0.0f, 0.0f, 0.0f), c.scale * math::Vector3f(0.0f, 1.0f, 0.0f) )
         << color(0.0f, 0.0f, 1.0f) << line( math::Vector3f(0.0f, 0.0f, 0.0f), c.scale * math::Vector3f(0.0f, 0.0f, 1.0f) )
		 << transform(trB)
         << color(1.0f, 0.0f, 0.0f) << line( math::Vector3f(0.0f, 0.0f, 0.0f), c.scale * math::Vector3f(1.0f, 0.0f, 0.0f) )
         << color(0.0f, 1.0f, 0.0f) << line( math::Vector3f(0.0f, 0.0f, 0.0f), c.scale * math::Vector3f(0.0f, 1.0f, 0.0f) )
         << color(0.0f, 0.0f, 1.0f) << line( math::Vector3f(0.0f, 0.0f, 0.0f), c.scale * math::Vector3f(0.0f, 0.0f, 1.0f) );

    if ( const physics::Motor* m = c.cons.getMotor(physics::Constraint::DOF_X_ROT) ) {
        mesh << motor(*m, c.scale, c.forceScale, c.forceSectorScale);
    }
    if ( const physics::Motor* m = c.cons.getMotor(physics::Constraint::DOF_Y_ROT) ) {
        mesh << motor(*m, c.scale, c.forceScale, c.forceSectorScale);
    }
    if ( const physics::Motor* m = c.cons.getMotor(physics::Constraint::DOF_Z_ROT) ) {
        mesh << motor(*m, c.scale, c.forceScale, c.forceSectorScale);
    }

    return mesh;
}

} // namespace debug
} // namespace graphics
} // namespace slon
