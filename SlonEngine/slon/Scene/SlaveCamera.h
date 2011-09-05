#ifndef __SLON_ENGINE_CAMER_SLAVE_CAMERA__
#define __SLON_ENGINE_CAMER_SLAVE_CAMERA__

#include "CommonCamera.h"

namespace slon {
namespace scene {

/** Slave camera get its transformation from the master camera.
 * Slave camera must have valid master camera, otherwise any get
 * function except getMasterCamera, getRenderPass will assert.
 */
class SLON_PUBLIC SlaveCamera :
    public CommonCamera
{
private:
    // noncopyable
    SlaveCamera(const SlaveCamera&);
    SlaveCamera& operator = (const SlaveCamera&);

public:
    SlaveCamera();
    SlaveCamera(const Camera& masterCamera);
    virtual ~SlaveCamera() {}

    /** Get master camera of this slave camera */
    const Camera* getMasterCamera() const { return masterCamera.get(); }

    /** Set master camera of this slave camera */
    void setMasterCamera(const Camera* _masterCamera) { masterCamera.reset(_masterCamera); }

    // Override camera
    const math::Frustumf& getFrustum() const            { assert(masterCamera); return masterCamera->getFrustum(); }
    const math::Matrix4f& getProjectionMatrix() const   { assert(masterCamera); return masterCamera->getProjectionMatrix(); }
    const math::Matrix4f& getViewMatrix() const         { assert(masterCamera); return masterCamera->getViewMatrix(); }
    const math::Matrix4f& getInverseViewMatrix() const  { assert(masterCamera); return masterCamera->getInverseViewMatrix(); }
    const math::Matrix3f& getNormalMatrix() const       { assert(masterCamera); return masterCamera->getNormalMatrix(); }

protected:
    boost::intrusive_ptr<const Camera>  masterCamera;
};

typedef boost::intrusive_ptr<SlaveCamera>            slave_camera_ptr;
typedef boost::intrusive_ptr<const SlaveCamera>      const_slave_camera_ptr;

} // namespace scene
} // namespace slon

#endif // __SLON_ENGINE_CAMER_SLAVE_CAMERA__
