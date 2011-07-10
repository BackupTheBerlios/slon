#ifndef __SLON_ENGINE_GRAPHICS_POST_PROCESS_FFT_FILTER_H__
#define __SLON_ENGINE_GRAPHICS_POST_PROCESS_FFT_FILTER_H__

#include "Detail/EffectShaderProgram.h"
#include "Filter.h"
#include <sgl/Math/Matrix.hpp>
#include <sgl/Program.h>
#include <sgl/State.h>

namespace slon {
namespace graphics {

/* Program for performing 2D FFT. */
class FFTFilterShaderProgram :
    public EffectShaderProgram
{
public:
    FFTFilterShaderProgram();

protected:
    void redirectUniforms();

public:
    // uniforms
    sgl::SamplerUniform2D*      fftInputUniform;
    sgl::Uniform2F*             fftShiftUniform;
    sgl::Uniform2F*             fftRowDirectionUniform;
    sgl::UniformF*              fftDirectionUniform;
    sgl::UniformF*              fftSizeUniform;
    sgl::UniformF*              fftNumPartitionsUniform;
};

/** Filter performing 2-dimensional FFT or IFFT.
 * FFT filter can perfrom 2 fft simultaneously, if the
 * texture have RGBA format.
 */
class FFTFilter :
    public Filter
{
private:
    // noncopyable
    FFTFilter(const FFTFilter&);
    FFTFilter& operator = (const FFTFilter&);

public:
    FFTFilter();

    /** Setup fft direction. 1 for FFT, -1 for IFFT */
    void setDirection(int direction);

    /** Get direction for the fft. 1 for FFT, -1 for IFFT */
    int getDirection() const;

    /** Setup index shift of the fft. This means
     * the fft will calculate the sum from -shift to shift + size.
     */
    void setIndexShift(const math::Vector2i& fftShift);

    /** Get sum shift of the fft
     * @see setFFTShift
     */
    math::Vector2i getIndexShift() const;

    // Override Filter
    unsigned perform( sgl::RenderTarget*    renderTarget,
                      unsigned              source ) const;

private:
    // shader performing the fft
    FFTFilterShaderProgram  fftProgram;

    // uniforms
    int                     fftDirection;
    math::Vector2i          fftShift;
};

} // namespace graphics
} // namespace slon

#endif // __SLON_ENGINE_GRAPHICS_POST_PROCESS_FFT_FILTER_H__
