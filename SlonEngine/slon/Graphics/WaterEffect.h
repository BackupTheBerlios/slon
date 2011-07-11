#ifndef __SLON_ENGINE_GRAPHICS_WATER_WATER_EFFECT_FFT_H__
#define __SLON_ENGINE_GRAPHICS_WATER_WATER_EFFECT_FFT_H__

#include "../Scene/Camera.h"
#include "../Scene/Light.h"
#include "Effect.h"
#include "FrequencySpectrum.h"
#include "FFTFilter.h"
#include "ParameterBinding.h"
#include "Pass.h"
#include "WaterEffect.h"

namespace slon {
namespace graphics {

class FFTMapsGenerator;

/** Water effect based on the fft approach */
class WaterEffect :
    public Effect
{
public:
    typedef boost::shared_ptr<FrequencySpectrum> frequency_spectrum_ptr;

public:
	WaterEffect(const frequency_spectrum_ptr& _frequencySpectrum);

    /** Setup frequency spectrum generator for the water effect */
    void setFrequencySpectrum(const frequency_spectrum_ptr& _frequencySpectrum);

    /** Get precomuted coefficent map used to generate fourier coefficents */
    frequency_spectrum_ptr getFrequencySpectrum() const { return frequencySpectrum; }

    /** Get precomuted displacement map of the water surface */
	//sgl::Texture2D* getHeightMap() const { return heightMap.get(); }

    /** Set sharpeness of the waves */
    void setWaveSharpness(float waveSharpness);

    /** Set water dullness */
    void setWaterTransparency(float waterTransparency);

    /** Set distance attenuation. The folowing formula is used to determine distance water smoothness:
     * smoothness = exp(-distance * distanceSmoothness);
     */
    void setDistanceSmoothness(float distanceSmoothness);

    /** Set distance grid vertex squeezing.
     * 
     */
    void setSqueezing(float _squeezing) { squeezing = _squeezing; }

    /** Set reflection cube map for the water surface */
    void setEnvironmentTexture(sgl::TextureCube* _environmentTexture);

    /** Set reflection texture for the water surface */
    void setReflectCameraAndTexture(const scene::Camera* camera, sgl::Texture2D* texture);

    /** Set refraction texture for the water surface */
    void setRefractTexture(sgl::Texture2D* texture);

    /** Set reflection texture for the water surface */
    void setDepthTexture(sgl::Texture2D* texture);

    /** Get waves sharpeness */
    float getWaveSharpness() const { return waveSharpness; }

    /** Get water dullness */
    float getWaterTransparency() const { return waterTransparency; }

    /** Get distance smoothness
     * @see WaterEffect::setDistanceSmoothness
     */
    float getDistanceSmoothness() const { return distanceSmoothness; }

    /** Get distance smoothness
     * @see WaterEffect::setSqueezing
     */
    float getSqueezing() const { return squeezing; }

    /** Get reflection cube map of the water surface */
    sgl::TextureCube* getEnvironmentTexture() const { return environmentTexture.get(); }

    /** Get reflection texture of the water surface */
    sgl::Texture2D* getReflectTexture() const { return reflectTexture.get(); }

    /** Get refraction texture of the water surface */
    sgl::Texture2D* getRefractTexture() const { return refractTexture.get(); }

    /** Get reflection texture of the water surface */
    sgl::Texture2D* getDepthTexture() const { return depthTexture.get(); }

    /** Get camera for rendering reflctions */
    const scene::Camera* getReflectCamera() const { return reflectCamera.get(); }

    // Override Effect
    int                               present(render_group_handle renderGroup, render_pass_handle renderPass, Pass** passes);
    const abstract_parameter_binding* getParameter(hash_string name) const;
    bool                              bindParameter(hash_string                     name,
                                                    const abstract_parameter_binding* binding);
    int                               queryAttribute(hash_string name) { return -1; }

private:
    void dirtyShaderTechniques();

private:
    scene::const_camera_ptr                 reflectCamera;
    sgl::ref_ptr<sgl::Texture2D>            reflectTexture;
    sgl::ref_ptr<sgl::Texture2D>            refractTexture;
    sgl::ref_ptr<sgl::Texture2D>            depthTexture;
    sgl::ref_ptr<sgl::TextureCube>          environmentTexture;
    boost::intrusive_ptr<FFTMapsGenerator>  mapsGenerator;

    // settings
    float                   squeezing;
    math::Vector2f          squeezeParams;
    math::Vector2f          surfaceSize;
    float                   waveSharpness;
    float                   waterTransparency;
    float                   distanceSmoothness;
    math::Matrix4f          worldViewMatrix;
    math::Matrix4f          reflectionMatrix;
    frequency_spectrum_ptr  frequencySpectrum;

    // binders
    const_binding_mat4x4f_ptr   projectionMatrixBinder;
    const_binding_mat4x4f_ptr   viewMatrixBinder;
    const_binding_mat4x4f_ptr   viewMatrixInverseBinder;
    const_binding_vec3f_ptr     projectedGridCornersBinder;
    const_binding_bool_ptr      allowCullingBinder;
    binding_mat4x4f_ptr         worldViewMatrixBinder;
    binding_mat4x4f_ptr         reflectionMatrixBinder;
    binding_vec2f_ptr           squeezeParamsBinder;
    binding_vec2f_ptr           surfaceSizeBinder;
    binding_float_ptr           sharpnessBinder;
    binding_float_ptr           distanceSmoothnessBinder;
    binding_float_ptr           waterTransparencyBinder;
    binding_tex_2d_ptr          heightMapBinder;
    binding_tex_2d_ptr          normalMapBinder;
    binding_tex_2d_ptr          reflectMapBinder;
    binding_tex_2d_ptr          refractMapBinder;
    binding_tex_2d_ptr          depthMapBinder;
    binding_tex_cube_ptr        environmentMapBinder;

    // technique
    pass_ptr                    passes[scene::Light::NUM_LIGHT_TYPES];
};

typedef boost::intrusive_ptr<WaterEffect>           water_effect_ptr;
typedef boost::intrusive_ptr<const WaterEffect>     const_water_effect_ptr;

} // namespace graphics
} // namespace slon

#endif // __SLON_ENGINE_GRAPHICS_WATER_WATER_EFFECT_FFT_H__
