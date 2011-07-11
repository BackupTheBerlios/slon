#ifndef __SLON_ENGINE_GRAPHICS_CAMERA_ALIGNED_QUAD__
#define __SLON_ENGINE_GRAPHICS_CAMERA_ALIGNED_QUAD__

#include "Graphics/Camera.h"
#include "Graphics/Geode.h"
#include "Graphics/Renderable/MeshSubset.h"

namespace slon {
namespace graphics {

/** Quad that aligned to the camera. Can be used for rendering 
 * water or landscape surface.
 */
class CameraAlignedQuad : 
    public Geode
{
public:
    CameraAlignedQuad();
    CameraAlignedQuad(const boost::shared_ptr<Camera>& _camera);

    /** Align quad to specified camera */
    void setMasterCamera(const boost::shared_ptr<Camera>& _masterCamera) { masterCamera = _masterCamera; }

    /** Get camera that quad is aligned to */
    boost::shared_ptr<Camera> getMasterCamera() const { return masterCamera; }

    /** Attach effect to the quad */
    void setEffect(const boost::shared_ptr<Effect>& _effect) { quadRenderable.setEffect(_effect); }

    /** Get effect of the quad */
    boost::shared_ptr<Effect> getEffect() const { return quadRenderable.getEffect(); }

    /** Setup grid using exponential subdivision density: y = exp(power * v), 0.0 < v <1.0
     * @param sizeX - number of vertices on x axis.
     * @param sizeY - number of vertices on y axis.
     * @param power - exponent power.
     */
    void setupExponentialGrid( int     sizeX, 
                               int     sizeY,
                               double  power );

    // Override Geode
    void performOnRenderables(const function_on_renderable& func);

private:
    boost::shared_ptr<Camera>   masterCamera;
	MeshSubset<Effect>          quadRenderable;
};

} // namespace graphics
} // namespace slon

#endif // __SLON_ENGINE_GRAPHICS_CAMERA_ALIGNED_QUAD__
