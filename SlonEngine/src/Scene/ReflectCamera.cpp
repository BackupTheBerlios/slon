#include "stdafx.h"
#include "Scene/ReflectCamera.h"
#include <sgl/Math/Matrix.hpp>

using namespace slon;
using namespace scene;
using namespace graphics;
using namespace math;

namespace {

    inline float sgn(float a)
    {
        if (a > 0.0f) return 1.0f;
        if (a < 0.0f) return -1.0f;
        return 0.0f;
    }

} // anonymous namespace

ReflectCamera::ReflectCamera()
{
}

ReflectCamera::ReflectCamera(const math::Planef& _reflectPlane) :
    reflectPlane(_reflectPlane)
{
}

ReflectCamera::ReflectCamera( const Camera& _masterCamera,
                              const math::Planef& _reflectPlane ) :
    SlaveCamera(_masterCamera),
    reflectPlane(_reflectPlane)
{
}

const Matrix4f& ReflectCamera::getProjectionMatrix() const
{
    assert(masterCamera);

    // get oblique near clip plane clipping matrix
    // From Eric's Lengyel code http://www.terathon.com/code/oblique.html
    projectionMatrix = masterCamera->getProjectionMatrix();
    {
        // transform clip plane into view space
        math::Planef clipPlane = getViewMatrix() * reflectPlane;
       
        // Calculate the clip-space corner point opposite the clipping plane
        // as (sgn(clipPlane.x), sgn(clipPlane.y), 1, 1) and
        // transform it into camera space by multiplying it
        // by the inverse of the projection matrix
        math::Vector4f q;
        q.x = (sgn(clipPlane.normal.x) + projectionMatrix[0][2]) / projectionMatrix[0][0];
        q.y = (sgn(clipPlane.normal.y) + projectionMatrix[1][2]) / projectionMatrix[1][1];
        q.z = -1.0f;
        q.w = (1.0f + projectionMatrix[2][2]) / projectionMatrix[2][3];
        
        // Calculate the scaled plane vector
        Vector4f c = clipPlane.as_vec() * ( 2.0f / dot(clipPlane.as_vec(), q) );
        
        // Replace the third row of the projection matrix
        projectionMatrix[2][0] = c.x;
        projectionMatrix[2][1] = c.y;
        projectionMatrix[2][2] = c.z - 1.0f;
        projectionMatrix[2][3] = c.w;
    }

    return projectionMatrix;
}

const math::Frustumf& ReflectCamera::getFrustum() const
{
    frustum = math::make_frustum( masterCamera->getProjectionMatrix() * getViewMatrix() ); 
    return frustum;
}

const Matrix4f& ReflectCamera::getViewMatrix() const
{
    math::Matrix4f camViewMatrix = masterCamera->getViewMatrix();
    math::Vector3f camPosition   = math::get_translation( masterCamera->getInverseViewMatrix() );

    Vector3f position    = reflectPlane.reflect(camPosition);
    Vector3f direction   = reflectPlane.reflect( math::Vector3f(camViewMatrix[2][0], camViewMatrix[2][1], camViewMatrix[2][2]) );
    Vector3f up          = reflectPlane.reflect( math::Vector3f(camViewMatrix[1][0], camViewMatrix[1][1], camViewMatrix[1][2]) );
    Vector3f right       = reflectPlane.reflect( math::Vector3f(camViewMatrix[0][0], camViewMatrix[0][1], camViewMatrix[0][2]) );

    return viewMatrix = math::make_matrix( right.x,     right.y,     right.z,     -dot(right, position),
                                           up.x,        up.y,        up.z,        -dot(up, position),
                                           direction.x, direction.y, direction.z, -dot(direction, position),
                                           0.0f,        0.0f,        0.0f,        1.0f );
}

const Matrix4f& ReflectCamera::getInverseViewMatrix() const
{
    assert(masterCamera);
    return invViewMatrix = invert( getViewMatrix() );
}

const Matrix3f& ReflectCamera::getNormalMatrix() const
{
    assert(masterCamera);
    viewMatrix = getViewMatrix();
    return normalMatrix = math::make_matrix( viewMatrix[0][0], viewMatrix[0][1], viewMatrix[0][2],
                                             viewMatrix[1][0], viewMatrix[1][1], viewMatrix[1][2],
                                             viewMatrix[2][0], viewMatrix[2][1], viewMatrix[2][2] );
}

Matrix4f ReflectCamera::getReflectionMatrix() const
{
    assert(masterCamera);
    /*
    return observingCamera->getViewMatrix()
           * observingCamera->getProjectionMatrix()
           * Mat4f::scale(0.5f, 0.5f, 1.0f)
           * Mat4f::translate(0.5f, 0.5f, 0.0f);
    */
    return math::Matrix4f::translation(0.5f, 0.5f, 0.0f)
           * math::Matrix4f::scaling(0.5f, 0.5f, 1.0f)
           * masterCamera->getProjectionMatrix()
           * masterCamera->getViewMatrix();
}
