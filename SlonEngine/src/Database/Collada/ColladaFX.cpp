#include "stdafx.h"
#include "Database/Collada/Collada.h"

DECLARE_AUTO_LOGGER("database.COLLADA")

using namespace slon;
using namespace database;

namespace {

	using namespace math;

	inline sgl::SamplerState::WRAPPING wrappingFromString(const std::string& clampStr)
	{
		if (clampStr == "WRAP") {
			return sgl::SamplerState::REPEAT;
		}
		else if (clampStr == "CLAMP") {
			return sgl::SamplerState::CLAMP_TO_EDGE;
		}
		else if (clampStr == "BORDER") {
			return sgl::SamplerState::CLAMP;
		}

		return sgl::SamplerState::CLAMP;
	}

	inline sgl::SamplerState::FILTER filterFromString(const std::string& filterStr)
	{
		if (filterStr == "NEAREST") {
			return sgl::SamplerState::NEAREST;
		}
		else if (filterStr == "LINEAR") {
			return sgl::SamplerState::LINEAR;
		}

		return sgl::SamplerState::NEAREST;
	}

	inline const char* toString(sgl::SamplerState::WRAPPING clamp)
	{
		switch (clamp)
		{
		case sgl::SamplerState::REPEAT: 
			return "WRAP";

		case sgl::SamplerState::CLAMP_TO_EDGE: 
			return "CLAMP";

		case sgl::SamplerState::CLAMP: 
			return "BORDER";

		default:
			assert(!"Can't get here");
			return "CLAMP";
		}
	}

	inline const char* toString(sgl::SamplerState::FILTER filter)
	{
		switch (filter)
		{
		case sgl::SamplerState::NONE: 
			return "NONE";

		case sgl::SamplerState::NEAREST: 
			return "NEAREST";

		case sgl::SamplerState::LINEAR: 
			return "LINEAR";

		default:
			assert(!"Can't get here");
			return "NONE";
		}
	}

} // anonymous namespace

