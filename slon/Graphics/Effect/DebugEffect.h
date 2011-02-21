#ifndef __SLON_ENGINE_GRAPHICS_EFFECT_DEBUG_EFFECT_H__
#define __SLON_ENGINE_GRAPHICS_EFFECT_DEBUG_EFFECT_H__

#include "../../Utility/Memory/aligned_allocator.hpp"
#include "../../Utility/Memory/object_in_pool.hpp"
#include "TransformEffect.h"

namespace slon {
namespace graphics {

/** Effect for rendering debug information */
class DebugEffect :
    public object_in_pool<DebugEffect, TransformEffect, aligned_allocator<0x10> >
{
public:
    struct DESC
    {
        math::Matrix4f  projectionMatrix;
        math::Matrix4f  modelMatrix;
        math::Vector4f  color;
        bool            useCameraProjection;
        bool            depthTest;
        bool            wireframe;
    };

public:
    DebugEffect(const DESC& desc);
    DebugEffect(const DebugEffect& effect);

	/** Recreate debug effect with provided desc */
	void	reset(const DESC& desc);

    // Override Effect
    int     present(render_group_handle renderGroup, render_pass_handle renderPass, Pass** passes);
    int     queryAttribute(hash_string /*name*/) { return -1; }

private:
    DESC                desc;

    // binders
    binding_vec4f_ptr   colorBinder;
    binding_mat4x4f_ptr ownProjectionMatrixBinder;

    // technique
    pass_ptr            pass;
};

typedef boost::intrusive_ptr<DebugEffect>       debug_effect_ptr;
typedef boost::intrusive_ptr<const DebugEffect> const_debug_effect_ptr;

} // namespace graphics
} // namespace slon

#endif // __SLON_ENGINE_GRAPHICS_EFFECT_DEBUG_EFFECT_H__
