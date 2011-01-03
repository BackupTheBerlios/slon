#ifndef __SLON_ENGINE_CAMERA_THREAD_SAFE_CAMERA__
#define __SLON_ENGINE_CAMERA_THREAD_SAFE_CAMERA__

namespace slon {
namespace graphics {

class ThreadSafeCamera :
    public CommonCamera
{
public:
    LookAtCamera();

    /** Set position of the camera */
    void setPosition(const math::Vector3f& _position) { position = _position; }

    /** Set direction(z-axis) of the camera */
    void setDirection(const math::Vector3f& _direction);

    /** Set up vector(y-axis) of the camera */
    void setUp(const math::Vector3f& _up);

    /** Set projection matrix */
    void setProjectionMatrix(const math::Matrix4f& _projectionMatrix) { projectionMatrix = _projectionMatrix; }

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
    void moveForward(float length) { position += math::Vector4f(direction * length); }

    /** Move camera along the up vector */
    void moveUp(float length) { position += math::Vector4f(up * length); }

    /** Move camera along the right vector */
    void moveRight(float length) { position += math::Vector4f(getRight() * length); }

    // Override camera
    math::Vector4f getPosition() const { return position; }
    math::Vector3f getDirection() const { return direction; }
    math::Vector3f getUp() const { return up; }
    math::Vector3f getRight() const { return cross(up, direction); }

    math::Matrix3f getNormalMatrix() const;
    math::Matrix4f getProjectionMatrix() const;
    math::Matrix4f getViewMatrix() const;
    math::Matrix4f getInverseViewMatrix() const;

private:
    math::Matrix4f projectionMatrix;
    math::Vector4f position;
    math::Vector3f direction;
    math::Vector3f up;
};

} // namespace graphics
} // namespace slon

#endif // __SLON_ENGINE_CAMERA_THREAD_SAFE_CAMERA__
