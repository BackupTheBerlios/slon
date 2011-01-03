#include "stdafx.h"
#include "Scene/Camera/LookAtCamera.h"
#include <sgl/Math/MatrixFunctions.hpp>
#include <sgl/Math/Quaternion.hpp>
#include <sgl/Math/Utility.hpp>

using namespace slon;
using namespace scene;
using namespace graphics;
using namespace math;

LookAtCamera::LookAtCamera() :
    CommonCamera(),
    position(0.0f, 0.0f, 0.0f),
    direction(0.0f, 0.0f, 1.0f),
    up(0.0f, 1.0f, 0.0f)
{
}

void LookAtCamera::setProjectionMatrix(const math::Matrix4f& _projectionMatrix) 
{ 
    projectionMatrix = _projectionMatrix; 
    frustum.make_dirty();
}

void LookAtCamera::setPosition(const math::Vector3f& _position) 
{
    position = _position; 
    frustum.make_dirty();
    viewMatrix.make_dirty();
    invViewMatrix.make_dirty();
    normalMatrix.make_dirty();
}

void LookAtCamera::moveForward(float length)
{ 
    position += direction * length;
    frustum.make_dirty();
    viewMatrix.make_dirty();
    invViewMatrix.make_dirty();
    normalMatrix.make_dirty();
}

void LookAtCamera::moveUp(float length)
{ 
    position += up * length; 
    frustum.make_dirty();
    viewMatrix.make_dirty();
    invViewMatrix.make_dirty();
    normalMatrix.make_dirty();
}

void LookAtCamera::moveRight(float length) 
{ 
    position += getRight() * length;
    frustum.make_dirty();
    viewMatrix.make_dirty();
    invViewMatrix.make_dirty();
    normalMatrix.make_dirty();
}

void LookAtCamera::setDirection(const math::Vector3f& _direction)
{
    direction = normalize(_direction);
    Vector3f x = cross(up, direction);
    up = cross(direction, x);

    frustum.make_dirty();
    viewMatrix.make_dirty();
    invViewMatrix.make_dirty();
    normalMatrix.make_dirty();
}

void LookAtCamera::setUp(const math::Vector3f& _up)
{
    up = normalize(_up);
    Vector3f x = cross(up, direction);
    direction = cross(x, up);

    frustum.make_dirty();
    viewMatrix.make_dirty();
    invViewMatrix.make_dirty();
    normalMatrix.make_dirty();
}

void LookAtCamera::turnPitch(float angle)
{
    if ( fabs(angle) > math::EPSf )
    {
        Quaternionf rotation = math::from_axis_angle( angle * getRight() );
        direction = normalize( rotation * direction );
        up = normalize( rotation * up );

        viewMatrix.make_dirty();
        invViewMatrix.make_dirty();
        normalMatrix.make_dirty();
    }
}

void LookAtCamera::turnYaw(float angle)
{
    if ( fabs(angle) > math::EPSf )
    {
        Quaternionf rotation = math::from_axis_angle(angle * up);
        direction = normalize( rotation * direction );

        frustum.make_dirty();
        viewMatrix.make_dirty();
        invViewMatrix.make_dirty();
        normalMatrix.make_dirty();
    }
}

void LookAtCamera::turnRoll(float angle)
{
    if ( fabs(angle) > math::EPSf )
    {
        Quaternionf rotation = math::from_axis_angle(angle * direction);
        up = normalize( rotation * up );

        frustum.make_dirty();
        viewMatrix.make_dirty();
        invViewMatrix.make_dirty();
        normalMatrix.make_dirty();
    }
}

void LookAtCamera::turnAroundAxis(float angle, const math::Vector3f& axis)
{
    if ( fabs(angle) > math::EPSf )
    {
        Quaternionf rotation = math::from_axis_angle(angle * axis);
        direction = normalize( rotation * direction );
        up = normalize( rotation * up );

        frustum.make_dirty();
        viewMatrix.make_dirty();
        invViewMatrix.make_dirty();
        normalMatrix.make_dirty();
    }
}

const math::Frustumf& LookAtCamera::getFrustum() const
{
    if ( frustum.is_dirty() ) {
        frustum = math::make_frustum( getProjectionMatrix() * getViewMatrix() ); 
    }

    return frustum;
}

const math::Matrix4f& LookAtCamera::getViewMatrix() const
{
    if ( viewMatrix.is_dirty() )
    {
        math::Vector3f right = getRight();
        viewMatrix = math::make_matrix( right.x,     right.y,     right.z,     -dot(right, position),
                                        up.x,        up.y,        up.z,        -dot(up, position),
                                        direction.x, direction.y, direction.z, -dot(direction, position),
                                        0.0f,        0.0f,        0.0f,        1.0f );
    }

    return viewMatrix;
}

const math::Matrix4f& LookAtCamera::getInverseViewMatrix() const
{
    if ( invViewMatrix.is_dirty() )
    {
        math::Vector3f right = getRight();
        invViewMatrix = math::make_matrix( right.x,     up.x,   direction.x, position.x,
                                           right.y,     up.y,   direction.y, position.y,
                                           right.z,     up.z,   direction.z, position.z,
                                           0.0f,        0.0f,   0.0f,        1.0f );
    }

    return invViewMatrix;
}

const math::Matrix3f& LookAtCamera::getNormalMatrix() const
{
    if ( normalMatrix.is_dirty() )
    {
        math::Vector3f right = getRight();
        normalMatrix = math::make_matrix( right.x,     right.y,     right.z,
                                          up.x,        up.y,        up.z,
                                          direction.x, direction.y, direction.z );
    }

    return normalMatrix;
}
