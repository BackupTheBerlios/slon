#include "stdafx.h"
#include "Animation/Detail/Animation.h"
#include "Animation/Detail/AnimationController.h"
#include "Animation/Detail/AnimationTrack.h"
#include "Database/Collada/Collada.h"
#include "Database/DatabaseManager.h"
#include "Database/Detail/Library.h"
#include "FileSystem/File.h"
#include "FileSystem/FileSystemManager.h"
#include "Graphics/Common.h"
#include "Graphics/Renderable/SkinnedMesh.h"
#include "Graphics/Renderable/StaticMesh.h"
#include "Graphics/Effect/LightingEffect.h"
#include "Scene/Skeleton.h"
#include "Scene/Visitors/CullVisitor.h"
#include "Scene/Visitors/DFSNodeVisitor.h"
#include "Scene/Visitors/FilterVisitor.h"
#include "Scene/Visitors/TransformVisitor.h"
#include "Utility/uri/file_uri.hpp"

#ifdef SLON_ENGINE_USE_PHYSICS
#   include "Physics/PhysicsManager.h"
#   include "Physics/PhysicsModel.h"
#endif

__DEFINE_LOGGER__("database.COLLADA")

namespace {

	using namespace math;
	using namespace slon;
    using namespace scene;
	using namespace database;

	using namespace std;
	
    std::string tolower(const std::string& str)
    {
		std::locale locale;
        std::string out(str);
        for (size_t i = 0; i<str.size(); ++i) {
            out[i] = std::tolower(out[i], locale);
        }
        return out;
    }
	
    std::string toupper(const std::string& str)
    {
		std::locale locale;
        std::string out(str);
        for (size_t i = 0; i<str.size(); ++i) {
            out[i] = std::toupper(out[i], locale);
        }
        return out;
    }

    struct find_by_id
    {
        find_by_id(const std::string& id_) :
            id(id_)
        {}
        
        bool operator () (const scene::Node& node) const
        {
            std::string node_id = node.getName().str();
            size_t      sharp_i = node_id.find('#');
            if ( sharp_i != std::string::npos ) {
                node_id = node_id.substr(0, sharp_i);
            }

            return node_id == id;
        }

        std::string id;
    };

    struct find_by_sid
    {
        find_by_sid(const std::string& sid_) :
            sid(sid_)
        {}
        
        bool operator () (const scene::Node& node) const
        {
            std::string node_sid = node.getName().str();
            size_t      sharp_i  = node_sid.find('#');
            if ( sharp_i != std::string::npos ) {
                node_sid = node_sid.substr(sharp_i + 1);
            }

            return node_sid == sid;
        }

        std::string sid;
    };

    class find_transform_visitor :
        public scene::FilterVisitor<scene::DFSNodeVisitor, scene::Joint, scene::MatrixTransform>
    {
    public:
        find_transform_visitor(hash_string name_)
        :   found(0)
        ,   name(name_)
        {}

        find_transform_visitor(const std::string& name_)
        :   found(0)
        ,   name(name_)
        {}

        void visit(scene::Joint& joint)
        {
            find_by_id pr(name.str());
            if ( pr(joint) ) {
                found = &joint;
            }
        }

        void visit(scene::MatrixTransform& mt)
        {
            if (mt.getName() == name) {
                found = &mt;
            }
        }

        MatrixTransform*  found;
        hash_string       name;
    };

	class SceneBuilder
	{
    public:
        typedef std::map<std::string, graphics::mesh_ptr>       mesh_map;
        typedef std::vector<collada_instance_controller_ptr>    controller_vector;

	public:
		SceneBuilder(const ColladaDocument& _document) :
			document(_document)
		{
		}

		sgl::Texture2D* createTexture2D(const collada_texture& colladaTexture)
		{
			assert(colladaTexture.sampler && "Texture must have sampler. Overwise it is COLLADA error");
			assert(colladaTexture.sampler->surface && "Sampler must have surface. Overwise it is COLLADA error");
			assert(colladaTexture.sampler->surface->image && "Surface must have image. Overwise it is COLLADA error");

			// texture
			graphics::Texture2D* texture = 0;

			// load image
			const collada_image& image = *(colladaTexture.sampler->surface->image);
			if (image.image) {
                texture = image.image->CreateTexture2D();
			}
			else
			{
				file_uri<char> uri(image.uri);
				std::string fileName = uri.valid() ? uri.path : image.uri;
                texture = dynamic_cast<graphics::Texture2D*>( database::loadTexture(fileName).get() );
				if (!texture)
				{
					logger << log::S_WARNING << "Failed to load texture from file: " << fileName << std::endl;
					return texture;
				}
			}
            texture->GenerateMipmap();

			// setup sampler parameters
			const collada_sampler2D_param& sampler = *colladaTexture.sampler;
            {
                sgl::SamplerState::DESC desc;
                desc.filter[0]   = sampler.getFilter(0);
                desc.filter[1]   = sampler.getFilter(1);
                desc.filter[2]   = sampler.getFilter(2);
                desc.wrapping[0] = sampler.getWrapping(0);
                desc.wrapping[1] = sampler.getWrapping(1);
                
                //if (desc.filter[0] == sgl::SamplerState::LINEAR && desc.filter[2] == sgl::SamplerState::LINEAR) {
                //    texture->GenerateMipmap();
                //}

                sgl::SamplerState* samplerState = graphics::currentDevice()->CreateSamplerState(desc);
                texture->BindSamplerState(samplerState);
            }

			return texture;
		}

		graphics::LightingMaterial* createLightingMaterial(const collada_phong_technique& phongTechnique)
		{
			graphics::LightingMaterial* lightingMaterial = new graphics::LightingMaterial();

			// setup diffuse
            float specular = math::length( math::xyz(phongTechnique.specular.color) );
			if (phongTechnique.diffuse.type == collada_color_or_texture::TEXTURE)
            {
                lightingMaterial->setDiffuseSpecular( Vector4f(1.0f, 1.0f, 1.0f, specular) );
				lightingMaterial->setDiffuseSpecularMap( createTexture2D(phongTechnique.diffuse.texture) );
			}
			else {
                lightingMaterial->setDiffuseSpecular( math::make_vec( math::xyz(phongTechnique.diffuse.color), specular ) );
			}

			// setup emission
			if (phongTechnique.emission.type == collada_color_or_texture::TEXTURE) {
				lightingMaterial->setEmissionMap( createTexture2D(phongTechnique.emission.texture) );
			}
			else if ( phongTechnique.emission.color != Vector4f(0.0f, 0.0f, 0.0f, 0.0f) ) {
                lightingMaterial->setEmission( phongTechnique.emission.color );
			}

			// setup shininess
			lightingMaterial->setShininess(phongTechnique.shininess.value);

			return lightingMaterial;
		}

		graphics::material_ptr createMaterial(const collada_material& material)
		{
			assert(material.effect && "Material must have valid effect. Overwise it is COLLADA error.");

			logger << log::S_NOTICE << "Creating material: " << material.id << std::endl;
			if (collada_effect* colladaEffect = material.effect)
			{
				logger << log::S_NOTICE << "Found effect: " << colladaEffect->id << std::endl;

				collada_technique_ptr colladaTechnique = colladaEffect->techniques[0];
				if (colladaEffect->techniques.size() > 1) {
					logger << log::S_WARNING << "Effect has more than one technique. SlonEngine will use first." << std::endl;
				}

				graphics::Material* material = 0;
				switch ( colladaTechnique->getTechniqueType() )
				{
					case collada_technique::PHONG:
					{
						const collada_phong_technique& phongTechnique = static_cast<const collada_phong_technique&>(*colladaTechnique);
						material = createLightingMaterial(phongTechnique);
						break;
					}
					default:
						assert(!"Can't get here");
						break;
				}

				if (material) {
					logger << log::S_NOTICE << "Material succesfully created." << std::endl;
				}

                return graphics::material_ptr(material);
			}
			else
			{
				logger << log::S_ERROR << "Material doesn't have attached effect." << std::endl;
			}

			return graphics::material_ptr();
		}

