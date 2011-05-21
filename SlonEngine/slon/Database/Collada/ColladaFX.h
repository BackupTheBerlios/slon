#ifndef __SLON_ENGINE_DATABASE_COLLADA_COLLADA_FX_H__
#define __SLON_ENGINE_DATABASE_COLLADA_COLLADA_FX_H__

#include <sgl/Image.h>
#include <sgl/Math/Matrix.hpp>
#include <sgl/Math/Containers.hpp>
#include <sgl/BlendState.h>
#include <sgl/DepthStencilState.h>
#include <sgl/SamplerState.h>
#include "ColladaCommon.h"
#ifdef SLON_ENGINE_USE_SSE
#   include "../../Utility/Memory/aligned.hpp"
#endif

namespace slon {
namespace database {

// Forward
class collada_material;
class collada_bind_material;
class collada_effect;
class collada_technique;
class collada_phong_technique;
class collada_param;
class collada_image;

// instance & library typedefs
typedef collada_library<collada_image>					collada_library_images;
typedef collada_library<collada_effect>					collada_library_effects;
typedef collada_library<collada_material>				collada_library_materials;

typedef collada_instance<collada_material> 				collada_instance_material;
typedef collada_instance<collada_effect>				collada_instance_effect;
typedef collada_instance<collada_image>					collada_instance_image;

// ptr typedefs
typedef boost::shared_ptr<collada_material> 			collada_material_ptr;
typedef boost::shared_ptr<collada_bind_material> 		collada_bind_material_ptr;
typedef boost::shared_ptr<collada_instance_material> 	collada_instance_material_ptr;
typedef boost::shared_ptr<collada_effect> 				collada_effect_ptr;
typedef boost::shared_ptr<collada_technique> 			collada_technique_ptr;
typedef boost::shared_ptr<collada_phong_technique>		collada_phong_technique_ptr;
typedef boost::shared_ptr<collada_param> 				collada_param_ptr;
typedef boost::shared_ptr<collada_image> 				collada_image_ptr;

// storage typedefs
typedef boost::unordered_map<std::string, collada_material_ptr>	ColladaMaterialStorage;
typedef boost::unordered_map<std::string, collada_effect_ptr>	ColladaEffectStorage;
typedef boost::unordered_map<std::string, collada_image_ptr>	ColladaImageStorage;

template<> struct library_elements<collada_image>		{ static std::string name() { return "library_images"; } };  
template<> struct library_elements<collada_effect>		{ static std::string name() { return "library_effects"; } }; 
template<> struct library_elements<collada_material>	{ static std::string name() { return "library_materials"; } }; 

/** Represents collada <image> element */
class collada_image
{
public:
	XMLPP_ELEMENT_SERIALIZATION(collada_image, ColladaDocument);

public:
	// Can store image or texture uri if specified
	std::string					id;
	std::string					uri;
	sgl::ref_ptr<sgl::Image>	image;
};

// Explicitly specialize init_from for images
template<>
class collada_instance<collada_image> :
	public collada_instance_base<collada_image>
{
public:
    void load(const ColladaDocument& document, const xmlpp::element& elem);
    void save(ColladaDocument& document, xmlpp::element& elem) const {}
};

/** Represents <newparam> element */
class collada_param :
    public aligned<0x10>
{
public:
	enum PARAM_TYPE
	{
		FLOAT,
		FLOAT2,
		FLOAT3,
		FLOAT4,
		SURFACE,
		SAMPLER2D
	};

public:
	/** Get type of the parameter */
	virtual PARAM_TYPE getParamType() const = 0;

	// call this in child elements
	XMLPP_ELEMENT_SERIALIZATION(collada_param, ColladaDocument);

public:
	std::string	sid;
	std::string semantic;
};

/** Represents FLOAT, FLOAT2, FLOAT3 or FLOAT4 param */
template<typename value_type>
class collada_value_param :
	public collada_param
{
public:
	// Override collada_param
	PARAM_TYPE getParamType() const { return PARAM_TYPE(sizeof(value_type) / 4); }

	// Read param
	XMLPP_ELEMENT_SERIALIZATION(collada_value_param, ColladaDocument);

public:
	value_type value;
};

typedef collada_value_param<float> 			collada_float_param;
typedef collada_value_param<math::Vector2f> collada_float2_param;
typedef collada_value_param<math::Vector3f> collada_float3_param;
typedef collada_value_param<math::Vector4f> collada_float4_param;

/** Represents collada <surface> param */
class collada_surface_param :
	public collada_param
{
public:
	// Override collada_param
	PARAM_TYPE getParamType() const { return SURFACE; }

	XMLPP_ELEMENT_SERIALIZATION(collada_surface_param, ColladaDocument);

public:
	collada_instance_image	image;
};

/** Represents collada <sampler2D> param */
class collada_sampler2D_param :
	public collada_param
{
friend class collada_effect;
public:
	typedef boost::shared_ptr<collada_surface_param> collada_surface_param_ptr;

public:
	XMLPP_ELEMENT_SERIALIZATION(collada_sampler2D_param, ColladaDocument);

	// Override collada_param
	PARAM_TYPE getParamType() const { return SAMPLER2D; }
	
	sgl::SamplerState::WRAPPING getWrapping(int mode) const;
	sgl::SamplerState::FILTER	getFilter(int mode) const;

	void setWrapping(int mode, sgl::SamplerState::WRAPPING wrapping);
	void setFilter(int mode, sgl::SamplerState::FILTER filter);

public:
	collada_surface_param_ptr	surface;

private:
	std::string	surfaceSid;
	std::string clamp[2];
	std::string filter[3];
};

/** Represents collada <texture> element */
class collada_texture
{
friend class collada_phong_technique;
public:
	typedef boost::shared_ptr<collada_sampler2D_param> collada_sampler2D_param_ptr;

public:
	XMLPP_ELEMENT_SERIALIZATION(collada_texture, ColladaDocument);

public:
	std::string					texcoord;
	collada_sampler2D_param_ptr	sampler;

private:
	std::string	samplerSid;
};

/** Represents color or texture map parameter */
class collada_color_or_texture :
    public aligned<0x10>
{
public:
	enum TYPE
	{
		COLOR,
		TEXTURE
	};

public:
    collada_color_or_texture() :
        color(0.0f, 0.0f, 0.0f, 0.0f),
        type(COLOR)
    {}

	XMLPP_ELEMENT_SERIALIZATION(collada_color_or_texture, ColladaDocument);

	void setColor(const math::Vector4f& color_) 
	{
		type = COLOR;
		color = color_;
	}

	void setTexture(const collada_texture& texture_)
	{
		type = TEXTURE;
		texture = texture_;
	}

public:
	math::Vector4f		color;
	collada_texture		texture;
	TYPE				type;
};

/** Represents COLLADA common_float_or_param_type */
class collada_float_or_param
{
public:
    explicit collada_float_or_param(float value_ = 0.0f) :
        value(value_)
    {}  

