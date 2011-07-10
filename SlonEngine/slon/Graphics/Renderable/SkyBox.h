#ifndef SLON_ENGINE_RENDERABLE_SKY_BOX_H
#define SLON_ENGINE_RENDERABLE_SKY_BOX_H

#include <sgl/TextureCube.h>
#include "../../Scene/Geode.h"
#include "../../Scene/Visitor/CullVisitor.h"
#include "../../Utility/math.hpp"
#include "../Detail/AttributeTable.h"
#include "../Effect/SkyBoxEffect.h"
#include "../Renderable.h"

namespace slon {
namespace graphics {

class SkyBox :
    public scene::Geode,
    public Renderable
{
public:
    typedef boost::intrusive_ptr<SkyBoxEffect>  sky_box_effect_ptr;

public:
    SkyBox();

	/** Make sky box from 6 side maps */
	void MakeFromSideTextures(const std::string maps[6]);

    /** Get sky box cube map */
    sgl::TextureCube* getCubeMap() const { return cubemap.get(); }

    // Override Entity
    using Entity::accept;

    void accept(scene::CullVisitor& visitor) const;

    // Override Entity
    const math::AABBf& getBounds() const { return bounds<math::AABBf>::infinite(); }

	// Override Renderable
    Effect* getEffect() const { return effect.get(); }
	void    render() const;

private:
    // non copyable
    SkyBox(const SkyBox&);
    SkyBox& operator = (const SkyBox&);

private:
    // effect
    sky_box_effect_ptr                  effect;

	// geometry
    sgl::ref_ptr<sgl::TextureCube>      cubemap;
    sgl::ref_ptr<sgl::VertexBuffer>     vbo;
    sgl::ref_ptr<sgl::VertexLayout>     vertexLayout;
    sgl::ref_ptr<sgl::IndexBuffer>      ibo;

    // atribute bindings
    detail::AttributeTable::binding_ptr positionBinding;
};

} // namespace graphics
} // namespace slon

#endif // SLON_ENGINE_RENDERABLE_SKY_BOX_H
