#include "stdafx.h"
#include "Graphics/Renderable/SkinnedMesh.h"
#include "Scene/Visitors/DFSNodeVisitor.h"
#include "Scene/Visitors/CullVisitor.h"
#include "Scene/Visitors/FilterVisitor.h"
#include "Scene/Visitors/TransformVisitor.h"
#include "Scene/Skeleton.h"
#include "Utility/error.hpp"
#include "Utility/math.hpp"

__DEFINE_LOGGER__("graphics.SkinnedMesh")

namespace {

    using namespace slon;
    using namespace graphics;

    struct gather_joints_visitor :
        public scene::FilterVisitor<scene::DFSNodeVisitor, scene::Joint>
    {
        gather_joints_visitor(SkinnedMesh&                  skinnedMesh_,
                              SkinnedMesh::joint_vector&    joints_)
        :	skinnedMesh(skinnedMesh_)
        ,	joints(joints_)
        {
			extendable = joints.empty();
		}

        void visit(scene::Joint& joint)
        {
            unsigned id = joint.getId();

            if ( id >= joints.size() ) 
            {
				if (extendable) 
				{
					joints.resize(id + 1);
					joints[id].reset(&joint);
				}
				else
				{
					logger << log::S_ERROR << "Error gathering joints for skinned mesh '" << skinnedMesh.getName() 
						                    << "'. Joint '" << joint.getName() << "' has invalid id: " << id << std::endl;
				}
            }
            else if ( joints[id] ) 
            {
                logger << log::S_ERROR << "Error gathering joints for skinned mesh '" << skinnedMesh.getName() 
                                        << "'. Joint '" << joint.getName() << "' overrides another joint '" << joints[id]->getName() << "'\n";
            }
            else 
            {
                joints[id].reset(&joint);
            }
        }

        SkinnedMesh&                skinnedMesh;
        SkinnedMesh::joint_vector&  joints;
		bool						extendable;
    };

    template<typename OutIterator>
    void transform_attributes4f( const math::vector_of_vector4f&  attributes,
                                 const std::vector<unsigned>&     wcount,
                                 const std::vector<float>&        weights,
                                 const std::vector<int>&          bones,
                                 const math::vector_of_matrix4f&  matrices,
                                 OutIterator                      out )
    {
        size_t i,j,k;
        for (i = 0, k = 0; i<wcount.size(); ++i, ++out)
        {
            if (wcount[i] > 0)
            {
                (*out) += weights[k] * (matrices[ bones[k] ] * attributes[i]); ++k;
                for (j = 1; j<wcount[i]; ++j, ++k) {
                    (*out) += weights[k] * (matrices[ bones[k] ] * attributes[i]);
                }
            }
            else {
                (*out) = attributes[i];
            }
        }
    }

    template<typename OutIterator>
    void transform_attributes3f( const math::vector_of_vector4f&  attributes,
                                 const std::vector<unsigned>&     wcount,
                                 const std::vector<float>&        weights,
                                 const std::vector<int>&            bones,
                                 const math::vector_of_matrix4f&  matrices,
                                 OutIterator                      out )
    {
        size_t i,j,k;
        for (i = 0, k = 0; i<wcount.size(); ++i, ++out)
        {
            if (wcount[i] > 0)
            {
                (*out) = math::xyz( weights[k] * (matrices[ bones[k] ] * attributes[i]) ); ++k;
                for (j = 1; j<wcount[i]; ++j, ++k) 
                {
                    (*out) += math::xyz( weights[k] * (matrices[ bones[k] ] * attributes[i]) );
                }
            }
            else {
                (*out) = math::xyz(attributes[i]);
            }
        }
    }

    template<typename OutIterator>
    void transform_attributes3f( const math::vector_of_vector3f&  attributes,
                                 const std::vector<unsigned>&     wcount,
                                 const std::vector<float>&        weights,
                                 const std::vector<int>&          bones,
                                 const math::vector_of_matrix4f&  matrices,
                                 OutIterator                      out )
    {
        size_t i,j,k;
        for (i = 0, k = 0; i<wcount.size(); ++i, ++out)
        {
            if (wcount[i] > 0)
            {
                (*out) += math::xyz( weights[k] * (matrices[ bones[k] ] * math::make_vec(attributes[i], 0.0f) ) ); ++k;
                for (j = 1; j<wcount[i]; ++j, ++k) {
                    (*out) += math::xyz( weights[k] * (matrices[ bones[k] ] * math::make_vec(attributes[i], 0.0f) ) );
                }
            }
            else {
                (*out) = attributes[i];
            }
        }
    }

