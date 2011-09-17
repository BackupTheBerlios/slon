#ifndef SLON_ENGINE_GRAPHICS_WATER_PHILLIPS_SPECTRUM
#define SLON_ENGINE_GRAPHICS_WATER_PHILLIPS_SPECTRUM

#include "FrequencySpectrum.h"

namespace slon {
namespace graphics {

/**
 * Interface for frequency spectrum generators for FFT based water
 */
class SLON_PUBLIC PhillipsSpectrum :
    public FrequencySpectrum
{
public:
    /** Create phillips spectrum
     * @see PhillipsSpectrum::generateFrequenciesMap
     */
    PhillipsSpectrum( int _size,
                      const math::Vector2f& _wind,
					  const math::Vector2f& _surfaceSize );

    float getWaveAmplitude() const               { return dot(wind, wind) / 9.8f; }
    sgl::Texture2D* getFrequenciesMap()          { return frequenciesMap.get(); }
    int getGridSize() const                      { return frequenciesMap->Width(); }
    const math::Vector2f& getWind() const        { return wind; }
    const math::Vector2f& getSurfaceSize() const { return surfaceSize; }

private:
    /** Create texture used to compute phases and amplitudes of the waves
     * composing ocean surface.
     * @param size - size of the texture. Must be power of 2
     * @param wind - wind direction and strength(vector length)
     * @param surfaceSize - size of the water surface in meters
     */
	void generateFrequenciesMap( int _size,
		       			         const math::Vector2f& _wind,
		 						 const math::Vector2f& _surfaceSize );

protected:
    // map
    sgl::ref_ptr<sgl::Texture2D>    frequenciesMap;

    // settings
    math::Vector2f     wind;
    math::Vector2f     surfaceSize;
};

} // namespace graphics
} // namespace slon

#endif // SLON_ENGINE_GRAPHICS_WATER_PHILLIPS_SPECTRUM
