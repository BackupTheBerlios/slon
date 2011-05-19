#include "stdafx.h"
#include "Database/DatabaseManager.h"
#include "Database/Collada/Collada.h"
#include "Graphics/Renderable/Mesh.h"
#include "Utility/URI/file_uri.hpp"
#include <sgl/Math/Utility.hpp>
#include <sstream>

DECLARE_AUTO_LOGGER("database.COLLADA")

namespace slon {
namespace database {

void collada_source::loadTechniqueCommon( const ColladaDocument&   /*document*/,
                                          const xmlpp::node&       node)
{
    xmlpp::element techniqueElem(node);

    xmlpp::const_element_iterator accessorIter = techniqueElem.first_child_element("accessor");
    if ( !accessorIter ) {
        throw collada_error(AUTO_LOGGER, "technique_common doesn't have accessor elem", techniqueElem);
    }

    stride = xmlpp::read_attribute<size_t>("stride", *accessorIter);

	//elemType = ELEMENT_TYPE(stride - 1);
}

void collada_source::serialize( ColladaDocument&    document,
                                xmlpp::element&     elem,
                                xmlpp::s_state      state )
{
    // get array
    collada_serializer serializer;
    serializer  &= xmlpp::make_nvp( "id",                xmlpp::as_attribute(id) );
    serializer  &= xmlpp::make_nvp( "Name_array",        xmlpp::as_string(nameArray) );
    serializer  &= xmlpp::make_nvp( "float_array",       xmlpp::as_string(floatArray) );
    serializer >>= xmlpp::make_nvp( "technique_common",  xmlpp::make_elem_loader( boost::bind(&collada_source::loadTechniqueCommon, this, _1, _2) ) );
    serializer.serialize(document, elem, state);
}

void collada_input::serialize( ColladaDocument&     document,
                               xmlpp::element&      elem,
                               xmlpp::s_state       state )
{
    collada_serializer serializer;
    serializer &= xmlpp::make_nvp( "semantic",  xmlpp::as_attribute(semantic) );
    serializer &= xmlpp::make_nvp( "source",    xmlpp::as_attribute(sourceId) );
    serializer &= xmlpp::make_nvp( "set",       xmlpp::as_attribute(set) );
    serializer &= xmlpp::make_nvp( "offset",    xmlpp::as_attribute(offset) );
    serializer.serialize(document, elem, state);

    // select attribute index
    if ( state == xmlpp::LOAD)
    {
	    sourceId = sourceId.substr(1);

        if ( semantic == "POSITION" ) 
        {
            attributeIndex = graphics::Mesh::POSITION;
            attributeName  = "position";
        }
        else if ( semantic == "NORMAL" ) 
        {
            attributeIndex = graphics::Mesh::NORMAL;
            attributeName  = "normal";
        }
        else if ( semantic == "TEXCOORD" ) 
        {
            attributeIndex = graphics::Mesh::TEXCOORD + set;
            attributeName  = "texcoord";
        }
        else if ( semantic == "COLOR" )
        {
            attributeIndex = graphics::Mesh::COLOR;
            attributeName  = "color";
        }
        else if ( semantic == "JOINT" )
        {
            attributeIndex = graphics::Mesh::BONE_INDEX;
            attributeName  = "joint";
        }
        else if ( semantic == "WEIGHT" )
        {
            attributeIndex = graphics::Mesh::BONE_WEIGHT;
            attributeName  = "weight";
        }
        else
        {
            // ignore unknown attributes
            attributeIndex = -1;
            attributeName  = semantic;
        }
    }
}

void collada_primitives::load( const ColladaDocument&   document, 
                               const xmlpp::element&    elem )
{
    // get primitive type
    if ( elem.get_value() == "triangles" ) {
        primType = sgl::TRIANGLES;
    }
	else if ( elem.get_value() == "polylist" ) {
		primType = sgl::POLYGON;
	}

    size_t count;
	std::vector<size_t> vcount;

    // load inputs
    collada_serializer serializer;
    serializer &= xmlpp::make_nvp( "material",  xmlpp::as_attribute(material) );
    serializer &= xmlpp::make_nvp( "count",     xmlpp::as_attribute(count) );
    serializer &= xmlpp::make_nvp( "input",     xmlpp::as_element_set(inputs) );
    serializer &= xmlpp::make_nvp( "vcount",    xmlpp::as_string(vcount) );
    serializer.load(document, elem);

    // read data
	xmlpp::const_element_iterator pIter = elem.first_child_element("p");
    if ( pIter != elem.end_child_element() )
    {
		// sort inputs by offset
		sort( inputs.begin(), inputs.end(), boost::bind(&collada_input::offset, _1) < boost::bind(&collada_input::offset, _2) );

		// read inputs
		if ( primType == sgl::POLYGON )
		{
			inputIndices.resize( inputs.size() );
			
			std::vector<size_t> start( inputs.size() );
			std::vector<size_t> prev( inputs.size() );
			size_t cur;

    		std::istringstream ss( pIter->get_text() );
			for(size_t k = 0; k<vcount.size(); ++k)
			{
				for(size_t j = 0; j<inputs.size(); ++j)
				{
					ss >> start[j];
				}

				for(size_t j = 0; j<inputs.size(); ++j)
				{
					ss >> prev[j];
				}

				for(size_t i = 2; i<vcount[k]; ++i)
				{
					for(size_t j = 0; j<inputs.size(); ++j)
					{
						ss >> cur;
						inputIndices[j].push_back(start[j]);
						inputIndices[j].push_back(prev[j]);
						inputIndices[j].push_back(cur);
						prev[j] = cur;
					}
				}
			}

			primType = sgl::TRIANGLES;
		}
		else
		{
			inputIndices.resize( inputs.size() );
			for(size_t i = 0; i<inputs.size(); ++i) {
				inputIndices[i].resize(count*3);
			}

    		std::istringstream ss( pIter->get_text() );
			for(size_t i = 0; i<count*3; ++i)
			{
				for(size_t j = 0; j<inputs.size(); ++j)
				{
					ss >> inputIndices[j][i];
				}
			}
		}
	}
}

void collada_primitives::save(ColladaDocument& document, xmlpp::element& elem) const
{
	switch (primType)
	{
	case sgl::TRIANGLES:
		elem.set_value("triangles");
		break;

	case sgl::POLYGON:
		elem.set_value("polylist");
		break;

	default:
		throw slon_error(AUTO_LOGGER, "collada_primitives::save failed. Unsupported primitives type for saving.");
	}
	
	std::vector<size_t> vcount( inputIndices.size() );
	std::vector<size_t> v;
	for (size_t i = 0; i<inputIndices.size(); ++i) 
	{
		vcount[i] = inputIndices[i].size();
		for (size_t j = 0; j<inputIndices[i].size(); ++j) {
			v.push_back(inputIndices[i][j]);
		}
	}

    // load inputs
    collada_serializer serializer;
    serializer <<= xmlpp::make_nvp( "material",  xmlpp::to_attribute(material) );
    serializer <<= xmlpp::make_nvp( "count",     xmlpp::to_attribute(vcount.size()) );
    serializer <<= xmlpp::make_nvp( "input",     xmlpp::to_element_set(inputs) );
    serializer <<= xmlpp::make_nvp( "vcount",    xmlpp::to_string(vcount) );
	serializer <<= xmlpp::make_nvp( "p",			xmlpp::to_string(v) );
    serializer.save(document, elem);
}

void collada_vertices::serialize( ColladaDocument&  document, 
                                  xmlpp::element&   elem, 
                                  xmlpp::s_state    state )
{
    // load inputs
    collada_serializer serializer;
    serializer &= xmlpp::make_nvp( "input", xmlpp::as_element_set(inputs) );
    serializer.serialize(document, elem, state);
}

collada_source_ptr collada_mesh::getSourceById(const std::string& sourceId)
{
	for (size_t i = 0; i<sources.size(); ++i)
	{
		if (sources[i]->id == sourceId) {
			return sources[i];
		}
	}

	return collada_source_ptr();
}

void collada_instance_geometry::load(const ColladaDocument& document, 
                                     const xmlpp::element&  elem)
{
    // load geometry
    if ( !elem.has_attribute("url") ) {
        throw collada_error(AUTO_LOGGER, "Missing url in the <instance_geometry> element", elem);
    }
    geometry = document.libraryGeometries.get_element( elem.get_attribute("url").substr(1) );

    xmlpp::const_element_iterator bindMaterialIter = elem.first_child_element("bind_material");
    if (bindMaterialIter)
    {
        material.reset(new collada_bind_material);
        material->load(document, *bindMaterialIter);
    }
}

void collada_geometry::serialize( ColladaDocument&  document, 
                                  xmlpp::element&   elem, 
                                  xmlpp::s_state    state )
{
	
	collada_serializer serializer;
    serializer &= xmlpp::make_nvp( "id",	xmlpp::as_attribute(id) );
    serializer &= xmlpp::make_nvp( "name",  xmlpp::as_attribute(name) );
    serializer.serialize(document, elem, state);
}

void collada_mesh::serialize( ColladaDocument& document,
							  xmlpp::element&  element, 
                              xmlpp::s_state   state )
{
    using namespace boost;

	// serialize base
	collada_geometry::serialize(document, *element.get_parent(), state);

	// load
	collada_serializer serializer;
    serializer &= xmlpp::make_nvp( "source",    xmlpp::as_element_set<collada_source>(sources) );
    serializer &= xmlpp::make_nvp( "triangles", xmlpp::as_element_set(primitives) );
    serializer &= xmlpp::make_nvp( "polylist",  xmlpp::as_element_set(primitives) );
    serializer &= xmlpp::make_nvp( "vertices",  xmlpp::as_element(vertices) );
    serializer.serialize(document, element, state);

	if (state == xmlpp::LOAD)
	{
		// check mesh validness
		if ( sources.empty() ) {
			throw collada_error(AUTO_LOGGER, "Mesh must have at least one source element: " + id, element);
		}

		if ( vertices.inputs.empty() ) {
			throw collada_error(AUTO_LOGGER, "Mesh must have <vertices> element with at least one input: " + id, element);
		}

		// find primitives input sources
		for(size_t i = 0; i<primitives.size(); ++i)
		{
    		collada_primitives& prims = primitives[i];
    		for(size_t j = 0; j<prims.inputs.size(); ++j)
    		{
    			collada_input& input = prims.inputs[j];

    			if ( input.attributeName != "VERTEX" )
    			{
    				input.source = getSourceById(input.sourceId);
					if (!input.source) {
						throw collada_error(AUTO_LOGGER, "Couldn't find mesh source element: " + input.sourceId, element);
					}
    			}
    		}
		}

		// find vertices input sources
		for(size_t j = 0; j<vertices.inputs.size(); ++j)
		{
			collada_input& input = vertices.inputs[j];

			input.source = getSourceById(input.sourceId);
			if (!input.source) {
				throw collada_error(AUTO_LOGGER, "Couldn't find mesh source element: " + input.sourceId, element);
			}
		}
	}
}

void collada_convex_mesh::load( const ColladaDocument& document,
                                const xmlpp::element&  elem )
{
	// serialize base
	collada_geometry::load(document, *elem.get_parent());

    if ( elem.has_attribute("convex_hull_of") )
    {
        std::string target = elem.get_attribute("convex_hull_of");

        isConvexHullOfMesh = true;
        mesh = boost::shared_dynamic_cast<collada_mesh>( document.libraryGeometries.get_element(target) );

        if (!mesh) {
            throw collada_error(AUTO_LOGGER, "Unable to load mesh to build convex hull: " + target);
        }
    }
    else
    {
        // load as regular mesh
        isConvexHullOfMesh = false;
        mesh.reset(new collada_mesh);
        mesh->load(document, elem);
    }
}

void collada_convex_mesh::save(ColladaDocument& document, xmlpp::element& elem) const
{
	assert(mesh);

	// serialize base
	collada_geometry::save(document, elem);

	if (isConvexHullOfMesh) {
		elem.set_attribute("convex_hull_of", mesh->id);
	}
	else {
		mesh->save(document, elem);
	}
}

std::istream& operator >> (std::istream& is, collada_node::TYPE& type)
{
	std::string typeStr;
	is >> typeStr;
	type = (typeStr == "JOINT") ? collada_node::JOINT : collada_node::NODE;
	return is;
}

std::ostream& operator << (std::ostream& os, collada_node::TYPE type)
{
	os << (type == collada_node::JOINT) ? "JOINT" : "NODE";
	return os;
}

void collada_node::serialize(ColladaDocument& document, 
                             xmlpp::element&  elem, 
                             xmlpp::s_state   state)
{
    if ( state == xmlpp::LOAD ) {
        transform = math::make_identity<float, 4>();
	}

	// load
	collada_serializer serializer;
    serializer  &= xmlpp::make_nvp( "id",					xmlpp::as_attribute(id) );
    serializer  &= xmlpp::make_nvp( "name",					xmlpp::as_attribute(name) );
    serializer  &= xmlpp::make_nvp( "sid",					xmlpp::as_attribute(sid) );
    serializer  &= xmlpp::make_nvp( "type",					xmlpp::as_attribute(type) );
    serializer  &= xmlpp::make_nvp( "instance_geometry",	xmlpp::as_element_set(geometries) );
    serializer  &= xmlpp::make_nvp( "instance_controller",	xmlpp::as_element_set(controllers) );
	serializer  &= xmlpp::make_nvp( "node",					xmlpp::as_element_set<collada_node>(children) );

	serializer >>= xmlpp::make_nvp( "translate",			xmlpp::make_elem_loader( read_translate(transform) ) );
	serializer >>= xmlpp::make_nvp( "rotate",				xmlpp::make_elem_loader( read_rotate(transform) ) );
	serializer >>= xmlpp::make_nvp( "scale",				xmlpp::make_elem_loader( read_scale(transform) ) );
    serializer  &= xmlpp::make_nvp( "matrix",				xmlpp::as_text(transform) );
    
    serializer.serialize(document, elem, state);
}

void collada_joints::serialize(ColladaDocument&             document, 
                               xmlpp::element&              elem, 
                               xmlpp::s_state   state)
{
    collada_serializer serializer;
    serializer &= xmlpp::make_nvp( "input", xmlpp::as_element_set(inputs) );
    serializer.serialize(document, elem, state);
}

void collada_vertex_weights::serialize(ColladaDocument& document, 
                                       xmlpp::element&  elem,
                                       xmlpp::s_state   state)
{
    size_t count = vcount.size();

    collada_serializer serializer;
    serializer &= xmlpp::make_nvp("count",  xmlpp::as_attribute(count) );
    serializer &= xmlpp::make_nvp("input",  xmlpp::as_element_set(inputs) );
    serializer &= xmlpp::make_nvp("vcount", xmlpp::as_string(vcount) );
    serializer &= xmlpp::make_nvp("v",      xmlpp::as_string(v) );
    serializer.serialize(document, elem, state);

    // verify
    assert( count == vcount.size() );
    for (size_t i = 0, count = 0; i<vcount.size(); ++i) {
        count += vcount[i];
    }
    //assert( count == v.size() );
}

void collada_controller::serialize( ColladaDocument&    document, 
                                    xmlpp::element&     elem, 
                                    xmlpp::s_state      state )
{
    collada_serializer serializer;
    serializer &= xmlpp::make_nvp( "id",    xmlpp::as_attribute(id) );
    serializer &= xmlpp::make_nvp( "name",  xmlpp::as_attribute(name) );
    serializer &= xmlpp::make_nvp( "skin",  xmlpp::as_element<collada_skin>(controlElement) );
    serializer.serialize(document, elem, state);
}

void collada_skin::serialize( ColladaDocument&           document, 
                              xmlpp::element&            elem, 
                              xmlpp::s_state state )
{
    if (state == xmlpp::SAVE) {
        sourceURI = source->id;
    }

    collada_serializer serializer;
    serializer &= xmlpp::make_nvp( "source",            xmlpp::as_attribute(sourceURI) );
    serializer &= xmlpp::make_nvp( "bind_shape_matrix", xmlpp::as_text(bindShapeMatrix) );
    serializer &= xmlpp::make_nvp( "source",            xmlpp::as_element_set(sources) );
    serializer &= xmlpp::make_nvp( "joints",            xmlpp::as_element(joints) );
    serializer &= xmlpp::make_nvp( "vertex_weights",    xmlpp::as_element(weights) );
    serializer.serialize(document, elem, state);

    if (state == xmlpp::LOAD) 
    {
        source = document.libraryGeometries.get_element(sourceURI.substr(1));
    
        // find input sources
	    for(size_t j = 0; j<weights.inputs.size(); ++j)
	    {
		    weights.inputs[j].source = getSourceById(weights.inputs[j].sourceId);
		    if (!weights.inputs[j].source) {
			    throw collada_error(AUTO_LOGGER, "Couldn't find mesh source element: " + weights.inputs[j].sourceId, elem);
		    }
	    }

	    for(size_t j = 0; j<joints.inputs.size(); ++j)
	    {
		    joints.inputs[j].source = getSourceById(joints.inputs[j].sourceId);
		    if (!joints.inputs[j].source) {
			    throw collada_error(AUTO_LOGGER, "Couldn't find mesh source element: " + joints.inputs[j].sourceId, elem);
		    }
	    }
    }
}

collada_source_ptr collada_skin::getSourceById(const std::string& sourceId)
{
	for (size_t i = 0; i<sources.size(); ++i)
	{
		if (sources[i]->id == sourceId) {
			return sources[i];
		}
	}

	return collada_source_ptr();
}

void collada_instance_controller::load( const ColladaDocument&  document, 
                                        const xmlpp::element&   elem )
{
    // load geometry
    collada_serializer serializer;
    serializer &= xmlpp::make_nvp( "url",       xmlpp::as_attribute(url) );
    serializer &= xmlpp::make_nvp( "skeleton",  xmlpp::as_text(skeleton) );
    serializer.load(document, elem);

    controller = document.libraryControllers.get_element( url.substr(1) );

    xmlpp::const_element_iterator bindMaterialIter = elem.first_child_element("bind_material");
    if (bindMaterialIter)
    {
        material.reset(new collada_bind_material);
        material->load(document, *bindMaterialIter);
    }
}

std::string collada_channel::get_target_node() const
{
    size_t slash_i = target.find('/');
    return slash_i != std::string::npos ? target.substr(0, slash_i ) : "";
}

std::string collada_channel::get_target_transform() const
{
    size_t slash_i = target.find('/');
    return slash_i != std::string::npos ? target.substr(slash_i + 1) : "";
}

void collada_channel::serialize( ColladaDocument&  document, 
                                 xmlpp::element&   elem, 
                                 xmlpp::s_state    state )
{
	collada_serializer serializer;
    serializer &= xmlpp::make_nvp( "source",    xmlpp::as_attribute(source) );
    serializer &= xmlpp::make_nvp( "target",    xmlpp::as_attribute(target) );
    serializer.serialize(document, elem, state);
}

void collada_sampler::serialize( ColladaDocument&  document, 
                                 xmlpp::element&   elem, 
                                 xmlpp::s_state    state )
{
	collada_serializer serializer;
    serializer &= xmlpp::make_nvp( "id",    xmlpp::as_attribute(id) );
    serializer &= xmlpp::make_nvp( "input", xmlpp::as_element_set(inputs) );
    serializer.serialize(document, elem, state);
}

collada_animation::source_iterator collada_animation::get_source(const std::string& id)
{
    return std::find_if( sources.begin(), sources.end(), boost::bind(&collada_source::id, _1) == id );
}

collada_animation::sampler_iterator collada_animation::get_sampler(const std::string& id)
{
    return std::find_if( samplers.begin(), samplers.end(), boost::bind(&collada_sampler::id, _1) == id );
}

void collada_animation::serialize( ColladaDocument&  document, 
                                   xmlpp::element&   elem, 
                                   xmlpp::s_state    state )
{
	collada_serializer serializer;
    serializer &= xmlpp::make_nvp( "id",        xmlpp::as_attribute(id) );
    serializer &= xmlpp::make_nvp( "name",      xmlpp::as_attribute(name) );
    serializer &= xmlpp::make_nvp( "animation", xmlpp::as_element_set(animations) );
    serializer &= xmlpp::make_nvp( "source",    xmlpp::as_element_set(sources) );
    serializer &= xmlpp::make_nvp( "sampler",   xmlpp::as_element_set(samplers) );
    serializer &= xmlpp::make_nvp( "channel",   xmlpp::as_element_set(channels) );
    serializer.serialize(document, elem, state);
}

void collada_visual_scene::serialize( ColladaDocument&  document, 
                                      xmlpp::element&   elem, 
                                      xmlpp::s_state    state )
{
	collada_serializer serializer;
	serializer &= xmlpp::make_nvp( "id",	xmlpp::as_attribute(id) );
	serializer &= xmlpp::make_nvp( "name",	xmlpp::as_attribute(name) );
    serializer &= xmlpp::make_nvp( "node",  xmlpp::as_element_set<collada_node>(nodes) );
    serializer.serialize(document, elem, state);
}

} // namespace database
} // namesapce slon
