#ifndef __SLON_ENGINE_GRAPHICS_RENDERABLE_SKINNED_MESH_H__
#define __SLON_ENGINE_GRAPHICS_RENDERABLE_SKINNED_MESH_H__

#include "../../Scene/Geode.h"
#include "Mesh.h"
#include <sgl/Math/Containers.hpp>

namespace slon {

// forward
namespace scene
{
    class Joint;
    class Skeleton;

    typedef boost::intrusive_ptr<Joint>       joint_ptr;
    typedef boost::intrusive_ptr<const Joint> const_joint_ptr;

    typedef boost::intrusive_ptr<Skeleton>       skeleton_ptr;
    typedef boost::intrusive_ptr<const Skeleton> const_skeleton_ptr;
}

namespace graphics {

/** Mesh class represents renderable 3d object. Skinned mesh passes
 * all visitors, except CullVisitor to the its skeleton.
 */
class SkinnedMesh :
    public scene::Geode
{
public:
    typedef scene::Geode base_type;

    typedef std::vector<unsigned>   uint_vector;
    typedef std::vector<int>        int_vector;
    typedef std::vector<float>      float_vector;

    typedef std::vector<scene::const_joint_ptr> joint_vector;
    typedef joint_vector::iterator              joint_iterator;
    typedef joint_vector::const_iterator        joint_const_iterator;

public:
    struct DESC
    {
        graphics::Mesh* mesh;
        const unsigned* jointCountArray;
        const int*      jointArray;
        const float*    weightArray;

		DESC()
		:	mesh(0)
		,	jointCountArray(0)
		,	jointArray(0)
		,	weightArray(0)
		{}
    };

public:
    SkinnedMesh(const DESC& desc);

    // Override Entity
    const math::AABBf& getBounds() const;

    // Override node
    using Node::accept;

    void accept(scene::NodeVisitor& visitor);
    void accept(scene::TraverseVisitor& visitor);
    void accept(scene::UpdateVisitor& visitor);
    void accept(scene::CullVisitor& visitor);

    /** Setup skeleton for the skinned mesh to link vertices with joints.
     * Skeleton must already have joint hierarchy.
     */
    virtual void setSkeleton(scene::Skeleton* skeleton);

    /** Get skeleton affecting mesh */
    virtual scene::Skeleton* getSkeleton()      { return skeleton.get(); }

	/** Check whenever mesh is shadow caster */
	virtual bool IsShadowCaster() const         { return shadowCaster; }

	/** Setup mesh to be shadow caster or not */
	virtual void SetShadowCaster(bool toggle)   { shadowCaster = toggle; }

	/** Check whenever mesh is shadow receiver */
	virtual bool IsShadowReceiver() const       { return shadowReceiver; }

	/** Setup mesh to be shadow receiver or not */
	virtual void SetShadowReceiver(bool toggle) { shadowReceiver = toggle; }

	/** Get mesh holding geometry */
	const Mesh* getMesh() const { return mesh.get(); }

private:
	// geometry
	mesh_ptr            mesh;
    scene::skeleton_ptr skeleton;
    joint_vector        joints;
	bool				cpuSkinning;

    // helpers for animation
    Mesh::attribute_const_iterator    positionIter;
    Mesh::attribute_const_iterator    normalIter;
    Mesh::attribute_const_iterator    tangentIter;
    Mesh::attribute_const_iterator    boneIndexIter;
    Mesh::attribute_const_iterator    boneWeightIter;

    math::vector_of_matrix4f    boneMatrices;
    math::vector_of_vector4f    boneRotations;
    math::vector_of_vector3f    boneTranslations;

    math::vector_of_vector4f    positions;
    math::vector_of_vector3f    normals;
    math::vector_of_vector3f    tangents;
    uint_vector                 weightCount;
    int_vector                  bones;
    float_vector                weights;
    bool                        haveBindShapeBone;

    // spatial
	mutable math::AABBf			bounds;
    mutable math::Matrix4f      worldMatrix;
    mutable math::Matrix4f      invWorldMatrix;

	// lighting
	bool shadowCaster;
	bool shadowReceiver;
};

} // namespace graphics
} // namespace slon

#endif // __SLON_ENGINE_GRAPHICS_RENDERABLE_SKINNED_MESH_H__