    float operator = (float value_) { return value = value_; }

	XMLPP_ELEMENT_SERIALIZATION(collada_float_or_param, ColladaDocument);

public:
    float       value;
    std::string param;
};

/** Represents COLLADA value or param element */
template<typename T>
class collada_value_or_param
{
public:
    explicit collada_value_or_param( const T& value_ = T() ) :
        value(value_)
    {}  

    T& operator = (const T& value_);

	XMLPP_ELEMENT_SERIALIZATION(collada_value_or_param, ColladaDocument);

public:
    T           value;
    std::string param;
    unsigned    index;
};

/** Represents <states> COLLADA element */
class collada_states :
    public aligned<0x10>
{
public:
    collada_states();
	XMLPP_ELEMENT_SERIALIZATION(collada_states, ColladaDocument);

public:
    collada_value_or_param<bool>        blendEnable;
    collada_value_or_param<std::string> blendEquation[2];
    collada_value_or_param<std::string> blendFunc[2][2];

    collada_value_or_param<bool>        cullFaceEnable;
    collada_value_or_param<std::string> cullFace;

    collada_value_or_param<bool>        depthTestEnable;
    collada_value_or_param<std::string> depthFunc;

    collada_value_or_param<bool>        stencilTestEnable;
    collada_value_or_param<std::string> stencilFunc[2];
    collada_value_or_param<std::string> stencilOp[2][3];
    collada_value_or_param<unsigned>    stencilRef[2];
    collada_value_or_param<unsigned>    stencilMask[2];
};

/** Represents <pass> COLLADA element */
class collada_pass :
    public aligned<0x10>
{
public:
	XMLPP_ELEMENT_SERIALIZATION(collada_pass, ColladaDocument);

public:
    std::string     sid;
    collada_states  states;
    unsigned        shaderTarget;
};

/** Base class for collada technique elements: <phong>, <blinn>, etc. */
class collada_technique :
    public aligned<0x10>
{
public:
	enum TECHNIQUE_TYPE
	{
		PHONG,
        GLSL
	};

public:
	collada_technique(collada_effect* _parentEffect = 0) :
		parentEffect(_parentEffect)
	{}

	/** Get type of the technique */
	virtual TECHNIQUE_TYPE getTechniqueType() const = 0;
	virtual void load(const ColladaDocument& document, const xmlpp::element& elem) = 0;
    virtual void save(ColladaDocument& document, xmlpp::element& elem) const  = 0;

	virtual ~collada_technique() {}

protected:
	collada_effect* parentEffect;
};

/** Represengs <phong> element */
class collada_phong_technique :
	public collada_technique
{
public:
	collada_phong_technique(collada_effect* parentEffect = 0) :
		collada_technique(parentEffect)
	{}

	// Override collada_technique
	virtual TECHNIQUE_TYPE getTechniqueType() const { return PHONG; }

	void load(const ColladaDocument& document, const xmlpp::element& elem);
    void save(ColladaDocument& document, xmlpp::element& elem) const {}

public:
	// settings
	collada_color_or_texture	    ambient;
	collada_color_or_texture	    diffuse;
	collada_color_or_texture	    specular;
	collada_color_or_texture	    emission;
	collada_value_or_param<float>   shininess;
	collada_value_or_param<float>   opacity;
};

/** Represengs <technique> element in profile_GLSL */
class collada_glsl_technique :
	public collada_technique
{
public:
    typedef std::vector<collada_pass>   collada_pass_vector;

public:
	collada_glsl_technique(collada_effect* parentEffect = 0) :
		collada_technique(parentEffect)
	{}

	// Override collada_technique
	virtual TECHNIQUE_TYPE getTechniqueType() const { return GLSL; }

	XMLPP_ELEMENT_SERIALIZATION(collada_glsl_technique, ColladaDocument);

public:
	// settings
    std::string         sid;
    collada_pass_vector passes;
};

/** Represents collada <code> element. */
class collada_code
{
public:
	XMLPP_ELEMENT_SERIALIZATION(collada_code, ColladaDocument);

private:
    std::string sid;
    std::string source;
};

/** represents collada <include> element. */
class collada_include
{
public:
	XMLPP_ELEMENT_SERIALIZATION(collada_include, ColladaDocument);

private:
    std::string sid;
    std::string url;
};

/** Represents collada <effect> element.
 * TODO: add support of different profiles and techniques. Not only <phong>.
 * TODO: add <instance_effect> and its elements.
 */
class collada_effect
{
public:
	typedef std::vector<collada_technique_ptr> 	technique_vector;
	typedef std::vector<collada_param_ptr>		param_vector;
    typedef std::vector<collada_code>           code_vector;
    typedef std::vector<collada_include>        include_vector;

private:
	void load_profile_COMMON(const ColladaDocument& document, const xmlpp::element& elem);
    void load_profile_GLSL(const ColladaDocument& document, const xmlpp::element& elem);

public:
	/** Get parameter by its id */
	collada_param_ptr getParameterBySid(const std::string& sid);

