#ifndef __SLON_ENGINE_GRAPHICS_EFFECT_SKY_BOX_EFFECT_H__
#define __SLON_ENGINE_GRAPHICS_EFFECT_SKY_BOX_EFFECT_H__

#include "TransformEffect.h"

namespace slon {
namespace graphics {

/** Effect used to render simple sky box */
class SkyBoxEffect :
    public TransformEffect
{
public:
    SkyBoxEffect();

    // override Effect
    int     present(render_group_handle renderGroup, render_pass_handle renderPass, Pass** passes);
    int     queryAttribute(hash_string /*name*/) { return -1; }

    /** Setup cubemap of the skyBox */
    void setEnvironmentMap(sgl::TextureCube* _environmentMap);

    /** Get sky box cubemap */
    sgl::TextureCube* getEnvironmentMap() const { return environmentMap.get(); }

private:
    void dirtyShaderTechniques();

private:
    math::Matrix4f                  viewProjectionMatrix;
    sgl::ref_ptr<sgl::TextureCube>  environmentMap;
    pass_ptr                        pass;
    bool                            ffp;

    // binders
    binding_mat4x4f_ptr             myProjectionMatrixBinder;
    binding_mat4x4f_ptr             viewProjectionMatrixBinder;
    binding_tex_cube_ptr            environmentMapBinder;
    binding_tex_ptr                 environmentMapBinder2;
};

}
}

#endif // __SLON_ENGINE_GRAPHICS_EFFECT_SKY_BOX_EFFECT_H__
