#include "stdafx.h"
#include "Database/Collada/Collada.h"

__DEFINE_LOGGER__("database.COLLADA")

namespace {

    using namespace slon;
    using namespace database;

    struct read_min_max
    {
    public:
        read_min_max(math::Vector3f& _minVal, math::Vector3f& _maxVal) :
            minVal(_minVal),
            maxVal(_maxVal)
        {}

        void operator () ( const ColladaDocument&	document,
                           const xmlpp::element&	elem )
        {
            collada_loader loader;
            loader >>= xmlpp::make_nvp( "min", xmlpp::make_elem_loader( boost::bind(&read_min_max::read, this, boost::ref(minVal), _1, _2) ) );
            loader >>= xmlpp::make_nvp( "max", xmlpp::make_elem_loader( boost::bind(&read_min_max::read, this, boost::ref(maxVal), _1, _2) ) );
            loader.load(document, elem);
        }

    private:
        void read( math::Vector3f&        value,
                   const ColladaDocument& document,
                   const xmlpp::node&     node )
        {
            std::istringstream ss( xmlpp::element(node).get_text() );
            for(int i = 0; i<3; ++i)
            {
                std::string strValue;
                ss >> strValue;
                if (strValue == "-INF") {
                    value[i] = -std::numeric_limits<float>::infinity();
                }
                else if (strValue == "INF") {
                    value[i] =  std::numeric_limits<float>::infinity();
                }
                else
                {
                    std::istringstream ss(strValue);
                    ss >> value[i];
                }
            }
        }

    private:
        math::Vector3f& minVal;
        math::Vector3f& maxVal;
    };

} // anonymous namespace

