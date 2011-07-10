#include "stdafx.h"
#include "Graphics/PostProcess/Common.h"
#include "Graphics/Effect.h"
#include "Graphics/FFTFilter.h"
#include "Graphics/Common.h"
#include "Log/Logger.h"
#include "Utility/error.hpp"

DECLARE_AUTO_LOGGER("graphics.FFTFilter")

namespace slon {
namespace graphics {

FFTFilterShaderProgram::FFTFilterShaderProgram() :
    EffectShaderProgram(AUTO_LOGGER)
{
    addShader("Data/Shaders/PostProcess/FFT/FFT.vert");
    addShader("Data/Shaders/PostProcess/FFT/FFT.frag");
    constructProgram();
}

void FFTFilterShaderProgram::redirectUniforms()
{
	using namespace sgl;
    using namespace math;

    // get uniforms
    fftInputUniform         = loadSamplerUniform<Texture2D>("fftInput");
    fftShiftUniform         = loadUniform<Vector2f>("shift");
    fftRowDirectionUniform  = loadUniform<Vector2f>("rowDirection");
    fftDirectionUniform     = loadUniform<float>("direction");
    fftSizeUniform          = loadUniform<float>("N");
    fftNumPartitionsUniform = loadUniform<float>("numPartitions");
}

FFTFilter::FFTFilter() :
    fftDirection(1),
    fftShift(0, 0)
{
}

void FFTFilter::setDirection(int _fftDirection)
{
    fftDirection = _fftDirection;
}

int FFTFilter::getDirection() const
{
    return fftDirection;
}

void FFTFilter::setIndexShift(const math::Vector2i& _fftShift)
{
    fftShift = _fftShift;
}

math::Vector2i FFTFilter::getIndexShift() const
{
    return fftShift;
}

// Override Filter
unsigned FFTFilter::perform( sgl::RenderTarget*     renderTarget,
                             unsigned               source ) const
{
    using namespace sgl;
    using namespace math;

    assert(renderTarget);
    sgl::Texture2D* attachments[2] =
    {
        static_cast<sgl::Texture2D*>( renderTarget->ColorAttachment(0) ),
        static_cast<sgl::Texture2D*>( renderTarget->ColorAttachment(1) )
    };

    assert( attachments[0] && attachments[1] 
            && attachments[0]->Width()  == attachments[1]->Width()
            && attachments[0]->Height() == attachments[1]->Height() );

    // perform
    unsigned numSwitches = 0;
    if ( SGL_OK == renderTarget->Bind() )
    {
        // draw objects
        Device* device = currentDevice();

        // compute FFT
        int  N             = attachments[0]->Width();
        int  numIterations = static_cast<int>( ::log( (double)N ) / ::log(2.0) + 0.5 );// + 1;

        // bind geometry
        ppu::DrawUtilities::beginChain();
        device->SetViewport( rectangle(0, 0, N, N) );

        // bind uniforms
        fftProgram.getProgram()->Bind();
        fftProgram.fftDirectionUniform->Set( float(fftDirection) );
        fftProgram.fftSizeUniform->Set( float(N) );
        fftProgram.fftShiftUniform->Set( math::Vector2f(fftShift) );
        fftProgram.fftInputUniform->Set(0, 0);

        // IFFT on rows
        int curPing = source;
        fftProgram.fftRowDirectionUniform->Set( Vector2f(1.0f, 0.0f) );
        for(int i = 1; i <= numIterations; ++i)
        {
            sgl::Texture2D* attachment = attachments[curPing];
            attachment->Bind(0);
            fftProgram.fftNumPartitionsUniform->Set( static_cast<float>(N >> i) );

            curPing = !curPing;

            renderTarget->SetDrawBuffer(curPing);
            device->Draw(QUADS, 0 ,4);
        }

        // IFFT on columns
        fftProgram.fftRowDirectionUniform->Set( Vector2f(0.0f, 1.0f) );
        for(int i = 1; i <= numIterations; ++i)
        {
            sgl::Texture2D* attachment = attachments[curPing];
            attachment->Bind(0);
            fftProgram.fftNumPartitionsUniform->Set( static_cast<float>(N >> i) );

            curPing = !curPing;

            renderTarget->SetDrawBuffer(curPing);
            device->Draw(QUADS, 0 ,4);
        }

        renderTarget->Unbind();
        ppu::DrawUtilities::endChain();

        numSwitches = 2 * numIterations;
    }

    return numSwitches;
}

} // namespace graphics
} // namespace slon