		// loads mesh from collada mesh
        graphics::mesh_ptr createMesh(const collada_mesh&               colladaMesh,
						        	  const collada_bind_material_ptr&  bindMaterial,
                                      const collada_skin*               skin = 0)
		{
			using namespace sgl;

			// make geometry mesh
            graphics::mesh_ptr mesh;
            mesh_map::iterator iter = meshMap.find(colladaMesh.id);
			if ( !colladaMesh.id.empty() && iter != meshMap.end() ) {
                mesh.reset( (*iter).second->shallowCopy() );
			}
            else
            {
                // mesh without primitives elements is empty for SlonEngine
                if ( colladaMesh.primitives.empty() ) {
                    return graphics::mesh_ptr();
                }

		        // merge inputs
		        collada_primitives::input_vector inputs = colladaMesh.vertices.inputs;
		        for (size_t i = 0; i<colladaMesh.primitives.size(); ++i)
		        {
			        const collada_primitives& prims = colladaMesh.primitives[i];

			        // add new primitives inputs
			        for (size_t j = 0; j<prims.inputs.size(); ++j)
			        {
				        const collada_input& input = prims.inputs[j];

				        // ignore unknown inputs
				        if (input.attributeIndex < 0) {
					        continue;
				        }

				        // find input by index
				        collada_primitives::const_input_iterator inputIter =
					        std::find_if( inputs.begin(),
							              inputs.end(),
							              boost::bind(&collada_input::attributeIndex, _1) == input.attributeIndex );

				        // input not found add it
				        if ( inputIter == inputs.end() ) {
					        inputs.push_back(input);
				        }
				        else if ( inputIter->source != input.source )
				        {
					        throw construct_scene_error(logger, "Geometry primitives have inputs with similar semantic and different source. \
							 					                 SlonEngine doesn't know what to do in this situation.");
				        }
			        }
		        }

		        // can't construct mesh from unknown primitives
		        if ( inputs.empty() ) {
			        throw construct_scene_error(logger, "Couldn't construct mesh from invalid inputs.");
		        }

		        // merge indices
		        std::vector<collada_primitives::int_vector>	indices( inputs.size() );
		        for (size_t i = 0; i<inputs.size(); ++i)
		        {
			        const collada_input& input = inputs[i];

			        // check if it is vertex input
			        bool vertexInput = i < colladaMesh.vertices.inputs.size();

			        // merge indices for each input
			        for(size_t j = 0; j<colladaMesh.primitives.size(); ++j)
			        {
				        const collada_primitives& prims = colladaMesh.primitives[j];

				        // find corresponding input
				        collada_primitives::const_input_iterator inputIter = prims.inputs.end();
				        if (vertexInput)
				        {
					        inputIter = std::find_if( prims.inputs.begin(),
										              prims.inputs.end(),
										              boost::bind(&collada_input::attributeName, _1) == "VERTEX" );
				        }
				        else
				        {
					        inputIter = std::find_if( prims.inputs.begin(),
										              prims.inputs.end(),
										              boost::bind(&collada_input::attributeIndex, _1) == input.attributeIndex );
				        }

				        // input not found - continue
				        if ( inputIter == prims.inputs.end() )
				        {
					        // keep inputIndices size
					        size_t size = indices[i].size();
					        indices[i].resize( size + prims.inputIndices[0].size() );
					        std::fill( indices[i].begin() + size,
							           indices[i].end(),
							           0 );
				        }
				        else
				        {
					        // add indices
					        size_t inputIndex = inputIter - prims.inputs.begin();
					        std::copy( prims.inputIndices[inputIndex].begin(),
							           prims.inputIndices[inputIndex].end(),
							           std::back_inserter(indices[i]) );
				        }
			        }
		        }

                // create mesh
                graphics::mesh_data_ptr meshData(new graphics::MeshData);
                {
		            // setup data
		            for (size_t i = 0; i<inputs.size(); ++i)
		            {
                        if ( inputs[i].attributeName == "position" && skin )
                        {
                            // transform positions by bind shape matrix
                            size_t size = inputs[i].source->floatArray.size() / inputs[i].source->stride;
                            if (inputs[i].source->stride == 3)
                            {
                                for (size_t j = 0; j<size; ++j)
                                {
                                    math::Vector4f vec = math::make_vec( inputs[i].getVector<float, 3>(j), 1.0f );
                                    inputs[i].setVector<float, 3>( j, math::xyz(skin->bindShapeMatrix * vec) );
                                }
                            }
                            else if (inputs[i].source->stride == 4)
                            {
                                for (size_t j = 0; j<size; ++j) {
                                    inputs[i].setVector<float, 4>( j, skin->bindShapeMatrix * inputs[i].getVector<float, 4>(j) );
                                }
                            }
                            else {
                                logger << log::S_WARNING << "Skinned mesh have non 3 or 4-component positions, ignoring bind shape matrix transformation\n";
                            }
                        }

                        if ( !inputs[i].source->floatArray.empty() )
                        {
			                meshData->setAttributes( inputs[i].attributeName,
                                                     inputs[i].attributeIndex,
								                     inputs[i].source->stride,
                                                     inputs[i].source->floatArray.size() / inputs[i].source->stride,
								                     sgl::FLOAT,
								                     &inputs[i].source->floatArray[0] );

			                meshData->setIndices( inputs[i].attributeIndex,
								                  indices[i].size(),
									              &indices[i][0] );
                        }
		            }

                    // reserve attributes for skinning
                    if (skin)
                    {
				        // find corresponding input
				        collada_primitives::const_input_iterator jointInputIter = std::find_if( skin->weights.inputs.begin(),
										                                                        skin->weights.inputs.end(),
										                                                        boost::bind(&collada_input::attributeName, _1) == "joint" );
                        if ( jointInputIter == skin->weights.inputs.end() ) {
					        throw construct_scene_error(logger, "Skinned mesh doesn't have joint indices.");
                        }

				        collada_primitives::const_input_iterator weightInputIter = std::find_if( skin->weights.inputs.begin(),
										                                                         skin->weights.inputs.end(),
										                                                         boost::bind(&collada_input::attributeName, _1) == "weight" );
                        if ( weightInputIter == skin->weights.inputs.end() ) {
					        throw construct_scene_error(logger, "Skinned mesh doesn't have joint weights.");
                        }

                        math::vector_of_vector4f boneIndices( skin->weights.vcount.size() );
                        math::vector_of_vector4f boneWeights( skin->weights.vcount.size() );

                        size_t index = 0;
                        for (size_t i = 0; i<skin->weights.vcount.size(); ++i)
                        {
                            unsigned vc = skin->weights.vcount[i];
                            float    ws = 0.0f;
                            unsigned k  = 0;
                            for (unsigned j = 0; j<vc; ++j) 
                            {
                                int   boneIndex    = skin->weights.v[index + jointInputIter->offset];
                                int   weightIndex  = skin->weights.v[index + weightInputIter->offset];
                                float weight       = weightInputIter->source->floatArray[weightIndex];
                                index             += skin->weights.inputs.size();

                                if (weight > 0.0f)
                                {
                                    if (k == 4)
                                    {
                                        logger << log::S_WARNING << "SlonEngine doesn't support more than 4 bones per vertex, ignoring exceeded" << std::endl;
                                        index  += (vc - j - 1) * skin->weights.inputs.size();
                                        vc      = 4;
                                    }
                                    else
                                    {
                                        boneIndices[i][k] = (float)boneIndex;
                                        boneWeights[i][k] = weightInputIter->source->floatArray[weightIndex];
                                        ws += weight; 
                                        ++k;
                                    }
                                }
                            }

                            // normalize weights
                            for (unsigned j = 0; j<k; ++j) {
                                boneWeights[i][j] /= ws;
                            }

                            for (unsigned j = k; j<4; ++j) 
                            {
                                boneIndices[i][j] = 0.0f;
                                boneWeights[i][j] = 0.0f;
                            }
                        }

                        meshData->setAttributes( "bone_index",
                                                 graphics::Mesh::BONE_INDEX,
								                 4,
                                                 boneIndices.size(),
								                 sgl::FLOAT,
								                 &boneIndices[0] );

                        meshData->setAttributes( "bone_weight",
                                                 graphics::Mesh::BONE_WEIGHT,
								                 4,
                                                 boneWeights.size(),
								                 sgl::FLOAT,
								                 &boneWeights[0] );
                        
                    }
                }
                mesh.reset( new graphics::Mesh(meshData.get()) );

		        // setup subsets
		        size_t stride = 0;
		        for(size_t i = 0; i<colladaMesh.primitives.size(); ++i)
		        {
			        const collada_primitives& prims = colladaMesh.primitives[i];
			        mesh->addIndexedSubset( 0, TRIANGLES, stride, prims.inputIndices[0].size() );
			        stride += prims.inputIndices[0].size();
		        }
            }

			// bind materials
			if (bindMaterial)
			{
                typedef std::vector<graphics::material_ptr>     material_vector;
				typedef std::vector<collada_instance_material> 	instance_material_vector;
				typedef instance_material_vector::iterator		instance_material_iterator;

				instance_material_vector    primitiveMaterials;
                material_vector             materials;
				for(size_t i = 0; i<colladaMesh.primitives.size(); ++i)
				{
					const collada_primitives& prims = colladaMesh.primitives[i];
					if ( !prims.material.empty() )
					{
						instance_material_iterator materialInstanceIter =
								std::find_if( bindMaterial->materials.begin(),
										      bindMaterial->materials.end(),
											  boost::bind(&collada_instance_material::symbol, _1) == prims.material );

						if ( materialInstanceIter != bindMaterial->materials.end() )
                        {
                            // find material if already added
                            instance_material_iterator iter = std::find( primitiveMaterials.begin(),
											                             primitiveMaterials.end(),
												                         *materialInstanceIter );

							if ( iter != primitiveMaterials.end() )
							{
                                // add sub subset
                                size_t primIndex = std::distance( iter, primitiveMaterials.begin() );
                                mesh->getSubset(i).setEffect( materials[primIndex]->createEffect() );
							}
							else
							{
								// add new subset
                                graphics::material_ptr material = createMaterial(**materialInstanceIter);
                                mesh->getSubset(i).setEffect( material->createEffect() );
                                primitiveMaterials.push_back(*materialInstanceIter);
                                materials.push_back(material);
							}
						}
						else {
							logger << log::S_ERROR << "Couldn't find material by symbol: " << prims.material << std::endl;
						}
                    }
				}
			}
            else
            {
                // use default material
                graphics::material_ptr defaultMaterial(new graphics::LightingMaterial);
                for(size_t i = 0; i<colladaMesh.primitives.size(); ++i) {
                    mesh->getSubset(i).setEffect( defaultMaterial->createEffect() );
                }
                logger << log::S_WARNING << "Mesh subset doesn't have any material, using default." << std::endl;
            }

			return mesh;
		}

