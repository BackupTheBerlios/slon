#ifndef __SLON_ENGINE_CAMERA_LOOK_AT_CAMERA__
#define __SLON_ENGINE_CAMERA_LOOK_AT_CAMERA__

#include "../Utility/cached_value.hpp"
#include "CommonCamera.h"

namespace slon {
namespace scene {

/** Coommon camera with several functions to manipulate camera position
 * in the space.
 */
class SLON_PUBLIC LookAtCamera :
    public CommonCamera
{
public:
    LookAtCamera();

    /** Set position of the camera */
    void setPosition(const math::Vector3f& _position);

    /** Set direction(z-axis) of the camera */
    void setDirection(const math::Vector3f& _direction);

    /** Set up vector(y-axis) of the camera */
    void setUp(const math::Vector3f& _up);

    /** Set projection matrix */
    void setProjectionMatrix(const math::Matrix4f& _projectionMatrix);

    /** Rotate camera around camera right vector */
    void turnPitch(float angle);

    /** Rotate camera around camera up vector */
    void turnYaw(float angle);

    /** Rotate camera around camera direction */
    void turnRoll(float angle);

    /** Rotate camera around arbitrary axis */
    void turnAroundAxis(float angle, const math::Vector3f& axis);

    /** Move camera along camera axises */
    void move(float x, float y, float z);

    /** Move camera along the direction vector */
    void moveForward(float length);

    /** Move camera along the up vector */
    void moveUp(float length);

    /** Move camera along the right vector */
    void moveRight(float length);

    // Override camera
    math::Vector3f getPosition() const { return position; }
    math::Vector3f getDirection() const { return direction; }
    math::Vector3f getUp() const { return up; }
    math::Vector3f getRight() const { return cross(up, direction); }

    const math::Frustumf& getFrustum() const;
    const math::Matrix4f& getProjectionMatrix() const { return projectionMatrix; }
    const math::Matrix4f& getViewMatrix() const;
    const math::Matrix4f& getInverseViewMatrix() const;
    const math::Matrix3f& getNormalMatrix() const;

private:
    void dirtyViewMatrix();

private:
    mutable CachedMatrix4f  viewMatrix;
    mutable CachedMatrix4f  invViewMatrix;
    mutable CachedMatrix3f  normalMatrix;
    mutable CachedFrustumf  frustum;
    math::Matrix4f          projectionMatrix;

    math::Vector3f position;
    math::Vector3f direction;
    math::Vector3f up;
};

typedef boost::intrusive_ptr<LookAtCamera>            look_at_camera_ptr;
typedef boost::intrusive_ptr<const LookAtCamera>      const_look_at_camera_ptr;

} // namespace scene
} // namespace slon

#endif // __SLON_ENGINE_CAMERA_LOOK_AT_CAMERA__