    void operator () (const ColladaDocument& document, const xmlpp::element& elem) { load(document, elem); }
	void load(const ColladaDocument& document, const xmlpp::element& elem);
    void save(ColladaDocument& document, xmlpp::element& elem) const {}

public:
	std::string			id;
	technique_vector 	techniques;
	param_vector		params;
    code_vector         codes;
    include_vector      includes;
};

/** Represents collada <material> element
 * TODO: add support of paramemetrs bindings
 */
class collada_material
{
public:
	XMLPP_ELEMENT_SERIALIZATION(collada_material, ColladaDocument);

public:
    std::string				id;
	collada_instance_effect	effect;
};

/** Represents <instance_material> element */
template<>
class collada_instance<collada_material> :
	public collada_instance_base<collada_material>
{
public:
    void operator () (const ColladaDocument& document, 
                      const xmlpp::node&     node)
    {
        load( document, xmlpp::element(node) );
    }

	void load(const ColladaDocument& document, const xmlpp::element& elem);
    void save(ColladaDocument& document, xmlpp::element& elem) const {}

public:
	std::string symbol;
	std::string	target;
};

/** Represents <bind_material> element */
class collada_bind_material
{
public:
	typedef std::vector<collada_instance_material> 		instance_material_vector;
	typedef instance_material_vector::iterator 			instance_material_iterator;
	typedef instance_material_vector::const_iterator 	const_instance_material_iterator;

public:
	XMLPP_ELEMENT_SERIALIZATION(collada_bind_material, ColladaDocument);

public:
	instance_material_vector materials;
};

} // namespace database
} // namespace slon

#endif // __SLON_ENGINE_DATABASE_COLLADA_COLLADA_FX_H__