		graphics::StaticMesh* createStaticMesh( const collada_mesh& 				colladaMesh,
						        			    const collada_bind_material_ptr& 	bindMaterial )
		{
			// make mesh
            graphics::mesh_ptr mesh = createMesh(colladaMesh, bindMaterial);
            if (mesh)
            {
                graphics::StaticMesh* staticMesh = new graphics::StaticMesh( mesh.get() );
			    staticMesh->setName( hash_string(colladaMesh.name) );
			    return staticMesh;
            }
            else 
            {
			    logger << log::S_WARNING << "Mesh is empty." << std::endl;
                return 0;
            }
		}

		scene::Node* createGeometry( const collada_geometry& 			colladaGeometry,
									 const collada_bind_material_ptr& 	bindMaterial = collada_bind_material_ptr() )
		{
			scene::Node* geometry = 0;
			switch ( colladaGeometry.getGeometryType() )
			{
				case collada_geometry::MESH:
				{
					const collada_mesh& mesh = static_cast<const collada_mesh&>(colladaGeometry);
					geometry = createStaticMesh(mesh, bindMaterial);
                    if (!geometry) 
                    {
                        // create dummy node replacement
                        geometry = new scene::Node();
                        geometry->setName( hash_string(mesh.name) );
                    }
					break;
				}

				default:
					assert(!"Can't get here");
					break;
			}

			return geometry;
		}
		
		scene::Node* createGeometry(const collada_instance_geometry& geometryInstance)
		{					
			if (geometryInstance.material) {
				return createGeometry(*geometryInstance.geometry, geometryInstance.material);
			}
			else {
				return createGeometry(*geometryInstance.geometry);
			}
		}

        graphics::SkinnedMesh* createSkinnedMesh( const collada_skin&               skin,
									              const collada_bind_material_ptr&  bindMaterial )
        {
            graphics::SkinnedMesh*		skinnedMesh = 0;
			graphics::SkinnedMesh::DESC desc;

            assert(skin.source);
            switch ( skin.source->getGeometryType() )
            {
				case collada_geometry::MESH:
				{
					graphics::mesh_ptr mesh = createMesh(static_cast<const collada_mesh&>(*skin.source), bindMaterial, &skin);
                    desc.mesh	= mesh.get();
                    skinnedMesh = new graphics::SkinnedMesh(desc);
					break;
				}

				default:
					assert(!"Invalid geometry type for skinning");
					break;
            }

            return skinnedMesh;
        }

		scene::Group* createNode(const collada_node& node)
		{
			using namespace scene;

			// create node
            if (node.type == collada_node::JOINT)
            {
                Joint* joint = new Joint();
                joint->setName( hash_string(node.id + "#" + node.sid) );
				joint->setTransform(node.transform);

			    // create hierarchy
                if ( !node.geometries.empty() || !node.controllers.empty() ) {
                    logger << log::S_WARNING << "Joint node '" << node.id << "' can't have entities children, skipping.";
                }

			    for (size_t i = 0; i<node.children.size(); ++i)
			    {
				    joint->addChild( createNode(*node.children[i]) );
			    }

                return joint;
            }
            else
            {
			    Group* group = 0;
			    if ( node.transform == make_identity<float, 4>() ) {
				    group = new Group();
			    }
			    else
			    {
				    MatrixTransform* transform = new MatrixTransform();
				    transform->setTransform(node.transform);
				    group = transform;
			    }
			    group->setName( hash_string(node.name) );

			    // attach entities
				for (size_t i = 0; i<node.geometries.size(); ++i) {
					group->addChild( createGeometry(*node.geometries[i]) );
				}
				
				for (size_t i = 0; i<node.controllers.size(); ++i) {
					controllers.push_back(node.controllers[i]);
				}

			    // create hierarchy
			    for (size_t i = 0; i<node.children.size(); ++i)
			    {
				    group->addChild( createNode(*node.children[i]) );
			    }

			    return group;
            }
		}

