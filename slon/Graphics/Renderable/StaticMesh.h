#ifndef SLON_ENGINE_GRAPHICS_RENDERABLE_MESH_H
#define SLON_ENGINE_GRAPHICS_RENDERABLE_MESH_H

#include "../../Scene/Geode.h"
#include "Mesh.h"

namespace slon {
namespace graphics {

/** Mesh class represents renderable 3d object */
class StaticMesh :
    public scene::Geode
{
public:
    typedef scene::Geode base_type;

public:
    StaticMesh(Mesh* mesh);

    // Override Entity
    using Entity::accept;

    void accept(scene::CullVisitor& visitor) const;
    void accept(scene::TransformVisitor& visitor);

    const math::AABBf& getBounds() const { return mesh->getBounds(); }

	/** Check whenever mesh is shadow caster */
	virtual bool IsShadowCaster() const         { return shadowCaster; }

	/** Setup mesh to be shadow caster or not */
	virtual void SetShadowCaster(bool toggle)   { shadowCaster = toggle; }

	/** Check whenever mesh is shadow receiver */
	virtual bool IsShadowReceiver() const       { return shadowReceiver; }

	/** Setup mesh to be shadow receiver or not */
	virtual void SetShadowReceiver(bool toggle) { shadowReceiver = toggle; }

	/** Get geometry of the mesh */
	virtual const Mesh* getMesh() const { return mesh.get(); }

private:
	// geometry
	mesh_ptr mesh;

    // spatial
    mutable math::Matrix4f worldMatrix;

	// lighting
	bool shadowCaster;
	bool shadowReceiver;
};

} // namespace graphics
} // namespace slon

#endif // SLON_ENGINE_GRAPHICS_RENDERABLE_MESH_H