    template<typename T, typename Y, typename OutIterator>
    void copy_attrubutes( const void*   data,
                          size_t        offset,
                          size_t        size,
                          size_t        stride,
                          size_t        numVerts,
                          OutIterator   out )
    {
        const char* ptr = reinterpret_cast<const char*>(data) + offset;
        for (size_t i = 0; i<numVerts; ++i, ptr += stride)
        {
            for (size_t j = 0; j<size; ++j, ++out) 
            {
                (*out) = Y(*(reinterpret_cast<const T*>(ptr) + j));
            }
        }
    }

} // anonymous namespace

namespace slon {
namespace graphics {

SkinnedMesh::SkinnedMesh(const DESC& desc)
:	mesh(desc.mesh)
,	cpuSkinning(false)
,	haveBindShapeBone(false)
,	bounds( mesh->getBounds() )
,	shadowCaster(true)
,	shadowReceiver(true)
{
    assert(mesh);

    // dummy binders
    const_binding_mat4x4f_ptr   dummyMatrixBinder( new parameter_binding<math::Matrix4f>(0) );
    const_binding_vec4f_ptr     dummyVec4Binder( new parameter_binding<math::Vector4f>(0) );
    const_binding_vec3f_ptr     dummyVec3Binder( new parameter_binding<math::Vector3f>(0) );

    // bind parameters
	const_binding_mat4x4f_ptr worldMatrixBinder( new parameter_binding<math::Matrix4f>(&worldMatrix, true) );
    for (Mesh::subset_iterator iter  = mesh->firstSubset();
                               iter != mesh->endSubset();
                               ++iter)
    {
        graphics::Effect* effect = (*iter)->getEffect();
        effect->bindParameter(hash_string("worldMatrix"), worldMatrixBinder.get());

		// bind dummy binders just to test effect to support skinning
		if ( !cpuSkinning 
             && !effect->bindParameter(hash_string("boneMatrices"), dummyMatrixBinder.get()) 
             && (!effect->bindParameter(hash_string("boneRotations"), dummyVec4Binder.get())
                 || !effect->bindParameter(hash_string("boneTranslations"), dummyVec3Binder.get())) )
		{
			// toggle to CPU skinning
			cpuSkinning = true;
			for (Mesh::subset_iterator j  = mesh->firstSubset();
									   j != iter;
									   ++j)
			{
				(*j)->getEffect()->bindParameter(hash_string("boneMatrices"), 0);
				(*j)->getEffect()->bindParameter(hash_string("boneRotations"), 0);
				(*j)->getEffect()->bindParameter(hash_string("boneTranslations"), 0);
			}

			break;
		}
    }

	// copy data necessary to perform CPU skinning
	if (cpuSkinning)
	{
		// find necessary attributes
		positionIter   = mesh->endAttribute();
		normalIter     = mesh->endAttribute();
		tangentIter    = mesh->endAttribute();
		boneIndexIter  = mesh->endAttribute();
		boneWeightIter = mesh->endAttribute();
		for (Mesh::attribute_const_iterator iter  = mesh->firstAttribute();
											iter != mesh->endAttribute();
											++iter)
		{
			if ( iter->semantic == Mesh::POSITION ) {
				positionIter = iter;
			}
			else if ( iter->semantic == Mesh::NORMAL ) {
				normalIter = iter;
			}
			else if ( iter->semantic == Mesh::TANGENT ) {
				tangentIter = iter;
			}
			else if ( iter->semantic == Mesh::BONE_INDEX ) {
				boneIndexIter = iter;
			}
			else if ( iter->semantic == Mesh::BONE_WEIGHT ) {
				boneWeightIter = iter;
			}
		}

		if ( boneIndexIter == mesh->endAttribute() ) {
			throw slon_error(logger, "Skinned mesh doesn't have bone index attribute.");
		}
		else if ( boneWeightIter == mesh->endAttribute() ) {
			throw slon_error(logger, "Skinned mesh doesn't have bone weight attribute.");
		}
		else if ( boneIndexIter->size != boneWeightIter->size ) {
			throw slon_error(logger, "Skinned mesh bone_weight and bone_index attributes have different sizes.");
		}

		Mesh::buffer_lock lock = mesh->lockVertexBuffer(Mesh::LOCK_READ);
		{
			// query positions
			if ( positionIter != mesh->endAttribute() )
			{
				switch (positionIter->type)
				{   
					case sgl::FLOAT:
					{
						if (positionIter->size == 3)
						{
							Mesh::vec3f_accessor positionAccessor(mesh.get(), positionIter, lock);
							positions.resize( mesh->getNumVertices() );
							for (size_t i = 0; i<positions.size(); ++i) {
								positions[i] = math::make_vec(positionAccessor[i], 1.0f);
							}
						}
						else if (positionIter->size == 4)
						{
							Mesh::vec4f_accessor positionAccessor(mesh.get(), positionIter, lock);
							positions.resize( mesh->getNumVertices() );
							std::copy( positionAccessor.begin(), positionAccessor.end(), positions.begin() );
						}
						else {
							throw slon_error(logger, "Skinned mesh position attribute have invalid size. Must be either 3 or 4.");
						}
                    
						break;
					}

					default:
						throw slon_error(logger, "Skinned mesh position attribute has invalid type. Must be FLOAT.");
						break;
				}
			}

			// query normals
			if ( normalIter != mesh->endAttribute() )
			{
				switch (normalIter->type)
				{   
					case sgl::FLOAT:
					{
						if (normalIter->size == 3)
						{
							Mesh::vec3f_accessor normalAccessor(mesh.get(), normalIter, lock);
							normals.resize( mesh->getNumVertices() );
							std::copy( normalAccessor.begin(), normalAccessor.end(), normals.begin() );
						}
						else {
							throw slon_error(logger, "Skinned mesh normal attribute have invalid size. Must be 3.");
						}
                    
						break;
					}

					default:
						throw slon_error(logger, "Skinned mesh position attribute has invalid type. Must be FLOAT.");
						break;
				}
			}

			// query tangents
			if ( tangentIter != mesh->endAttribute() )
			{
				switch (normalIter->type)
				{   
					case sgl::FLOAT:
					{
						if (tangentIter->size == 3)
						{
							Mesh::vec3f_accessor tangentAccessor(mesh.get(), tangentIter, lock);
							normals.resize( mesh->getNumVertices() );
							std::copy( tangentAccessor.begin(), tangentAccessor.end(), tangents.begin() );
						}
						else {
							throw slon_error(logger, "Skinned mesh tangent attribute have invalid size. Must be 3.");
						}

						break;
					}

					default:
						throw slon_error(logger, "Skinned mesh tangent attribute has invalid type. Must be FLOAT.");
						break;
				}
			}

			if (desc.jointArray && desc.weightArray && desc.jointCountArray)
			{
			    // remap indices
			    size_t numWeights = 0;
				weightCount.resize( mesh->getNumVertices() );
				for (size_t i = 0; i<mesh->getNumVertices(); ++i) 
				{
					weightCount[i] = desc.jointCountArray[i];
					numWeights    += weightCount[i];
				}

				bones.resize(numWeights);
				weights.resize(numWeights);
				for (size_t i = 0; i<numWeights; ++i) 
				{
					bones[i]   = desc.jointArray[i];
					weights[i] = desc.weightArray[i];
				}
			}
			else
			{
				// query weights
				switch (boneWeightIter->type)
				{      
					case sgl::FLOAT:
					{
						std::vector<float> plainWeights( mesh->getNumVertices() * boneWeightIter->size );
						copy_attrubutes<float, float>( lock->data(), boneWeightIter->offset, boneWeightIter->size, mesh->getVertexSize(), mesh->getNumVertices(), plainWeights.begin() );

						weightCount.resize( mesh->getNumVertices() );
						weights.reserve( plainWeights.size() );
						for (size_t i = 0; i<mesh->getNumVertices(); ++i) 
						{
							weightCount[i] = 0;
							for (unsigned j = 0; j<boneWeightIter->size; ++j)
							{
								float weight = plainWeights[i*boneWeightIter->size + j];
								if (weight > 0.0f) 
								{
									++weightCount[i];
									weights.push_back(weight);
								}
							}
						}

						break;
					}

					default:
						throw slon_error(logger, "Skinned mesh bone_weight attribute has invalid type. Must be FLOAT.");
						break;
				}

				// query indices
				std::vector<int> boneIndices( mesh->getNumVertices() * boneIndexIter->size );
				switch (boneIndexIter->type)
				{
					case sgl::BYTE:
						copy_attrubutes<char, int>( lock->data(), boneIndexIter->offset, boneIndexIter->size, mesh->getVertexSize(), mesh->getNumVertices(), boneIndices.begin() );
						break;

					case sgl::UBYTE:
						copy_attrubutes<unsigned char, int>( lock->data(), boneIndexIter->offset, boneIndexIter->size, mesh->getVertexSize(), mesh->getNumVertices(), boneIndices.begin() );
						break;
                
					case sgl::SHORT:
						copy_attrubutes<short, int>( lock->data(), boneIndexIter->offset, boneIndexIter->size, mesh->getVertexSize(), mesh->getNumVertices(), boneIndices.begin() );
						break;

					case sgl::USHORT:
						copy_attrubutes<unsigned short, int>( lock->data(), boneIndexIter->offset, boneIndexIter->size, mesh->getVertexSize(), mesh->getNumVertices(), boneIndices.begin() );
						break;

					case sgl::INT:
						copy_attrubutes<int, int>( lock->data(), boneIndexIter->offset, boneIndexIter->size, mesh->getVertexSize(), mesh->getNumVertices(), boneIndices.begin() );
						break;

					case sgl::UINT:
						copy_attrubutes<unsigned, int>( lock->data(), boneIndexIter->offset, boneIndexIter->size, mesh->getVertexSize(), mesh->getNumVertices(), boneIndices.begin() );
						break;
            
					case sgl::FLOAT:
						copy_attrubutes<float, int>( lock->data(), boneIndexIter->offset, boneIndexIter->size, mesh->getVertexSize(), mesh->getNumVertices(), boneIndices.begin() );
						break;

					default:
						throw slon_error(logger, "Skinned mesh bone_index attribute has invalid type. Must be either FLOAT, UINT, INT, USHORT, SHORT, UBYTE, BYTE.");
						break;
				}

				// remap indices
				size_t oldIndex = 0;
				size_t newIndex = 0;
				bones.resize( weights.size() );
				for (size_t i = 0; i<weightCount.size(); ++i)
				{
					for (unsigned j = 0; j<weightCount[i]; ++j)
					{
						bones[newIndex++] = boneIndices[oldIndex++];
					}
					oldIndex += boneIndexIter->size - weightCount[i];
				}
			}

			// find maximum bone index
			int maxBone = 0;
			for (size_t i = 0; i<bones.size(); ++i) {
				maxBone = std::max(maxBone, bones[i]);
			}

			for (size_t i = 0; i<bones.size(); ++i) 
			{
				if (bones[i] == -1) 
				{
					haveBindShapeBone   = true;
					bones[i]            = maxBone + 1;
				}
			}

			// setup number of joints accordinly to the maximum bone index used
			joints.resize(haveBindShapeBone ? maxBone + 2 : maxBone + 1);
			boneMatrices.resize(haveBindShapeBone ? maxBone + 2 : maxBone + 1);
		}
	}
}

const math::AABBf& SkinnedMesh::getBounds() const
{
	if ( skeleton && skeleton->getRootJoint() ) {
		return bounds;
	}

	return mesh->getBounds();
}

void SkinnedMesh::setSkeleton(scene::Skeleton* skeleton_)
{
    if (skeleton_)
    {
        std::fill( joints.begin(), joints.end(), scene::const_joint_ptr() );

        gather_joints_visitor visitor(*this, joints);
        visitor.traverse(*skeleton_);

        // check we have all necessary joints
        for (int i = 0; i<int(joints.size() - 1); ++i)
        {
            if (!joints[i]) 
            {
                std::ostringstream errorMsg;
                errorMsg << "SkinnedMesh '" << getName() << "doesn't have " << i << " joint" << std::endl;
                throw slon_error( logger, errorMsg.str() );
            }
        }

		if (!cpuSkinning)
		{
			boneMatrices.resize( joints.size() );
            boneRotations.clear();
            boneTranslations.clear();

			const_binding_mat4x4f_ptr   boneMatricesBinder( new parameter_binding<math::Matrix4f>(&boneMatrices[0], boneMatrices.size(), true) );
            const_binding_vec4f_ptr     boneRotationsBinder;
            const_binding_vec3f_ptr     boneTranslationsBinder;

			for (Mesh::subset_iterator iter  = mesh->firstSubset();
									   iter != mesh->endSubset();
									   ++iter)
			{
                graphics::Effect* effect = (*iter)->getEffect();
				if ( !effect->bindParameter(hash_string("boneMatrices"), boneMatricesBinder.get()) )
                {
                    if ( boneRotations.empty() ) 
                    {
                        boneRotations.resize( joints.size() );
                        boneRotationsBinder.reset( new parameter_binding<math::Vector4f>(&boneRotations[0], boneRotations.size(), true) );
                    }

                    if ( boneTranslations.empty() ) 
                    {
                        boneTranslations.resize( joints.size() );
                        boneTranslationsBinder.reset( new parameter_binding<math::Vector3f>(&boneTranslations[0], boneTranslations.size(), true) );
                    }

                    effect->bindParameter(hash_string("boneRotations"), boneRotationsBinder.get());
                    effect->bindParameter(hash_string("boneTranslations"), boneTranslationsBinder.get());
                }
			}
		}

        skeleton.reset(skeleton_);
    }
    else 
    {
        logger << log::S_WARNING << "SkinnedMesh '" << getName() << "have no skeleton" << std::endl;
    }
}

void SkinnedMesh::accept(scene::TransformVisitor& visitor)
{
    worldMatrix    = visitor.getLocalToWorldTransform();
    invWorldMatrix = visitor.getWorldToLocalTransform();

	// query joint matrices
	bounds = mesh->getBounds();
    if (joints.size() > 0)
    {
        if (haveBindShapeBone) 
        {
            for (size_t i = 0; i<joints.size() - 1; ++i) 
			{
                boneMatrices[i]  = joints[i]->getSkinningMatrix();
				bounds			 = math::merge( mesh->getBounds() * joints[i]->getTransform(), bounds );
			}

            boneMatrices.back() = math::make_identity<float, 4>();
        }
        else
        {
            for (size_t i = 0; i<joints.size(); ++i) 
			{
                boneMatrices[i]  = joints[i]->getSkinningMatrix();
				bounds			 = math::merge( mesh->getBounds() * joints[i]->getTransform(), bounds );
            }
        }
    }

    if ( !boneRotations.empty() && !boneTranslations.empty() ) 
    {
        for (size_t i = 0; i<boneMatrices.size(); ++i)
        {
            boneRotations[i]    = math::as_vec( math::from_matrix(boneMatrices[i]) );
            boneTranslations[i] = math::get_translation(boneMatrices[i]);
        }
    }
}

void SkinnedMesh::accept(scene::CullVisitor& visitor) const
{
    // perform CPU skinning
	if (cpuSkinning)
    {
        Mesh::buffer_lock lock = mesh->lockVertexBuffer(Mesh::LOCK_WRITE);

        if ( positionIter != mesh->endAttribute() )
        {
            if (positionIter->size == 4)
            {
                Mesh::vec4f_accessor positionAccessor(mesh.get(), positionIter, lock);
                transform_attributes4f( positions,
                                        weightCount,
                                        weights,
                                        bones,
                                        boneMatrices,
                                        positionAccessor.begin() );
            }
            else 
            {
                Mesh::vec3f_accessor positionAccessor(mesh.get(), positionIter, lock);
                transform_attributes3f( positions,
                                        weightCount,
                                        weights,
                                        bones,
                                        boneMatrices,
                                        positionAccessor.begin() );
            }
        }

        if ( normalIter != mesh->endAttribute() )
        {
            Mesh::vec3f_accessor normalAccessor(mesh.get(), normalIter, lock);
            transform_attributes3f( normals,
                                    weightCount,
                                    weights,
                                    bones,
                                    boneMatrices,
                                    normalAccessor.begin() );
        }

        if ( tangentIter != mesh->endAttribute() )
        {
            Mesh::vec3f_accessor tangentAccessor(mesh.get(), tangentIter, lock);
            transform_attributes3f( tangents,
                                    weightCount,
                                    weights,
                                    bones,
                                    boneMatrices,
                                    tangentAccessor.begin() );
        }
    }

    for( Mesh::subset_const_iterator subsetIter  = mesh->firstSubset();
                                     subsetIter != mesh->endSubset();
                                     ++subsetIter )
    {
        visitor.addRenderable( subsetIter->get() );
    }

    base_type::accept(visitor);
}

} // namespace graphics
} // namespace slon