		scene::group_ptr createVisualScene(const collada_visual_scene& colladaScene)
		{
            scene::group_ptr graphicsModel(new scene::MatrixTransform);
		    for (size_t i = 0; i<colladaScene.nodes.size(); ++i)
		    {
			    // create node hierarchy
			    graphicsModel->addChild( createNode(*colladaScene.nodes[i]) );
		    }

            // attach controllers
            for (size_t i = 0; i<controllers.size(); ++i)
            {
                switch ( controllers[i]->controller->controlElement->getControllerType() )
                {
                    case collada_control_element::SKIN:
                    {
                        scene::Node* rootNode = scene::findNode(*graphicsModel, find_by_id(controllers[i]->skeleton.substr(1)) );
                        
                        if ( scene::Joint* rootJoint = dynamic_cast<scene::Joint*>(rootNode) )
                        {
                            const collada_skin& skin = static_cast<const collada_skin&>(*controllers[i]->controller->controlElement);

                            // enumerate skeleton joints
                            collada_joints::const_input_iterator jointInputIter = std::find_if( skin.joints.inputs.begin(),   
                                                                                                skin.joints.inputs.end(),
                                                                                                boost::bind(&collada_input::semantic, _1) == "JOINT" );
                            if ( jointInputIter == skin.joints.inputs.end() ) {
                                throw collada_error(logger, "Unable to find joint name array for skinned mesh");
                            }

                            collada_joints::const_input_iterator invBindInputIter = std::find_if( skin.joints.inputs.begin(),   
                                                                                                  skin.joints.inputs.end(),
                                                                                                  boost::bind(&collada_input::semantic, _1) == "INV_BIND_MATRIX" );
                            if ( invBindInputIter == skin.joints.inputs.end() ) {
                                throw collada_error(logger, "Unable to find inv bind matrix array for skinned mesh");
                            }

                            skin.numJoints = jointInputIter->source->nameArray.size();
                            for (size_t j = 0; j<skin.numJoints; ++j)
                            {
                                std::string    name = jointInputIter->getName(j);
                                math::Matrix4f pose = invBindInputIter->getMatrix<float, 4, 4>(j);
                                scene::Node*   node = scene::findNode( *rootJoint, find_by_sid(name) );
                                if ( scene::Joint* joint = dynamic_cast<scene::Joint*>(node) ) 
                                {
                                    //scene::Group* tnot = new scene::Group();
                                    //slon::db::loadColladaVisualScene("Data/Models/torus_knot.DAE", *tnot);
                                    //joint->addChild(*tnot);
                                    joint->setId(j);
                                    joint->setInverseBindMatrix(pose);
                                }
                                else {
                                    throw collada_error(logger, "Unable to find joint '" + name + "' for skinned mesh");
                                }
                            }

                            // create skinned mesh
                            graphics::SkinnedMesh* skinnedMesh = createSkinnedMesh(skin, controllers[i]->material);

                            // insert skeleton into scene graph
                            scene::Skeleton* skeleton = new scene::Skeleton();
                            {
                                scene::joint_ptr holder(rootJoint);
                                rootJoint->getParent()->removeChild(rootJoint);
                                rootJoint->getParent()->addChild(skinnedMesh);
                                skeleton->setRootJoint(rootJoint);
                            }
                            skinnedMesh->setSkeleton(skeleton);
                        }
                        else {
                            throw collada_error(logger, "Skinned mesh have no root joint or it has not a joint type.");
                        }

                        break;
                    }

                    default:
                        throw collada_error(logger, "Unsupported controller type");
                }
            }
			
			// compute transforms after scene construction
			scene::TransformVisitor tv(*graphicsModel);
			return graphicsModel;
		}

        animation::animation_ptr createAnimation(collada_animation&    colladaAnimation,
												 scene::Group&         graphicsModel)
        {
            animation::detail::animation_ptr rootAnimation(new animation::detail::Animation);

            for (collada_animation::channel_iterator iter  = colladaAnimation.channels.begin();
                                                     iter != colladaAnimation.channels.end();
                                                     ++iter)
            {
                if ( iter->get_target_transform() != "matrix" ) {
                    throw collada_error(logger, "SlonEngine doesn't support non matrix animation targets");
                }

                // try to find transformation node
                find_transform_visitor visitor( iter->get_target_node() );
                visitor.traverse(graphicsModel);

                if (!visitor.found) {
                    throw collada_error(logger, "Can't find animation target transformation node");
                }

                animation::detail::AnimationTrack* track = 0;
                {
                    collada_animation::sampler_iterator samplerIt = colladaAnimation.get_sampler( iter->source.substr(1) );
                    if ( samplerIt == colladaAnimation.samplers.end() ) {
                        throw collada_error(logger, "Can't find animation sampler '" + iter->source.substr(1) + "'");
                    }

                    collada_animation::source_iterator inputSourceIt  = colladaAnimation.sources.end();
                    collada_animation::source_iterator outputSourceIt = colladaAnimation.sources.end();
                    for (collada_sampler::input_iterator inputIt  = samplerIt->inputs.begin();
                                                         inputIt != samplerIt->inputs.end();
                                                         ++inputIt)
                    {
                        if (inputIt->semantic == "INPUT") {
                            inputSourceIt = colladaAnimation.get_source(inputIt->sourceId);
                        }
                        else if (inputIt->semantic == "OUTPUT") {
                            outputSourceIt = colladaAnimation.get_source(inputIt->sourceId);
                        }
                    }

                    if ( inputSourceIt == colladaAnimation.sources.end() || outputSourceIt == colladaAnimation.sources.end() ) {
                        throw collada_error(logger, "Can't find animation input or output source");
                    }
                    else if ( inputSourceIt->floatArray.size() * 16 != outputSourceIt->floatArray.size() ) {
                        throw collada_error(logger, "Animation input and output sources are incompatible");
                    }

                    typedef animation::detail::AnimationTrack::frame            frame;
                    typedef sgl::vector<frame, sgl::aligned_allocator<frame> >  frame_vector;

                    frame_vector frames( inputSourceIt->floatArray.size() );
                    for (size_t i = 0; i<frames.size(); ++i)
                    {
                        math::Matrix4f transform = outputSourceIt->getMatrix<float, 4, 4>(i);

                        frames[i].time        = inputSourceIt->floatArray[i];
                        frames[i].rotation    = math::from_matrix(transform);
                        frames[i].translation = math::get_translation(transform);
                    }

                    animation::detail::AnimationTrack::DESC desc;
                    desc.numFrames = frames.size();
                    desc.frames    = &frames.front();
                    track = new animation::detail::AnimationTrack(desc);
                }

                animation::detail::AnimationController* controller = new animation::detail::AnimationController();
                controller->setTrack(track);

                animation::detail::Animation* animation = new animation::detail::Animation();
                {
                    animation->setName(colladaAnimation.id + "." + iter->target);
                    animation->setTarget(visitor.found);
                    animation->setController(controller);
                }

                rootAnimation->addAnimation(animation);
            }
        
            // build animation hierarchy
            for (collada_animation::animation_iterator iter  = colladaAnimation.animations.begin();
                                                       iter != colladaAnimation.animations.end();
                                                       ++iter)
            {
				animation::animation_ptr childAnimation = createAnimation(*iter, graphicsModel);
                rootAnimation->addAnimation( childAnimation.get() );
            }

            return rootAnimation;
        }

