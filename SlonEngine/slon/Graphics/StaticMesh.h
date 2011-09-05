#ifndef SLON_ENGINE_GRAPHICS_RENDERABLE_MESH_H
#define SLON_ENGINE_GRAPHICS_RENDERABLE_MESH_H

#include "../Scene/Entity.h"
#include "GPUSideMesh.h"

namespace slon {
namespace graphics {

/** Mesh class represents renderable 3d object */
class SLON_PUBLIC StaticMesh :
    public scene::Entity
{
public:
    typedef scene::Entity base_type;

public:
	StaticMesh();
    StaticMesh(const gpu_side_mesh_ptr& mesh);
    ~StaticMesh();

	// Override Serializable
    const char* serialize(database::OArchive& ar) const;
    void        deserialize(database::IArchive& ar);

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
	virtual const GPUSideMesh* getMesh() const { return mesh.get(); }

private:
	// geometry
	gpu_side_mesh_ptr mesh;

    // spatial
    mutable math::Matrix4f worldMatrix;

	// lighting
	bool shadowCaster;
	bool shadowReceiver;
};

} // namespace graphics
} // namespace slon

#endif // SLON_ENGINE_GRAPHICS_RENDERABLE_MESH_H