namespace slon {
namespace database {

void collada_shape::serialize( ColladaDocument& document, 
                               xmlpp::element&  elem, 
                               xmlpp::s_state	state )
{
    transform = math::make_identity<float, 4>();

    collada_serializer serializer;
    serializer >>= xmlpp::make_nvp( "translate",		xmlpp::make_elem_loader( read_translate(transform) ) );
    serializer >>= xmlpp::make_nvp( "rotate",			xmlpp::make_elem_loader( read_rotate(transform) ) );
    //loader.attach( "plane",             read_ptr<collada_plane>(geometry) );
    serializer &= xmlpp::make_nvp( "sphere",            xmlpp::as_element<collada_sphere_shape>(geometry) );
    serializer &= xmlpp::make_nvp( "box",               xmlpp::as_element<collada_box_shape>(geometry) );
    serializer &= xmlpp::make_nvp( "tapered_cylinder",  xmlpp::as_element<collada_tapered_cylinder_shape>(geometry) );
    //loader.attach( "capsule",           read_ptr<collada_capsule>(geometry) );
    serializer &= xmlpp::make_nvp( "instance_geometry", xmlpp::as_element<collada_geometry_shape>(geometry) );
    serializer.serialize(document, elem, state);
}

void collada_geometry_shape::serialize( ColladaDocument&	document, 
                                        xmlpp::element&		elem, 
                                        xmlpp::s_state		state )
{
    geometryInstance.load(document, elem);
}

void collada_tapered_cylinder_shape::serialize( ColladaDocument&	document, 
												xmlpp::element&		elem, 
												xmlpp::s_state		state )
{
    collada_serializer serializer;
    serializer &= xmlpp::make_nvp( "radius1", xmlpp::as_text(lowerRadius) );
	serializer &= xmlpp::make_nvp( "radius2", xmlpp::as_text(upperRadius) );
	serializer &= xmlpp::make_nvp( "height",  xmlpp::as_text(height) );
    serializer.serialize(document, elem, state);
}

void collada_sphere_shape::serialize( ColladaDocument&  document, 
                                      xmlpp::element&   elem, 
                                      xmlpp::s_state	state )
{
    collada_serializer serializer;
    serializer &= xmlpp::make_nvp( "radius", xmlpp::as_text(radius) );
    serializer.serialize(document, elem, state);
}

void collada_box_shape::serialize( ColladaDocument& document, 
                                   xmlpp::element&  elem, 
                                   xmlpp::s_state	state )
{
    collada_serializer serializer;
    serializer &= xmlpp::make_nvp( "half_extents", xmlpp::as_text(halfExtents) );
    serializer.serialize(document, elem, state);
}

void collada_physics_material::serialize( ColladaDocument&  document, 
                                          xmlpp::element&   elem, 
                                          xmlpp::s_state	state )
{
    dynamicFriction = staticFriction = restitution = 0.0f;

    collada_serializer serializer;
	serializer &= xmlpp::make_nvp( "id",                xmlpp::as_attribute(id) );
    serializer &= xmlpp::make_nvp( "dynamic_friction",  xmlpp::as_text(dynamicFriction) );
    serializer &= xmlpp::make_nvp( "static_friction",   xmlpp::as_text(staticFriction) );
    serializer &= xmlpp::make_nvp( "restitution",       xmlpp::as_text(restitution) );
    serializer.serialize(document, elem, state);
}

void collada_limits::serialize( ColladaDocument&  document, 
                                xmlpp::element&   elem, 
                                xmlpp::s_state    state )
{
    linear[0]         = linear[1]         = math::Vector3f(0.0f, 0.0f, 0.0f);
    swingConeTwist[0] = swingConeTwist[1] = math::Vector3f(0.0f, 0.0f, 0.0f);
	
    collada_serializer serializer;
    serializer >>= xmlpp::make_nvp( "swing_cone_and_twist",  xmlpp::make_elem_loader( read_min_max(swingConeTwist[0], swingConeTwist[1]) ) );
    serializer >>= xmlpp::make_nvp( "linear",                xmlpp::make_elem_loader( read_min_max(linear[0], linear[1]) ) );
    serializer.serialize(document, elem, state);
}

void collada_attachment::serialize( ColladaDocument&  document, 
                                    xmlpp::element&   elem, 
                                    xmlpp::s_state    state )
{
    transform = math::make_identity<float, 4>();
	
    collada_serializer serializer;
    serializer &=  xmlpp::make_nvp( "rigid_body",	xmlpp::as_attribute(rigidBodyStr) );
    serializer >>= xmlpp::make_nvp( "translate",	xmlpp::make_elem_loader( read_translate(transform) ) );
    serializer >>= xmlpp::make_nvp( "rotate",		xmlpp::make_elem_loader( read_rotate(transform) ) );
    serializer.serialize(document, elem, state);
}

void collada_rigid_constraint::load_technique_common( const ColladaDocument& document,
                                                      const xmlpp::node&     node )
{
    collada_serializer serializer;
    serializer >>= xmlpp::make_nvp( "enabled", xmlpp::make_elem_loader( read_bool(enabled) ) );
    serializer >>= xmlpp::make_nvp( "limits",  xmlpp::as_element(limits) );
    serializer.load(document, xmlpp::element(node));
}

void collada_rigid_constraint::serialize( ColladaDocument&  document, 
                                          xmlpp::element&   elem, 
                                          xmlpp::s_state    state )
{
    // setup default values
    enabled = true;

    // load
    collada_serializer serializer;
	serializer  &= xmlpp::make_nvp( "sid",				xmlpp::as_attribute(sid) );
    serializer  &= xmlpp::make_nvp( "attachment",       xmlpp::as_element(attachment) );
    serializer  &= xmlpp::make_nvp( "ref_attachment",   xmlpp::as_element(refAttachment) );
    serializer >>= xmlpp::make_nvp( "technique_common", xmlpp::make_elem_loader( boost::bind(&collada_rigid_constraint::load_technique_common, this, _1, _2) ) );
    serializer.serialize(document, elem, state);
}

void collada_instance_rigid_constraint::serialize( ColladaDocument&  document, 
                                                   xmlpp::element&   elem, 
                                                   xmlpp::s_state    state )
{
    collada_serializer serializer;
    serializer &= xmlpp::make_nvp( "constraint", xmlpp::as_attribute(constraintStr) );
	serializer.serialize(document, elem, state);
}

void collada_rigid_body::load_technique_common( const ColladaDocument& document,
                                                const xmlpp::node&     node )
{
    collada_serializer serializer;
    serializer  &= xmlpp::make_nvp( "mass",          xmlpp::as_text(mass) );
    serializer >>= xmlpp::make_nvp( "dynamic",       xmlpp::make_elem_loader( read_bool(dynamic) ) );
    //serializer &= xmlpp::make_nvp( "instance_physics_material", boost::ref(materialInstance) );
    //serializer &= xmlpp::make_nvp( "physics_material",          read_ptr(materialInstance.element) );
    serializer  &= xmlpp::make_nvp( "shape",         xmlpp::as_element_set(shapes) );
    serializer.load(document, xmlpp::element(node));
}

void collada_rigid_body::serialize( ColladaDocument&  document, 
                                    xmlpp::element&   elem, 
                                    xmlpp::s_state    state )
{
    collada_serializer serializer;
	serializer  &= xmlpp::make_nvp( "sid",			    xmlpp::as_attribute(sid) );
    serializer >>= xmlpp::make_nvp( "technique_common", xmlpp::make_elem_loader( boost::bind(&collada_rigid_body::load_technique_common, this, _1, _2) ) );
    serializer.serialize(document, elem, state);
}

void collada_instance_rigid_body::serialize( ColladaDocument&  document, 
											 xmlpp::element&   elem, 
											 xmlpp::s_state    state )
{
	collada_serializer serializer;
	serializer &= xmlpp::make_nvp( "body",		xmlpp::as_attribute(body) );
	serializer &= xmlpp::make_nvp( "target",	xmlpp::as_attribute(target) );
	serializer.serialize(document, elem, state);
}

void collada_physics_model::serialize( ColladaDocument&  document, 
                                       xmlpp::element&   elem, 
                                       xmlpp::s_state    state )
{
    collada_serializer serializer;
	serializer &= xmlpp::make_nvp( "id",               xmlpp::as_attribute(id) );
    serializer &= xmlpp::make_nvp( "rigid_body",       xmlpp::as_element_set(rigidBodies) );
    serializer &= xmlpp::make_nvp( "rigid_constraint", xmlpp::as_element_set(rigidConstraints) );
    serializer.serialize(document, elem, state);
}

void collada_instance_physics_model::load( const ColladaDocument&   document,
                                           const xmlpp::element&    instanceElement )
{
    // load child elements
    collada_loader loader;
	loader >>= xmlpp::make_nvp( "url",						 xmlpp::as_attribute(url) );
    loader >>= xmlpp::make_nvp( "instance_rigid_body",       xmlpp::as_element_set(rigidBodyInstances) );
    loader >>= xmlpp::make_nvp( "instance_rigid_constraint", xmlpp::as_element_set(rigidConstraintInstances) );
    loader.load(document, instanceElement);

    // resolve rigid bodies
    element = document.libraryPhysicsModels.get_element(url.substr(1));
    for ( instance_rigid_body_iterator iter =  rigidBodyInstances.begin();
                                       iter != rigidBodyInstances.end();
                                       ++iter )
    {
        for (size_t i = 0; i<element->rigidBodies.size(); ++i)
        {
            if (element->rigidBodies[i]->sid == iter->body)
            {
                iter->element = element->rigidBodies[i];
                break;
            }
        }

        if (!iter->element) {
            throw collada_error(logger, "Can't find rigid body: " + iter->body + "\n for the model: " + url);
        }
    }

    // resolve rigid constraints
    for ( instance_rigid_constraint_iterator iter =  rigidConstraintInstances.begin();
                                             iter != rigidConstraintInstances.end();
                                             ++iter )
    {
        for (size_t i = 0; i<element->rigidConstraints.size(); ++i)
        {
            if (element->rigidConstraints[i]->sid == iter->constraintStr)
            {
                iter->element = element->rigidConstraints[i];
                break;
            }
        }

        if (!iter->element) {
            throw collada_error(logger, "Can't find constraint: " + iter->constraintStr + "\n for the model: " + url);
        }

        // resolve constraints attachments
        for (size_t i = 0; i<element->rigidBodies.size(); ++i)
        {
            if ( element->rigidBodies[i]->sid == iter->element->attachment.rigidBodyStr) {
                iter->element->attachment.rigidBody = element->rigidBodies[i];
            }
            if ( element->rigidBodies[i]->sid == iter->element->refAttachment.rigidBodyStr) {
                iter->element->refAttachment.rigidBody = element->rigidBodies[i];
            }
        }

        if (!iter->element->attachment.rigidBody)
        {
            throw collada_error(logger, "Can't find constraint attachment: "
                                        + iter->element->attachment.rigidBodyStr + "\n for the constraint: " + iter->element->sid);
        }
        else if (!iter->element->refAttachment.rigidBody)
        {
            throw collada_error(logger, "Can't find constraint attachment: "
                                        + iter->element->refAttachment.rigidBodyStr + "\n for the constraint: " + iter->element->sid);
        }
    }
}

void collada_physics_scene::serialize( ColladaDocument& document, 
                                       xmlpp::element&  elem, 
                                       xmlpp::s_state   state )
{
	collada_serializer serializer;
	serializer &= xmlpp::make_nvp( "id",					 xmlpp::as_attribute(id) );
    serializer &= xmlpp::make_nvp( "instance_physics_model", xmlpp::as_element_set(physicsModelInstances) );
    serializer.serialize(document, elem, state);
}

} // namespace slon
} // namespace database