	private:
		const ColladaDocument& document;

        // storage for local data
        mesh_map            meshMap;
        controller_vector   controllers;
	};

#ifdef SLON_ENGINE_USE_PHYSICS
    class PhysicsSceneBuilder
    {
    public:
        PhysicsSceneBuilder( const ColladaDocument&     document_,
                             physics::DynamicsWorld&    dynamicsWorld_) :
            document(document_),
            dynamicsWorld(dynamicsWorld_)
        {
        }

        physics::CollisionShape* createGeometryCollisionShape(const collada_geometry_shape& colladaGeometryShape)
        {
            const collada_geometry& colladaGeometry = *colladaGeometryShape.geometryInstance.geometry;
            switch ( colladaGeometry.getGeometryType() )
            {
                case collada_geometry::CONVEX_MESH:
                {
                    const collada_convex_mesh& colladaConvexMesh
                        = static_cast<const collada_convex_mesh&>(colladaGeometry);

                    assert( colladaConvexMesh.mesh && "<convex_hull> must have mesh reference" );
                    const collada_mesh& colladaMesh = *colladaConvexMesh.mesh;

                    // search for vertices
                    physics::ConvexShape* collisionShape = new physics::ConvexShape();
                    for (size_t i = 0; i<colladaMesh.vertices.inputs.size(); ++i)
                    {
                        const collada_input&  colladaInput  = colladaMesh.vertices.inputs[i];
                        const collada_source& colladaSource = *colladaInput.source;

                        if ( colladaInput.attributeIndex == sgl::VertexLayout::VERTEX )
                        {
                            if (colladaSource.stride < 3) {
                                throw collada_error(logger, "Unable to build collision shape using 2-dimensional vertices");
                            }

                            collisionShape->buildConvexHull( vector_composer<physics::real, 3>( colladaSource.floatArray.begin(), colladaSource.stride ),
                                                             vector_composer<physics::real, 3>( colladaSource.floatArray.end(), colladaSource.stride ) );
                            break;
                        }
                    }

                    if ( collisionShape->vertices.empty() ) {
                        throw collada_error(logger, "Unable to build convex hull");
                    }

                    return collisionShape;
                }

                case collada_geometry::MESH:
                {
                    const collada_mesh& colladaMesh = static_cast<const collada_mesh&>(colladaGeometry);

                    // search for vertices
                    physics::TriangleMeshShape* collisionShape = new physics::TriangleMeshShape();
                    size_t                      vertexInput    = 0;
                    for (size_t i = 0; i<colladaMesh.vertices.inputs.size(); ++i)
                    {
                        const collada_input&  colladaInput  = colladaMesh.vertices.inputs[i];
                        const collada_source& colladaSource = *colladaInput.source;

                        if ( colladaInput.attributeIndex == sgl::VertexLayout::VERTEX )
                        {
                            if (colladaSource.stride < 3) {
                                throw collada_error(logger, "Unable to build collision shape using 2-dimensional vertices");
                            }

                            std::copy( vector_composer<physics::real, 3>( colladaSource.floatArray.begin(), colladaSource.stride ),
                                       vector_composer<physics::real, 3>( colladaSource.floatArray.end(), colladaSource.stride ),
                                       std::back_inserter(collisionShape->vertices) );

                            vertexInput = i;
                            break;
                        }
                    }

                    // indices
                    for (size_t i = 0; i < colladaMesh.primitives.size(); ++i)
                    {
                        const collada_primitives& colladaPrimitives = colladaMesh.primitives[i];
                        if (colladaPrimitives.primType != sgl::TRIANGLES) {
                            throw collada_error(logger, "Unable to build collision shape. Only triangle meshes are supported");
                        }

                        std::copy( colladaPrimitives.inputIndices[vertexInput].begin(),
                                   colladaPrimitives.inputIndices[vertexInput].end(),
                                   std::back_inserter(collisionShape->indices) );
                    }

                    return collisionShape;
                }

                default:
                    throw collada_error(logger, "Unable to create collision shape. Invalid geometry type");
                    break;
            }

            return 0;
        }

        physics::CollisionShape* createCollisionShape(const collada_shape& colladaShape)
        {
            switch ( colladaShape.geometry->getShapeType() )
            {
                case collada_shape_geometry::GEOMETRY:
                {
                    const collada_geometry_shape& colladaGeometryShape = static_cast<const collada_geometry_shape&>(*colladaShape.geometry);
                    return createGeometryCollisionShape(colladaGeometryShape);
                }

                case collada_shape_geometry::SPHERE:
                {
                    const collada_sphere_shape& colladaSphereShape = static_cast<const collada_sphere_shape&>(*colladaShape.geometry);
                    return new physics::SphereShape(colladaSphereShape.radius);
                }

                case collada_shape_geometry::BOX:
                {
                    const collada_box_shape& colladaBoxShape = static_cast<const collada_box_shape&>(*colladaShape.geometry);
                    return new physics::BoxShape(math::Vector3r(colladaBoxShape.halfExtents));
                }

                case collada_shape_geometry::TAPPERED_CYLINDER:
                {
                    const collada_tapered_cylinder_shape& colladaCylShape =
						static_cast<const collada_tapered_cylinder_shape&>(*colladaShape.geometry);
					
                    if (colladaCylShape.upperRadius[1] == 0) 
                    {
                        // cone Y
                        return new physics::ConeShape(colladaCylShape.lowerRadius[0], colladaCylShape.height * 2.0f);
                    }
                    else if (colladaCylShape.upperRadius[0] == colladaCylShape.upperRadius[1]
                             && colladaCylShape.upperRadius[0] == colladaCylShape.lowerRadius[0]
                             && colladaCylShape.lowerRadius[0] == colladaCylShape.lowerRadius[1]) 
                    {
                        // capsule Y
                        return new physics::CapsuleShape(colladaCylShape.lowerRadius[0], colladaCylShape.height * 2.0f);
                    }
                    else {
                        throw collada_error(logger, "Unable to create collision shape. Unsupported geometry type.");
                    }
                }

                default:
                    throw collada_error(logger, "Unable to create collision shape. Invalid geometry type.");
                    break;
            }
        }

        physics::Constraint* createRigidConstraintInstance( const collada_instance_rigid_constraint& colladaRigidConstraintInstance,
                                                            physics::PhysicsModel&                   sceneModel )
        {
            assert( colladaRigidConstraintInstance.element && "Constraint instance must point to some constraint." );

            collada_rigid_constraint& colladaConstraint = *colladaRigidConstraintInstance.element;

            // create constraint
            physics::Constraint::state_desc desc(colladaConstraint.sid);

            desc.frames[0] = math::Matrix4r(colladaConstraint.refAttachment.transform);
            desc.frames[1] = math::Matrix4r(colladaConstraint.attachment.transform);

            desc.rigidBodies[0] = sceneModel.findRigidBody(colladaConstraint.refAttachment.rigidBody->sid)->get();
            desc.rigidBodies[1] = sceneModel.findRigidBody(colladaConstraint.attachment.rigidBody->sid)->get();

            desc.linearLimits[0] = math::Vector3r(colladaConstraint.limits.linear[0]);
            desc.linearLimits[1] = math::Vector3r(colladaConstraint.limits.linear[1]);

            for (int i = 0; i<3; ++i)
            {
                desc.angularLimits[0][i] = math::deg_to_rad(colladaConstraint.limits.swingConeTwist[0][i]);
                desc.angularLimits[1][i] = math::deg_to_rad(colladaConstraint.limits.swingConeTwist[1][i]);
            }

            physics::Constraint* constraint = dynamicsWorld.createConstraint(desc);
            sceneModel.addConstraint(constraint);

            return constraint;
        }

