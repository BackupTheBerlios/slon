#ifndef __SLON_ENGINE_GRAPHICS_POST_PROCESS_FOG_FILTER_H__
#define __SLON_ENGINE_GRAPHICS_POST_PROCESS_FOG_FILTER_H__

#include "Detail/EffectShaderProgram.h"
#include "ParameterBinding.h"
#include "Filter.h"

namespace slon {
namespace graphics {

/* Program for performing post process fog. */
class FogShaderProgram :
    public EffectShaderProgram
{
public:
    FogShaderProgram(bool useAlphaBlend);

protected:
    void redirectUniforms();

public:
    // uniforms
    sgl::SamplerUniform2D*      depthMapUniform;
    sgl::SamplerUniform2D*      inputMapUniform;
    sgl::Uniform4x4F*           projectionMatrixInverseUniform;
    sgl::Uniform4F*             depthParamsUniform;
    sgl::Uniform3F*             directionUniform;
    sgl::Uniform3F*             colorUniform;
    sgl::UniformF*              frustumFarUniform;
    sgl::UniformF*              densityUniform;
    sgl::UniformF*              levelUniform;
    sgl::UniformF*              heightFalloffUniform;
};

/** Post process fog based on scene depth. */
class FogFilter :
    public Filter
{
private:
    // noncopyable
    FogFilter(const FogFilter&);
    FogFilter& operator = (const FogFilter&);

public:
    FogFilter();

    /** Set fog color */
    void setFogColor(const math::Vector4f& fogColor_) { fogColor = fogColor_; }

    /** Get fog color */
    const math::Vector4f& getFogColor() const { return fogColor; }

    /** Set fog density */
    void setFogDensity(float fogDensity_) { fogDensity = fogDensity_; }

    /** Get fog density */
    float getFogDensity() const { return fogDensity; }

    /** Set fog height falloff */
    void setFogHeightFalloff(float fogHeightFalloff_) { fogHeightFalloff = fogHeightFalloff_; }

    /** Get fog height falloff */
    float getFogHeightFalloff() const { return fogHeightFalloff; }

    /** Set fog level y coordinate */
    void setFogLevel(float fogLevel_) { fogLevel = fogLevel_; }

    /** Get fog level y coordinate. */
    float getFogLevel() const { return fogLevel; }

    // Override Filter
    unsigned perform( sgl::RenderTarget*    renderTarget,
                      unsigned              source ) const;

private:
    // shader performing the fft
    FogShaderProgram  fogProgram;
    FogShaderProgram  fogBlendProgram;

    // settings
    math::Vector4f  fogColor;
    float           fogLevel;
    float           fogDensity;
    float           fogHeightFalloff;

    // binders for retreiving depth map & input map
    binding_tex_2d_ptr        inputMapBinder;
    binding_tex_2d_ptr        depthMapBinder;
    binding_mat3x3f_ptr       normalMatrixBinder;
    binding_mat4x4f_ptr       projectionMatrixInverseBinder;
};

typedef boost::intrusive_ptr<FogFilter>         fog_filter_ptr;
typedef boost::intrusive_ptr<const FogFilter>   const_fog_filter_ptr;

} // namespace graphics
} // namespace slon

#endif // __SLON_ENGINE_GRAPHICS_POST_PROCESS_FOG_FILTER_H__
