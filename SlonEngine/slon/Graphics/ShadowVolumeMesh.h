#ifndef SLON_ENGINE_SHADOW_VOLUME_MESH_H
#define SLON_ENGINE_SHADOW_VOLUME_MESH_H

#include "../Effect/ExtrudeEffect.h"
#include "Mesh.h"
#include <sgl/State.h>
#include <map>

namespace slon {
namespace graphics {

/** Mesh with geometry preprocessed to implement
 * shadow volumes on the GPU
 */
class ShadowVolumeMesh :
	public Renderable
{
private:
    typedef boost::intrusive_ptr<ExtrudeEffect> extrude_effect_ptr;

public:
    ShadowVolumeMesh(graphics::Mesh* mesh = 0);

    /** Get effect that extrudes silhouette edges */
    extrude_effect_ptr getExtrudeEffect() const { return effect; }

    // Override renderable
    effect_ptr getAbstractEffect() const { return effect; }

	/** Create shadow volume mesh from the sgl mesh
	 * @param mesh - sgl mesh for shadow volume
	 */
    virtual void setupMesh(graphics::Mesh* mesh);

    /** Render mesh to the device */
    virtual void render() const;

    /** Check wether shadow volume mesh is constructed */
    virtual bool valid() const { return svMesh.get() != 0; }

protected:
	// render data
	mesh_ptr            svMesh;
    extrude_effect_ptr  effect;
};

} // namespace slon
} // namespace graphics

#endif // SLON_ENGINE_SHADOW_VOLUME_MESH_H