namespace slon {
namespace database {

void collada_image::serialize( ColladaDocument&  document, 
							   xmlpp::element&   elem, 
							   xmlpp::s_state    state )
{
	collada_serializer serializer;
	serializer &= xmlpp::make_nvp( "id",		xmlpp::as_attribute(id) );
	serializer &= xmlpp::make_nvp( "init_from", xmlpp::as_text(uri) );
	serializer.serialize(document, elem, state);
}

void collada_instance<collada_image>::load(const ColladaDocument& document, 
                                           const xmlpp::element&  elem)
{
	element = document.libraryImages.get_element( elem.get_text() );
}

void collada_param::serialize( ColladaDocument&  document, 
							   xmlpp::element&   elem, 
							   xmlpp::s_state    state )
{
	// get parent element, because it is called from derived
	xmlpp::element_iterator parent = elem.get_parent();
	assert( parent && "Call collada_param::operator() from derived type" );

    collada_serializer serializer;
    serializer &= xmlpp::make_nvp( "sid",      xmlpp::as_attribute(sid) );
    serializer &= xmlpp::make_nvp( "semantic", xmlpp::as_text(semantic) );
	serializer.serialize( document, *parent, state );
}

template<typename T>
void collada_value_param<T>::serialize( ColladaDocument&  document, 
										xmlpp::element&   elem, 
										xmlpp::s_state    state )
{
	// load common param settings
	xmlpp::element_iterator parent = elem.get_parent();
	collada_param::serialize(document, elem, state);

	if (state == xmlpp::LOAD) {
		xmlpp::as_text(value).load(document, elem);
	}
	else {
		xmlpp::as_text(value).save(document, elem);
	}
}

void collada_surface_param::serialize( ColladaDocument&  document, 
									   xmlpp::element&   elem, 
									   xmlpp::s_state    state )
{
	// load common param settings
    collada_param::serialize(document, elem, state);

	collada_serializer serializer;
    serializer &= xmlpp::make_nvp( "init_from", xmlpp::as_element(image) );
	serializer.serialize(document, elem, state);
}

void collada_sampler2D_param::serialize( ColladaDocument&  document, 
										 xmlpp::element&   elem, 
										 xmlpp::s_state    state )
{
	// load common param settings
	collada_param::serialize(document, elem, state);

	collada_serializer serializer;
    serializer &= xmlpp::make_nvp("source", 	xmlpp::as_text(surfaceSid));
    serializer &= xmlpp::make_nvp("wrap_s", 	xmlpp::as_text(clamp[0]));
	serializer &= xmlpp::make_nvp("wrap_t", 	xmlpp::as_text(clamp[1]));
	serializer &= xmlpp::make_nvp("minfilter", 	xmlpp::as_text(filter[0]));
	serializer &= xmlpp::make_nvp("magfilter", 	xmlpp::as_text(filter[1]));
	serializer &= xmlpp::make_nvp("mipfilter", 	xmlpp::as_text(filter[2]));
	serializer.serialize(document, elem, state);
}

sgl::SamplerState::WRAPPING collada_sampler2D_param::getWrapping(int mode) const
{
	assert (mode >= 0 && mode < sizeof(clamp) );
	return wrappingFromString(clamp[mode]);
}

sgl::SamplerState::FILTER collada_sampler2D_param::getFilter(int mode) const
{
	assert (mode >= 0 && mode < sizeof(filter) );
	return filterFromString(filter[mode]);
}

void collada_sampler2D_param::setWrapping(int mode, sgl::SamplerState::WRAPPING wrapping_)
{
	assert (mode >= 0 && mode < sizeof(clamp) );
	clamp[mode] = toString(wrapping_);
}

void collada_sampler2D_param::setFilter(int mode, sgl::SamplerState::FILTER filter_)
{
	assert (mode >= 0 && mode < sizeof(filter) );
	filter[mode] = toString(filter_);
}

void collada_texture::serialize( ColladaDocument&  document, 
								 xmlpp::element&   elem, 
								 xmlpp::s_state    state )	
{
	collada_serializer serializer;
    serializer &= xmlpp::make_nvp("texture",  xmlpp::as_attribute(samplerSid));
    serializer &= xmlpp::make_nvp("texcoord", xmlpp::as_attribute(texcoord));
    serializer.serialize(document, elem, state);
}

void collada_color_or_texture::serialize( ColladaDocument&  document, 
										  xmlpp::element&   elem, 
										  xmlpp::s_state    state )	
{
	collada_serializer serializer;
	serializer &= xmlpp::make_nvp("color",   xmlpp::as_text(color));
	serializer &= xmlpp::make_nvp("texture", xmlpp::as_element(texture));
	serializer.serialize(document, elem, state);

	if ( state == xmlpp::LOAD && (elem.first_child_element("texture") != elem.end_child_element()) ) {
		type = TEXTURE;
	}
}

// collada_phong
void collada_phong_technique::load( const ColladaDocument&  document,
								    const xmlpp::element&   elem )
{
	shininess = 0.0f;
	opacity   = 1.0f;

	collada_serializer loader;
    loader &= xmlpp::make_nvp( "ambient", 		xmlpp::as_element(ambient) );
	loader &= xmlpp::make_nvp( "diffuse", 		xmlpp::as_element(diffuse) );
	loader &= xmlpp::make_nvp( "specular", 		xmlpp::as_element(specular) );
	loader &= xmlpp::make_nvp( "emission", 		xmlpp::as_element(emission) );
    loader &= xmlpp::make_nvp( "shininess", 	xmlpp::as_element(shininess) );
	loader &= xmlpp::make_nvp( "transparency",	xmlpp::as_element(opacity) );
	loader.load(document, elem);

	// add transparency
	ambient.color.w = diffuse.color.w = specular.color.w = emission.color.w = opacity.value;

	// resolve parameters
	if (parentEffect)
	{
		static const size_t  	  num_maps = 4;
		collada_color_or_texture* maps[num_maps] = { &ambient, &diffuse, &specular, &emission };

		for(size_t i = 0; i<num_maps; ++i)
		{
			if (maps[i]->type == collada_color_or_texture::TEXTURE)
			{
				collada_param_ptr samplerParam = parentEffect->getParameterBySid(maps[i]->texture.samplerSid);
				maps[i]->texture.sampler = boost::shared_dynamic_cast<collada_sampler2D_param>(samplerParam);
                if (!maps[i]->texture.sampler) {
                    throw collada_error(AUTO_LOGGER, "Couldn't find sampler parameter: " + maps[i]->texture.samplerSid);
                }
			}
		}
	}
}

template<typename T>
T& collada_value_or_param<T>::operator = (const T& value_)
{
    value = value_;
    return value;
}

void collada_float_or_param::serialize( ColladaDocument&  document, 
										xmlpp::element&   elem, 
										xmlpp::s_state    state )	
{
    collada_serializer serializer;
    serializer &= xmlpp::make_nvp( "float",  xmlpp::as_text(value) );
    serializer &= xmlpp::make_nvp( "param",  xmlpp::as_text(param) );
    serializer.serialize(document, elem, state);
}

template<typename T>
void collada_value_or_param<T>::serialize( ColladaDocument&  document, 
										   xmlpp::element&   elem, 
										   xmlpp::s_state    state )	
{
	collada_serializer serializer;
	serializer &= xmlpp::make_nvp( "index",  xmlpp::as_attribute(index) );
	serializer &= xmlpp::make_nvp( "value",  xmlpp::as_attribute(value) );
	serializer &= xmlpp::make_nvp( "param",  xmlpp::as_text(param) );
	serializer.serialize(document, elem, state);
}

template<>
void collada_value_or_param<float>::serialize( ColladaDocument&  document, 
											   xmlpp::element&   elem, 
											   xmlpp::s_state    state )
{
    collada_serializer serializer;
    serializer &= xmlpp::make_nvp( "float",  xmlpp::as_text(value) );
    serializer &= xmlpp::make_nvp( "param",  xmlpp::as_text(param) );
    serializer.serialize(document, elem, state);
}

collada_states::collada_states() :
    cullFaceEnable(false),
    cullFace("FRONT_AND_BACK"),
    depthTestEnable(true),
    depthFunc("LEQUAL"),
    stencilTestEnable(false)
{
    blendEnable      = false;
    blendEquation[0] = "FUNC_ADD";
    blendEquation[1] = "FUNC_ADD";
    blendFunc[0][0]  = "ONE";
    blendFunc[0][1]  = "ONE";
    blendFunc[1][0]  = "ONE";
    blendFunc[1][1]  = "ONE";

    stencilFunc[0] = "ALWAYS";
    stencilFunc[1] = "ALWAYS";

    stencilOp[0][0] = "KEEP";
    stencilOp[0][1] = "KEEP";
    stencilOp[0][2] = "KEEP";

    stencilOp[1][0] = "KEEP";
    stencilOp[1][1] = "KEEP";
    stencilOp[1][2] = "KEEP";

    stencilRef[0] = 0;
    stencilRef[1] = 0;

    stencilMask[0] = 0xFFFFFFFF;
    stencilMask[1] = 0xFFFFFFFF;
}

void collada_states::serialize( ColladaDocument&  document, 
								xmlpp::element&   elem, 
								xmlpp::s_state    state )
{
    collada_serializer serializer;
    serializer &= xmlpp::make_nvp( "blend_enable",            xmlpp::as_element(blendEnable) );
/*
    serializer &= xmlpp::make_nvp( "blend_equation",          xmlpp::as_element(blendEquation[0]]) );
*/
    serializer &= xmlpp::make_nvp( "blend_equation_separate", xmlpp::make_nvp("rgb",     xmlpp::as_element(blendEquation[0])) 
                                                              & xmlpp::make_nvp("alpha", xmlpp::as_element(blendEquation[1])) );
/*
    serializer &= xmlpp::make_nvp( "blend_func",              xmlpp::make_nvp("src",   blendFunc[0][0] + blendFunc[0][1]) 
                                                              & xmlpp::make_nvp("dst", blendFunc[1][0] + blendFunc[1][1]) );
*/
    serializer &= xmlpp::make_nvp( "blend_func_separate",     xmlpp::make_nvp("src_rgb",     xmlpp::as_element(blendFunc[0][0])) 
                                                              & xmlpp::make_nvp("src_alpha", xmlpp::as_element(blendFunc[0][1])) 
                                                              & xmlpp::make_nvp("dst_rgb",   xmlpp::as_element(blendFunc[1][0])) 
                                                              & xmlpp::make_nvp("dst_alpha", xmlpp::as_element(blendFunc[1][1])) );
    serializer.serialize(document, elem, state);
}

void collada_pass::serialize( ColladaDocument&  document, 
							  xmlpp::element&   elem, 
							  xmlpp::s_state    state )
{
	collada_serializer serializer;
    serializer &= xmlpp::make_nvp( "sid",       xmlpp::as_attribute(sid) );
    serializer &= xmlpp::make_nvp( "states",    xmlpp::as_element(states) );
    serializer.serialize(document, elem, state);
}

void collada_glsl_technique::serialize( ColladaDocument&  document, 
										xmlpp::element&   elem, 
										xmlpp::s_state    state )
{
	collada_serializer serializer;
	serializer &= xmlpp::make_nvp( "sid",   xmlpp::as_attribute(sid) );
	serializer &= xmlpp::make_nvp( "pass",  xmlpp::as_element_set(passes) );
	serializer.serialize(document, elem, state);
}

void collada_code::serialize( ColladaDocument&  document, 
							  xmlpp::element&   elem, 
							  xmlpp::s_state    state )
{
	collada_serializer serializer;
	serializer &= xmlpp::make_nvp( "sid",   xmlpp::as_attribute(sid) );
	//serializer &= xmlpp::as_text(source);
	serializer.serialize(document, elem, state);
}

void collada_include::serialize( ColladaDocument&  document, 
								 xmlpp::element&   elem, 
								 xmlpp::s_state    state )
{
	collada_serializer serializer;
	serializer &= xmlpp::make_nvp( "sid",   xmlpp::as_attribute(sid) );
	serializer &= xmlpp::make_nvp( "url",   xmlpp::as_attribute(url) );
	serializer.serialize(document, elem, state);
}

collada_param_ptr collada_effect::getParameterBySid(const std::string& sid)
{
	for (size_t i = 0; i<params.size(); ++i)
	{
		if ( params[i]->sid == sid ) {
			return params[i];
		}
	}

	return collada_param_ptr();
}

void collada_effect::load_profile_COMMON( const ColladaDocument&  document,
										  const xmlpp::element&   element )
{
	// load params
	collada_serializer loader;
    loader &= xmlpp::make_nvp( "float",     xmlpp::as_element_set<collada_float_param>(params) );
	loader &= xmlpp::make_nvp( "float2",    xmlpp::as_element_set<collada_float2_param>(params) );
	loader &= xmlpp::make_nvp( "float3",    xmlpp::as_element_set<collada_float3_param>(params) );
	loader &= xmlpp::make_nvp( "float4",    xmlpp::as_element_set<collada_float4_param>(params) );
	loader &= xmlpp::make_nvp( "surface",   xmlpp::as_element_set<collada_surface_param>(params) );
    loader &= xmlpp::make_nvp( "sampler2D", xmlpp::as_element_set<collada_sampler2D_param>(params) );
	std::for_each( element.first_child_element("newparam"),
				   element.end_child_element(),
                   boost::bind(&collada_serializer::load, boost::ref(loader), boost::ref(document), _1) );

	// resolve parameters
	for (size_t i = 0; i<params.size(); ++i)
	{
		if ( params[i]->getParamType() == collada_param::SAMPLER2D )
		{
			collada_sampler2D_param& samplerParam = static_cast<collada_sampler2D_param&>(*params[i]);
			samplerParam.surface = boost::shared_dynamic_cast<collada_surface_param>( getParameterBySid(samplerParam.surfaceSid) );
            if (!samplerParam.surface) {
                throw collada_error(AUTO_LOGGER, "Couldn't find surface parameter: " + samplerParam.surfaceSid);
            }
		}
	}

	// find technique node
	xmlpp::const_element_iterator techniqueIter = element.first_child_element("technique");
	if (!techniqueIter) {
		throw collada_error(AUTO_LOGGER, "effect profile doesn't have any technique.", element);
	}

    techniqueIter = techniqueIter->first_child_element();
	if (!techniqueIter) {
		throw collada_error(AUTO_LOGGER, "invalid common profile <technique> element.", element);
	}

	// read technique according to type
	collada_technique_ptr technique;
	if ( techniqueIter->get_value() == "phong" ) {
		technique.reset( new collada_phong_technique(this) );
	}
	else if ( techniqueIter->get_value() == "blinn" ) {
		technique.reset( new collada_phong_technique(this) );
	}
	else if ( techniqueIter->get_value() == "lambert" ) {
		technique.reset( new collada_phong_technique(this) );
	}
	else
	{
		AUTO_LOGGER_MESSAGE(log::S_WARNING, "Unknown common profile technique: " << techniqueIter->get_value() << std::endl);
		return;
	}

	technique->load(document, *techniqueIter);
	techniques.push_back(technique);
}

void collada_effect::load_profile_GLSL( const ColladaDocument&  document,
										const xmlpp::element&   element )
{
    // load 
    {
        collada_serializer loader;
        loader &= xmlpp::make_nvp( "code",      xmlpp::as_element_set(codes) );
        loader &= xmlpp::make_nvp( "include",   xmlpp::as_element_set(includes) );
        loader.load(document, element);
    }

	// load params
    {
	    collada_serializer loader;
        loader &= xmlpp::make_nvp( "float",     xmlpp::as_element_set<collada_float_param>(params) );
	    loader &= xmlpp::make_nvp( "float2",    xmlpp::as_element_set<collada_float2_param>(params) );
	    loader &= xmlpp::make_nvp( "float3",    xmlpp::as_element_set<collada_float3_param>(params) );
	    loader &= xmlpp::make_nvp( "float4",    xmlpp::as_element_set<collada_float4_param>(params) );
	    loader &= xmlpp::make_nvp( "surface",   xmlpp::as_element_set<collada_surface_param>(params) );
        loader &= xmlpp::make_nvp( "sampler2D", xmlpp::as_element_set<collada_sampler2D_param>(params) );
        loader &= xmlpp::make_nvp( "technique", xmlpp::as_element_set<collada_glsl_technique>(techniques) );
	    std::for_each( element.first_child_element("newparam"),
				       element.end_child_element(),
                       boost::bind(&collada_serializer::load, boost::ref(loader), boost::ref(document), _1) );
    }

	// resolve parameters
	for (size_t i = 0; i<params.size(); ++i)
	{
		if ( params[i]->getParamType() == collada_param::SAMPLER2D )
		{
			collada_sampler2D_param& samplerParam = static_cast<collada_sampler2D_param&>(*params[i]);
			samplerParam.surface = boost::shared_dynamic_cast<collada_surface_param>( getParameterBySid(samplerParam.surfaceSid) );
            if (!samplerParam.surface) {
                throw collada_error(AUTO_LOGGER, "Couldn't find surface parameter: " + samplerParam.surfaceSid);
            }
		}
	}
}

void collada_effect::load( const ColladaDocument&	document,
						   const xmlpp::element&    elem )
{
    collada_serializer loader;
    loader  &= xmlpp::make_nvp( "id",             xmlpp::as_attribute(id) );
    loader >>= xmlpp::make_nvp( "profile_COMMON", xmlpp::make_elem_loader( boost::bind(&collada_effect::load_profile_COMMON, this, _1, _2) ) );
    loader >>= xmlpp::make_nvp( "profile_GLSL",   xmlpp::make_elem_loader( boost::bind(&collada_effect::load_profile_GLSL, this, _1, _2) ) );
	loader.load(document, elem);
}

void collada_material::serialize( ColladaDocument&  document, 
								  xmlpp::element&   elem, 
								  xmlpp::s_state    state )
{
	collada_serializer serializer;
    serializer &= xmlpp::make_nvp( "id",                xmlpp::as_attribute(id) );
    serializer &= xmlpp::make_nvp( "instance_effect",   xmlpp::as_element(effect) );
	serializer.serialize(document, elem, state);
}

void collada_instance_material::load(const ColladaDocument& document, 
                                     const xmlpp::element&  elem)
{
	// load material
	if ( !elem.has_attribute("symbol") ) {
		throw collada_error(AUTO_LOGGER, "Missing 'symbol' attribute in the <instance_material> element", elem);
	}
	if ( !elem.has_attribute("target") ) {
		throw collada_error(AUTO_LOGGER, "Missing 'target' attribute in the <instance_material> element", elem);
	}
    symbol  = elem.get_attribute("symbol");
	target  = elem.get_attribute("target");
	element = document.libraryMaterials.get_element(target.substr(1));
}

// Explicitly specialize collada_instance_material
template<>
class collada_instance<collada_instance_material>
{
public:
	typedef collada_geometry								loader_type;
	typedef boost::shared_ptr<collada_instance_material>	loader_ptr;

public:
	collada_instance(loader_ptr& _materialElement) :
		materialElement(_materialElement)
	{}

	void operator () (const ColladaDocument&    document, 
                      const xmlpp::node&        node)
	{
        load(document, xmlpp::element(node));
    }

	void load(const ColladaDocument&    document, 
              const xmlpp::element&     elem)
	{
		materialElement.reset(new collada_instance_material);
		materialElement->load(document, elem);
	}

private:
	loader_ptr& materialElement;
};

void collada_bind_material::serialize( ColladaDocument&  document, 
									   xmlpp::element&   elem, 
									   xmlpp::s_state    state )
{
	collada_serializer serializer;
	serializer &= xmlpp::make_nvp( "technique_common", xmlpp::make_nvp("instance_material", xmlpp::as_element_set(materials)) );
	serializer.serialize(document, elem, state);
}

} // namespace database
} // namespace slon
