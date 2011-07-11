#ifndef SLON_ENGINE_GRAPHICS_WATER_FREQUENCY_SPECTRUM_H
#define SLON_ENGINE_GRAPHICS_WATER_FREQUENCY_SPECTRUM_H

#include <boost/shared_ptr.hpp>
#include <sgl/Device.h>
#include <sgl/Texture2D.h>
#include <sgl/Math/Matrix.hpp>

namespace slon {
namespace graphics {

/**
 * Interface for frequency spectrum generators for FFT based water
 */
class FrequencySpectrum
{
public:
	/** Get maximum wave amplitude */
    virtual float getWaveAmplitude() const = 0;

    /** Get map with frequencies */
    virtual sgl::Texture2D* getFrequenciesMap() = 0;

    /** Get size of the coefficent texture */
    virtual int getGridSize() const = 0;

    /** Get wind direction and strength */
    virtual const math::Vector2f& getWind() const = 0;

    /** Get water segment size */
    virtual const math::Vector2f& getSurfaceSize() const = 0;

    virtual ~FrequencySpectrum() {}

protected:
    sgl::ref_ptr<sgl::Device>       pDevice;
};

typedef boost::shared_ptr<FrequencySpectrum> frequency_spectrum_ptr;

} // namespace graphics
} // namespace slon

#endif // SLON_ENGINE_GRAPHICS_WATER_FREQUENCY_SPECTRUM_H
