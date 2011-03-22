#ifndef WATER_SIMULATION_CAMERA_ALIGNED_WATER_SURFACE_H
#define WATER_SIMULATION_CAMERA_ALIGNED_WATER_SURFACE_H

#include "../../Scene/Camera.h"
#include "../../Scene/Geode.h"
#include "../../Utility/math.hpp"
#include "../Effect.h"
#include "../Renderable.h"
#include "../Detail/AttributeTable.h"

namespace slon {
namespace graphics {

class ProjectedGrid :
    public scene::Geode,
    public Renderable
{
public:
    typedef Geode base_type;

public:
    /** Create projective grid. Setups 128x512 grid
     * @param waterEffect - effect used to display grid
     */
	ProjectedGrid(const effect_ptr& effect);

    /** Create projective grid
     * @param sizeX - number of cells in x direction
     * @param sizeY - number of cells in y direction(Usually greater than sizeX)
     * @param waterEffect - effect used to display grid
     */
	ProjectedGrid(int sizeX, int sizeY, const effect_ptr& effect);

    /** Create static grid that will be displaced by the displacement map in the shader
     * @param sizeX - number of vertices on x axis
     * @param sizeY - number of vertices on y axis
     */
    void setupGrid(int sizeX, int sizeY);

    // Override Entity
    using Entity::accept;

    void accept(scene::CullVisitor& visitor) const;

    const math::AABBf& getBounds() const { return bounds<math::AABBf>::infinite(); }

	// Override Renderable
    Effect* getEffect() const { return effect.get(); }
	void    render() const;

private:
	// water surface mesh
    effect_ptr                      effect;
    sgl::ref_ptr<sgl::VertexLayout> vertexLayout;
    sgl::ref_ptr<sgl::VertexBuffer> vertexBuffer;
    sgl::ref_ptr<sgl::IndexBuffer>  indexBuffer;

    // attribute bindings
    bool                                allowCulling;
    sgl::PRIMITIVE_TYPE                 primitiveType;
    detail::AttributeTable::binding_ptr positionBinding;

    // for effect
    mutable math::Vector3f corners[4];
};

} // namespace graphics
} // namespace slon

#endif // WATER_SIMULATION_CAMERA_ALIGNED_WATER_SURFACE_H
