#ifndef __SLON_ENGINE_DATABASE_PROPRIETARY_SXML_SERIALIZER_H__
#define __SLON_ENGINE_DATABASE_PROPRIETARY_SXML_SERIALIZER_H__

#include <xml++/serialization/serialization.hpp>
#include "../Forward.h"

namespace slon {
namespace database {

class SXMLSerializer
{
public:
    typedef std::vector<graphics::mesh_ptr>         mesh_vector;
    typedef std::vector<scene::node_ptr>            visual_scene_vector;
#ifdef SLON_ENGINE_USE_PHYSICS
    typedef std::vector<physics::physics_model_ptr> physics_scene_vector;
#endif

    SXMLSerializer(xmlpp::document& doc,
                   xmlpp::s_state   state);

public:
    virtual void serialize(xmlpp::document& doc, 
                           xmlpp::element&  elem, 
                           xmlpp::s_state   state);

protected:
    virtual void serializeEffects(xmlpp::document& doc, 
                                  xmlpp::element&  elem, 
                                  xmlpp::s_state   state);

    virtual void serializeMaterials(xmlpp::document& doc, 
                                    xmlpp::element&  elem, 
                                    xmlpp::s_state   state);

    virtual void serializeTextures(xmlpp::document& doc, 
                                   xmlpp::element&  elem, 
                                   xmlpp::s_state   state);
#ifdef SLON_ENGINE_USE_PHYSICS
    virtual void serializePhysicsScenes(xmlpp::document& doc, 
                                        xmlpp::element&  elem, 
                                        xmlpp::s_state   state);
#endif
    virtual void serializeVisualScenes(xmlpp::document& doc, 
                                       xmlpp::element&  elem, 
                                       xmlpp::s_state   state);

    virtual void serializeObjects(xmlpp::document& doc, 
                                  xmlpp::element&  elem, 
                                  xmlpp::s_state   state);

    virtual void serializeLocations(xmlpp::document& doc, 
                                    xmlpp::element&  elem, 
                                    xmlpp::s_state   state);

public:
    // for serialization
    visual_scene_vector     visualScenes;
    physics_scene_vector    physicsScenes;

protected:
    mesh_vector meshes;
};

} // namespace database
} // namespace slon

#endif // __SLON_ENGINE_DATABASE_PROPRIETARY_SXML_SERIALIZER_H__