        physics::RigidBody* createRigidBodyInstance( const collada_instance_rigid_body&  colladaRigidBodyInstance,
                                                     physics::PhysicsModel&              sceneModel )
        {
            using namespace physics;

            collada_rigid_body& colladaRigidBody = *colladaRigidBodyInstance.element;

            RigidBody::state_desc desc;
            desc.target = colladaRigidBodyInstance.target.substr(1);
            desc.name   = colladaRigidBody.sid;
            desc.type   = colladaRigidBody.dynamic ? RigidBody::DT_DYNAMIC : RigidBody::DT_STATIC;

            if (colladaRigidBodyInstance.mass) {
                desc.mass = colladaRigidBodyInstance.mass;
            }
            else {
                desc.mass = colladaRigidBody.mass;
            }

            if (colladaRigidBodyInstance.massFrame) {
                desc.transform = colladaRigidBodyInstance.massFrame.value.transform;
            }
            else if (colladaRigidBody.massFrame) {
                desc.transform = colladaRigidBody.massFrame.value.transform;
            }
            else {
                desc.transform = math::make_identity<float, 4>();
            }

            math::Matrix4f invMassFrame = math::invert(desc.transform);
            if ( colladaRigidBody.shapes.size() > 1
                || !math::equal(invMassFrame * colladaRigidBody.shapes[0]->transform, math::make_identity<float, 4>(), 0.01f) )
            {
                physics::CompoundShape* compoundShape = new physics::CompoundShape();
                for (size_t i = 0; i<colladaRigidBody.shapes.size(); ++i)
                {
                    compoundShape->addShape( math::Matrix4r(invMassFrame * colladaRigidBody.shapes[i]->transform),
                                             createCollisionShape(*colladaRigidBody.shapes[i]) );
                }

                desc.collisionShape.reset(compoundShape);
            }
            else {
                desc.collisionShape.reset( createCollisionShape(*colladaRigidBody.shapes[0]) );
            }
            // material
            // desc.material   = *createPhysicsMaterialInstance(*colladaRigidBody.materialInstance);

            // attach shapes

            physics::RigidBody* rigidBody = dynamicsWorld.createRigidBody(desc);
            sceneModel.addRigidBody(rigidBody);

            return rigidBody;
        }

        physics::physics_model_ptr createPhysicsScene(const collada_physics_scene& colladaScene)
        {
			physics::physics_model_ptr physicsModel(new physics::PhysicsModel);
			physicsModel->setName(colladaScene.id);

			for (size_t k = 0; k<colladaScene.physicsModelInstances.size(); ++k)
			{
				const collada_instance_physics_model& colladaModel = colladaScene.physicsModelInstances[k];

				// add rigid bodies
				for (size_t i = 0; i<colladaModel.rigidBodyInstances.size(); ++i)
				{
					try
					{
						physics::RigidBody* rigidBody = createRigidBodyInstance(colladaModel.rigidBodyInstances[i], *physicsModel);
					}
					catch(collada_error&)
					{
						std::string sid = colladaModel.rigidBodyInstances[i].element->sid;
						logger << log::S_ERROR << "Unable to create rigid body: " + sid << std::endl;
					}
				}

				// add constraints
				for (size_t i = 0; i<colladaModel.rigidConstraintInstances.size(); ++i)
				{
					createRigidConstraintInstance(colladaModel.rigidConstraintInstances[i], *physicsModel);
				}
			}

			return physicsModel;
        }

    private:
        const ColladaDocument&  document;
        physics::DynamicsWorld& dynamicsWorld;
    };
#endif // SLON_ENGINE_USE_PHYSICS

	class ColladaSceneBuilder
	{
	private:
		typedef std::set<std::string>									string_set;
		typedef std::map<const scene::Geode*, collada_geometry_ptr>		geode_geometry_map;
		typedef std::map<const graphics::Effect*, collada_material_ptr>	effect_material_map;

		std::string getUniqueId(const std::string& name)
		{
			int counter = 0;
			std::string id = name;
			string_set::iterator iter = nodeIds.find(id);
			while ( iter != nodeIds.end() )
			{
				++counter;
				id = name + boost::lexical_cast<std::string>(counter);
				iter = nodeIds.find(id);
			}

			nodeIds.insert(id);
			return id;
		}

		const graphics::Mesh* getMesh(const scene::Geode* geode)
		{
			if ( const graphics::StaticMesh* mesh = dynamic_cast<const graphics::StaticMesh*>(geode) ) {
				return mesh->getMesh();
			}
			else if ( const graphics::SkinnedMesh* mesh = dynamic_cast<const graphics::SkinnedMesh*>(geode) ) {
				return mesh->getMesh();
			}

			return 0;
		}

	public:
		collada_geometry_ptr createGeometry(const scene::Geode* geode)
		{
			geode_geometry_map::iterator geomIter = geometries.find(geode);
			if ( geomIter != geometries.end() ) {
				return geomIter->second;
			}

			// attach renderables
			collada_geometry_ptr geometry;
			if ( const graphics::Mesh* mesh = getMesh(geode) )
			{
				collada_mesh_ptr							cMesh(new collada_mesh);
				std::vector<collada_input>					inputs;
				std::vector<collada_primitives::int_vector>	inputIndices;

				if ( const graphics::MeshData* meshData = mesh->getData() ) 
				{
					unsigned offsetCounter = 0;
					for (size_t i = 1; i<graphics::MeshData::MAX_NUM_ATTRIBUTES; ++i)
					{
						const graphics::MeshData::attribute_array& attributes = meshData->getAttributes(i);
						if (attributes.count > 0)
						{
							collada_source_ptr source(new collada_source);
							source->floatArray.resize( attributes.count * attributes.attribute_size() / sizeof(float) );
							std::copy( (const float*)attributes.data.get(), 
									   (const float*)attributes.data.get() + source->floatArray.size(), 
									   source->floatArray.begin() );
							source->id = getUniqueId(attributes.name);
							cMesh->sources.push_back(source);

							collada_input input;
							input.source         = source;
							input.sourceId       = source->id;
							input.semantic       = toupper(attributes.name);
							input.attributeIndex = i;
							input.attributeName  = attributes.name;
							if (attributes.name == "position")
							{
								input.offset = cMesh->vertices.inputs.size();
								cMesh->vertices.inputs.push_back(input);
							}
							input.offset = offsetCounter++;
							inputs.push_back(input);

							const graphics::MeshData::indices_array& indices = meshData->getIndices(i);
							collada_primitives::int_vector indicesArray(indices.indices.get(), indices.indices.get() + indices.count);
							inputIndices.push_back(indicesArray);
						}
					}

					for (graphics::Mesh::subset_const_iterator iter  = mesh->firstSubset();
															   iter != mesh->endSubset();
															   ++iter)
					{
						collada_material_ptr material = createMaterial( (*iter)->getEffect() );
					
						collada_primitives primitives;
						if ( const graphics::Mesh::indexed_subset* subset = dynamic_cast<const graphics::Mesh::indexed_subset*>( (*iter).get() ) )
						{
							primitives.primType     = subset->getPrimitiveType();
							primitives.material     = material->id;
							primitives.inputs       = inputs;

							primitives.inputIndices.resize( subset->getNumIndices() );
							std::copy( inputIndices.begin() + subset->getStartIndex(),
									   inputIndices.begin() + subset->getStartIndex() + subset->getNumIndices(),
									   primitives.inputIndices.begin() );
						}
						else if ( const graphics::Mesh::plain_subset* subset = dynamic_cast<const graphics::Mesh::plain_subset*>( (*iter).get() ) )
						{
							assert(!"supported");
							/*
							primitives.primType = subset->getPrimitiveType();
							primitives.material = material->id;
							primitives.inputs   = inputs;
							*/
						}

						cMesh->primitives.push_back(primitives);
					}

					geometry = cMesh;
				}
			}
			
			geometries.insert( geode_geometry_map::value_type(geode, geometry) );
			return geometry;
		}

