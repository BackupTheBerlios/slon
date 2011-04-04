#include "stdafx.h"
#include "Database/Proprietary/SXMLSerializer.h"
#include "Graphics/Renderable/SkinnedMesh.h"
#include "Graphics/Renderable/StaticMesh.h"
#include "Log/Logger.h"
#include "Scene/Skeleton.h"
#include "Scene/Light/PointLight.h"
#ifdef SLON_ENGINE_USE_PHYSICS
#   include "Physics/PhysicsModel.h"
#endif

__DEFINE_LOGGER__("database.SXML")

namespace 
{
    using namespace slon;
        
    typedef xmlpp::generic_serializer<xmlpp::document>  xmlpp_serializer;
    typedef xmlpp::generic_loader<xmlpp::document>      xmlpp_loader;
    typedef xmlpp::generic_saver<xmlpp::document>       xmlpp_saver;

    template<typename T>
    struct group_serializer
    {
        typedef xmlpp::element          xmlpp_holder_type;
        typedef boost::intrusive_ptr<T> node_ptr;

        void load(const xmlpp::document& doc, const xmlpp::element& elem, scene::node_ptr& node)
        {
            using namespace xmlpp;

            node_ptr typedNode(new T);
            typedNode->serialize( xmlpp_serializer(doc, elem, xmlpp::LOAD) );
            node = typedNode;

            scene::node_ptr child;
            xmlpp_loader    l;
            l >>= make_nvp("Group",              as_element_ex( child, group_serializer<scene::Group>() ) );
            l >>= make_nvp("MatrixTransform",    as_element_ex( child, group_serializer<scene::MatrixTransform>() ) );
            //l &= make_nvp("Skeleton",           as_element( node_serializer<scene::Skeleton>(node) ) );
            //l &= make_nvp("Joint",              as_element( group_serializer<scene::Joint>(node) ) );
            //l &= make_nvp("PointLight",         as_element( node_serializer<scene::PointLight>(node) ) );
            //l &= make_nvp("StaticMesh",         as_element( node_serializer<graphics::StaticMesh>(node) ) );
            //l &= make_nvp("SkinnedMesh",        as_element( node_serializer<graphics::SkinnedMesh>(node) ) );
            l.load(doc, elem);

            if (child) {
                typedNode->addChild(child.get());
            }
        }

        void save(xmlpp::document& doc, xmlpp::element& elem, const scene::node_ptr& node) const
        {
            using namespace xmlpp;

            node_ptr typedNode( static_cast<T*>(node.get()) );
            typedNode->serialize( xmlpp_serializer(doc, elem, xmlpp::SAVE) );
            for ( scene::Node* child = typedNode->getChild(); child != 0; child = child->getRight() )
            {
                xmlpp_saver s;
                //s &= make_nvp("Joint",              as_element<scene::Joint>( group_serializer<scene::Joint>(node, child) ) );
                s <<= make_nvp("MatrixTransform",    as_element_ex( scene::node_ptr(child), group_serializer<scene::MatrixTransform>() ) );
                s <<= make_nvp("Group",              as_element_ex( scene::node_ptr(child), group_serializer<scene::Group>() ) );
                //s &= make_nvp("Skeleton",           as_element<scene::Skeleton>( node_serializer<scene::Skeleton>(node, child) ) );
                //s &= make_nvp("PointLight",         as_element<scene::PointLight>( node_serializer<scene::PointLight>(node, child) ) );
                //s &= make_nvp("StaticMesh",         as_element<graphics::StaticMesh>( node_serializer<graphics::StaticMesh>(node, child) ) );
                //s &= make_nvp("SkinnedMesh",        as_element<graphics::SkinnedMesh>( node_serializer<graphics::SkinnedMesh>(node, child) ) );
                s.save(doc, elem);
            }
        }

        bool valid(const scene::node_ptr& node, xmlpp::s_state) const { return (bool)dynamic_cast<T*>(node.get()); }
    };

    struct mesh_serialization_policy
    {
        void load(const xmlpp::document& doc, const xmlpp::element& elem, boost::intrusive_ptr<graphics::Mesh>& obj)
        {
        }

        void save(xmlpp::document& doc, xmlpp::element& elem, const boost::intrusive_ptr<graphics::Mesh>& obj) const
        {
        }

        bool valid(const boost::intrusive_ptr<graphics::Mesh>& obj, xmlpp::s_state) const { return (bool)obj; }
    };

} // anonymous namespace

namespace slon {
namespace database {

void SXMLSerializer::serialize(xmlpp::document& doc, 
                               xmlpp::element&  elem, 
                               xmlpp::s_state   state)
{
    using namespace xmlpp;

    xmlpp_serializer s;
    s &= make_nvp("slon", make_nvp("visual_scenes", as_element(boost::bind(&SXMLSerializer::serializeVisualScenes, this, _1, _2, _3)))
                          #ifdef SLON_ENGINE_USE_PHYSICS
                          & make_nvp("physics_scenes", as_element(boost::bind(&SXMLSerializer::serializePhysicsScenes, this, _1, _2, _3)))
                          #endif 
                  );
    s.serialize(doc, elem, state);
}

void SXMLSerializer::serializeVisualScenes(xmlpp::document& doc, 
                                           xmlpp::element&  elem, 
                                           xmlpp::s_state   state)
{
    using namespace xmlpp;

    xmlpp_serializer s;
    s &= make_nvp( "meshes", make_nvp("Mesh", as_element_set_ex(meshes, mesh_serialization_policy())) );
    s &= make_nvp( "scenes", make_nvp("Group", as_element_set_ex(visualScenes, group_serializer<scene::Group>())) );
    s.serialize(doc, elem, state);
}

#ifdef SLON_ENGINE_USE_PHYSICS
void SXMLSerializer::serializePhysicsScenes(xmlpp::document& doc, 
                                            xmlpp::element&  elem, 
                                            xmlpp::s_state   state)
{
}
#endif // SLON_ENGINE_USE_PHYSICS

} // namespace database
} // namespace slon
