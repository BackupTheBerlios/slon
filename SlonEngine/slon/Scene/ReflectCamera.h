#ifndef __SLON_ENGINE_CAMERA_REFLECT_CAMERA_H__
#define __SLON_ENGINE_CAMERA_REFLECT_CAMERA_H__

#include <sgl/Math/Plane.hpp>
#include "SlaveCamera.h"

namespace slon {
namespace scene {

/** Reflect camera can be attached to the camera and
 * then used to render reflections from the arbitrary plane
 * and build reflection transformation matrix.
 * If observing camera is invalid returns NAN values.
 */
class ReflectCamera :
    public SlaveCamera
{
protected:
    typedef boost::intrusive_ptr<const Camera> const_camera_ptr;

private:
    ReflectCamera(const ReflectCamera& rhs);
    ReflectCamera& operator = (const ReflectCamera& rhs);

public:
    ReflectCamera();
    ReflectCamera(const math::Planef& _reflectPlane);
    ReflectCamera( const Camera& _masterCamera,
                   const math::Planef& _reflectPlane );

    /** Set plane which reflects objects */
    void setReflectPlane(const math::Planef& _reflectPlane) { reflectPlane = _reflectPlane; }

    /** Get plane used to render reflections */
    math::Planef getReflectPlane() const { return reflectPlane; }

    /** Get transformation matrix for mapping projected reflections */
    math::Matrix4f getReflectionMatrix() const;

    // Override camera
    const math::Frustumf& getFrustum() const;
    const math::Matrix4f& getProjectionMatrix() const;
    const math::Matrix4f& getViewMatrix() const;
    const math::Matrix4f& getInverseViewMatrix() const;
    const math::Matrix3f& getNormalMatrix() const;

private:
    math::Planef   reflectPlane;

    // cached transforms
    mutable math::Frustumf frustum;
    mutable math::Matrix4f projectionMatrix;
    mutable math::Matrix4f viewMatrix;
    mutable math::Matrix4f invViewMatrix;
    mutable math::Matrix3f normalMatrix;
};

typedef boost::intrusive_ptr<ReflectCamera>            reflect_camera_ptr;
typedef boost::intrusive_ptr<const ReflectCamera>      const_reflect_camera_ptr;

} // namespace scene
} // namespace slon

#endif // __SLON_ENGINE_CAMERA_REFLECT_CAMERA_H__
