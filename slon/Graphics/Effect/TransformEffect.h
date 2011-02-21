#ifndef __SLON_ENGINE_GRAPHICS_EFFECT_TRANSFORM_EFFECT_H__
#define __SLON_ENGINE_GRAPHICS_EFFECT_TRANSFORM_EFFECT_H__

#include "../Effect.h"
#include "../ParameterBinding.h"
#include "Pass.h"

namespace slon {
namespace graphics {

// forward decl
namespace detail {
    class DepthPass;
}

/** Effect used for transforming objects into view space. */
class TransformEffect :
    public Effect
{
friend class detail::DepthPass;
public:
    TransformEffect();

    // Override Effect
    int                               present(render_group_handle renderGroup, render_pass_handle renderPass, Pass** passes);
    const abstract_parameter_binding* getParameter(hash_string name) const;
    bool                              bindParameter(hash_string                        name,
                                                    const abstract_parameter_binding*    binding);
    int                               queryAttribute(hash_string name);

protected:
	void	dirty();

protected:
    // binders
    const_binding_mat4x4f_ptr   worldMatrixBinder;
	const_binding_mat4x4f_ptr	boneMatricesBinder;
	const_binding_vec4f_ptr	    boneRotationsBinder;
	const_binding_vec3f_ptr	    boneTranslationsBinder;
    binding_mat4x4f_ptr         viewMatrixBinder;
    binding_mat4x4f_ptr         projectionMatrixBinder;
    binding_mat4x4f_ptr         worldViewMatrixBinder;
    binding_mat4x4f_ptr         worldViewProjMatrixBinder;
    binding_mat3x3f_ptr         normalMatrixBinder;

    // technique 
	bool				isDirty;
    math::Matrix4f      worldViewMatrix;
    math::Matrix4f      worldViewProjMatrix;
    math::Matrix3f      normalMatrix;
    pass_ptr            depthPass;
    pass_ptr            backFaceDepthPass;
};

typedef boost::intrusive_ptr<TransformEffect>        tranform_effect_ptr;
typedef boost::intrusive_ptr<const TransformEffect>  const_tranform_effect_ptr;

} // namespace graphics
} // namespace slon

#endif // __SLON_ENGINE_GRAPHICS_EFFECT_TRANSFORM_EFFECT_H__