		collada_material_ptr createMaterial(const graphics::Effect* effect)
		{
			effect_material_map::iterator matIter = materials.find(effect);
			if ( matIter != materials.end() ) {
				return matIter->second;
			}

			collada_material_ptr material(new collada_material);
			material->id = getUniqueId("material");
			material->effect.element.reset(new collada_effect);
			material->effect.target = getUniqueId("effect");
				
			collada_effect_ptr cEffect = material->effect.element;
			cEffect->id = material->effect.target;

			if ( const graphics::LightingEffect* lightingEffect = dynamic_cast<const graphics::LightingEffect*>(effect) )
			{
				const graphics::abstract_parameter_binding* parameter = 0;
				collada_phong_technique_ptr					technique(new collada_phong_technique);
				
				parameter = lightingEffect->getParameter( hash_string("materialDiffuseSpecular") );
				if (const graphics::binding_vec4f* diffuseSpecular = graphics::cast_binding<math::Vector4f>(parameter) ) 
				{					
					technique->ambient.setColor(diffuseSpecular->value());
					technique->diffuse.setColor(diffuseSpecular->value());
					technique->emission.setColor( math::Vector4f(0.0f, 0.0f, 0.0f, 0.0f) );
					technique->specular.setColor( math::Vector4f(1.0f, 1.0f, 1.0f, 1.0f) );
				}
				
				parameter = lightingEffect->getParameter( hash_string("materialShininess") );
				if (const graphics::binding_float* shininess = graphics::cast_binding<float>(parameter) ) {					
					technique->shininess = shininess->value();
				}

				parameter = lightingEffect->getParameter( hash_string("opacity") );
				if (const graphics::binding_float* opacity = graphics::cast_binding<float>(parameter) ) {					
					technique->opacity = opacity->value();
				}

				parameter = lightingEffect->getParameter( hash_string("diffuseSpecularMap") );
				if (const graphics::binding_tex_2d* diffuseSpecular = graphics::cast_binding<sgl::Texture2D>(parameter) ) 
				{
					/*
					collada_texture texture;
					texture.texcoord = diffuseSpecular->value();
					technique->diffuse.setColor(diffuseSpecular->value());
					technique->emission.setColor( math::Vector4f(0.0f, 0.0f, 0.0f, 0.0f) );
					technique->specular.setColor( math::Vector4f(1.0f, 1.0f, 1.0f, 1.0f) );
					*/
				}

				cEffect->techniques.push_back(technique);
			}
		}

		collada_instance_material_ptr createMaterialInstance(const graphics::Effect* effect)
		{
			collada_instance_material_ptr materialInstance(new collada_instance_material);
			materialInstance->element = createMaterial(effect);
			materialInstance->symbol  = materialInstance->element->id;
			materialInstance->target  = materialInstance->element->id;
			return materialInstance;
		}

		collada_bind_material_ptr createBindMaterial(const scene::Geode* geode)
		{
			scene::CullVisitor visitor;
			const_cast<scene::Geode*>(geode)->accept(visitor);

			collada_bind_material_ptr bindMaterial(new collada_bind_material);
			for (scene::CullVisitor::renderable_iterator iter  = visitor.beginRenderable();
														 iter != visitor.endRenderable();
														 ++iter)
			{
				collada_instance_material_ptr material = createMaterialInstance( (*iter)->getEffect() );
				bindMaterial->materials.push_back(*material);
			}
		}

		collada_instance_geometry_ptr createGeometryInstance(const scene::Geode* geode)
		{
			collada_instance_geometry_ptr geometry(new collada_instance_geometry);
			geometry->geometry = createGeometry(geode);
			geometry->material = createBindMaterial(geode);
			return geometry;
		}

		void attachEntity(collada_node& cNode, const scene::Entity* entity)
		{
			switch ( entity->getNodeType() )
			{
			case scene::Entity::GEODE:
				cNode.geometries.push_back( createGeometryInstance( static_cast<const scene::Geode*>(entity) ) );
				break;

			default:
				logger << log::S_WARNING << "Node '" << entity->getName() << "' will not be exported. Unsupported type.";
				break;
			}
		}

		collada_node_ptr createNode(const scene::Node* node)
		{
			collada_node_ptr cNode(new collada_node);
            if ( const scene::Entity* entity = dynamic_cast<const scene::Entity*>(node) )
            {
				cNode->id   = getUniqueId( node->getName().str() ); 
				cNode->name = node->getName().str();
				attachEntity(*cNode, entity);
            }
            else
            {
				cNode->id   = getUniqueId( node->getName().str() ); 
				cNode->name = node->getName().str();

				// setup transform
				if ( const scene::Transform* transform = dynamic_cast<const scene::Transform*>(node) ) 
				{
					cNode->transform = transform->getTransform();
					if ( dynamic_cast<const scene::Joint*>(transform) ) {
						cNode->type = collada_node::JOINT;
					}
				}

				// add children
				if ( const scene::Group* group = dynamic_cast<const scene::Group*>(node) )
				{
					for ( const scene::Node* i = group->getChild(); i; i = i->getRight() ) 
					{
						if ( const scene::Entity* entity = dynamic_cast<const scene::Entity*>(i) ) {
							attachEntity(*cNode, entity);
						}
						else {
							cNode->children.push_back( createNode(i) );
						}
					}
				}
            }

			return cNode;
		}

		collada_visual_scene_ptr createVisualScene(ColladaDocument& document, const scene::Group& graphicsModel)
		{
			collada_visual_scene_ptr visualScene(new collada_visual_scene);
			for ( const scene::Node* i = graphicsModel.getChild(); i; i = i->getRight() )
			{
				// create node hierarchy
				visualScene->nodes.push_back( createNode(i) );
			}
			document.libraryVisualScenes.elements.insert( std::make_pair(visualScene->id, visualScene) );

			for (geode_geometry_map::iterator iter  = geometries.begin();
											  iter != geometries.end();
											  ++iter)
			{
                document.libraryGeometries.elements.insert( std::make_pair(iter->second->id, iter->second) );
			}

			for (effect_material_map::iterator iter  = materials.begin();
											   iter != materials.end();
											   ++iter)
			{
				collada_material_ptr material = iter->second;
				document.libraryMaterials.elements.insert( std::make_pair(material->id, material) );
                document.libraryEffects.elements.insert( std::make_pair(material->effect->id, material->effect.element) );
			}

			return visualScene;
		}

	private:
		string_set				nodeIds;
		geode_geometry_map		geometries;
		effect_material_map		materials;
	};

} // anonymous namespace

namespace slon {
namespace database {
	
void collada_scene::serialize( ColladaDocument&  document, 
                               xmlpp::element&   elem, 
                               xmlpp::s_state    state )
{
	collada_serializer serializer;
    serializer &= xmlpp::make_nvp( "instance_visual_scene",		xmlpp::as_element_set(visualScenes) );
	serializer &= xmlpp::make_nvp( "instance_physics_scene",	xmlpp::as_element_set(physicsScenes) );
    serializer.serialize(document, elem, state);
}

void ColladaDocument::set_file_source(const std::string& fileName)
{
	using namespace filesystem;
	logger << log::S_NOTICE << "Loading COLLADA file: " << fileName << std::endl;

    filesystem::file_ptr file( asFile( currentFileSystemManager().getNode(fileName.c_str()) ) );
	if ( file && file->open(filesystem::File::in) ) 
	{
		std::string source;
		source.resize( (size_t)file->size() );
		file->read( &source[0], file->size() );
		base_type::set_source( source.size(), source.c_str() );
	}
	else {
		throw slon_error(logger, "Can't open file: " + fileName);
	}
}

library_ptr ColladaLoader::load(std::istream& stream)
{
	typedef collada_library_visual_scenes::element_set	visual_scene_set;
	typedef collada_library_animations::element_set		animation_set;
#ifdef SLON_ENGINE_USE_PHYSICS	
	typedef collada_library_physics_scenes::element_set	physics_scene_set;
#endif
		
	// read file content
	std::ostringstream buffer;
	buffer << stream.rdbuf();

	ColladaDocument document;
	document.set_source( buffer.str().size(), buffer.str().c_str() );

	detail::library_ptr library(new detail::Library);
	scene::group_ptr	root(new scene::Group);
	SceneBuilder	    visualBuilder(document);
	for ( visual_scene_set::iterator iter  = document.libraryVisualScenes.elements.begin();
									 iter != document.libraryVisualScenes.elements.end();
									 ++iter )
	{
		scene::group_ptr visualScene = visualBuilder.createVisualScene(*iter->second);
		root->addChild(visualScene.get());
        library->visualScenes.push_back( Library::key_visual_scene_pair(visualScene->getName().str(), visualScene) );
	}

#ifdef SLON_ENGINE_USE_PHYSICS				
	PhysicsSceneBuilder physicsBuilder(document, *physics::currentPhysicsManager().getDynamicsWorld());
	for ( physics_scene_set::iterator iter  = document.libraryPhysicsScenes.elements.begin();
										iter != document.libraryPhysicsScenes.elements.end();
										++iter )
	{
		physics::physics_model_ptr physicsScene = physicsBuilder.createPhysicsScene(*iter->second);
		library->physicsScenes.push_back( Library::key_physics_scene_pair(physicsScene->getName(), physicsScene) );
	}
#endif

	for ( animation_set::iterator iter  = document.libraryAnimations.elements.begin();
								  iter != document.libraryAnimations.elements.end();
								  ++iter )
	{
		animation::animation_ptr animation = visualBuilder.createAnimation(*iter->second, *root);
		library->animations.push_back( Library::key_animation_pair(animation->getName(), animation) );
	}

	return library;
}

} // namespace database
} // namespace slon

void ColladaDocument::on_load()
{
    xmlpp::element_iterator colladaIter = first_child_element("COLLADA");
    if ( !colladaIter ) {
        throw collada_error(logger, "Couldn't find <COLLADA> xmlpp::element in the collada file");
    }

	// wee need 
	collada_serializer serializer;
	serializer &= xmlpp::make_nvp( "library_images",			xmlpp::as_element(libraryImages) );
	serializer.load(*this, *colladaIter);

	serializer.clear();
	serializer &= xmlpp::make_nvp( "library_effects",			xmlpp::as_element(libraryEffects) );
	serializer.load(*this, *colladaIter);

	serializer.clear();
	serializer &= xmlpp::make_nvp( "library_materials",			xmlpp::as_element(libraryMaterials) );
	serializer &= xmlpp::make_nvp( "library_geometries",		xmlpp::as_element(libraryGeometries) );
	serializer &= xmlpp::make_nvp( "library_visual_scenes",		xmlpp::as_element(libraryVisualScenes) );
	serializer &= xmlpp::make_nvp( "library_controllers",		xmlpp::as_element(libraryControllers) );
	serializer &= xmlpp::make_nvp( "library_animations",		xmlpp::as_element(libraryAnimations) );
	serializer &= xmlpp::make_nvp( "library_physics_scenes",	xmlpp::as_element(libraryPhysicsScenes) );
	serializer &= xmlpp::make_nvp( "library_physics_models",	xmlpp::as_element(libraryPhysicsModels) );
	serializer &= xmlpp::make_nvp( "library_physics_materials",	xmlpp::as_element(libraryPhysicsMaterials) );
	serializer &= xmlpp::make_nvp( "scene",						xmlpp::as_element(scene) );
	serializer.load(*this, *colladaIter);
}

void ColladaDocument::on_save()
{
	clear();

	xmlpp::element colladaElem("COLLADA");
	xmlpp::add_child(*this, colladaElem);

	collada_serializer serializer;
	serializer &= xmlpp::make_nvp( "library_images",			xmlpp::as_element(libraryImages) );
	serializer &= xmlpp::make_nvp( "library_effects",			xmlpp::as_element(libraryEffects) );
	serializer &= xmlpp::make_nvp( "library_materials",			xmlpp::as_element(libraryMaterials) );
	serializer &= xmlpp::make_nvp( "library_geometries",		xmlpp::as_element(libraryGeometries) );
	serializer &= xmlpp::make_nvp( "library_visual_scenes",		xmlpp::as_element(libraryVisualScenes) );
	serializer &= xmlpp::make_nvp( "library_controllers",		xmlpp::as_element(libraryControllers) );
	serializer &= xmlpp::make_nvp( "library_animations",		xmlpp::as_element(libraryAnimations) );
	serializer &= xmlpp::make_nvp( "library_physics_scenes",	xmlpp::as_element(libraryPhysicsScenes) );
	serializer &= xmlpp::make_nvp( "library_physics_models",	xmlpp::as_element(libraryPhysicsModels) );
	serializer &= xmlpp::make_nvp( "library_physics_materials",	xmlpp::as_element(libraryPhysicsMaterials) );
	serializer &= xmlpp::make_nvp( "scene",						xmlpp::as_element(scene) );
	serializer.save(*this, colladaElem);
}

template<> collada_library<collada_image>&				ColladaDocument::get_library() { return libraryImages; }
template<> collada_library<collada_material>&			ColladaDocument::get_library() { return libraryMaterials; }
template<> collada_library<collada_effect>&				ColladaDocument::get_library() { return libraryEffects; }
template<> collada_library<collada_geometry>&			ColladaDocument::get_library() { return libraryGeometries; }
template<> collada_library<collada_visual_scene>&		ColladaDocument::get_library() { return libraryVisualScenes; }
template<> collada_library<collada_controller>&			ColladaDocument::get_library() { return libraryControllers; }
template<> collada_library<collada_physics_scene>&		ColladaDocument::get_library() { return libraryPhysicsScenes; }
template<> collada_library<collada_physics_model>&		ColladaDocument::get_library() { return libraryPhysicsModels; }
template<> collada_library<collada_physics_material>&	ColladaDocument::get_library() { return libraryPhysicsMaterials; }