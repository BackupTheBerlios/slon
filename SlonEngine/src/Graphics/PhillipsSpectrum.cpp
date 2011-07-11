#include "stdafx.h"
#include "Graphics/PhillipsSpectrum.h"
#include "Graphics/Common.h"
#include "Thread/ThreadManager.h"
#include <algorithm>
#include <boost/bind.hpp>
#include <boost/random.hpp>
#include <ctime>
#include <sgl/Math/Containers.hpp>
#include <sgl/Math/Utility.hpp>

using namespace slon;
using namespace graphics;

namespace {
    const float inv_s2f = 1.0f / sqrtf(2.0f);
    const float gravity = 9.8f;
} // anonymous namesapce

template<class T>
T sqr(const T& value)
{
	return value * value;
}

float gauss_rand(float mean, float deviation)
{
	float x = (static_cast<float>( rand() ) / RAND_MAX - 0.5f) * 2.0f;
	return inv_s2f * exp( -0.5f * sqr(x - mean) / sqr(deviation) ) / deviation;
}

float gauss_rand()
{
	float x = (static_cast<float>( rand() ) / RAND_MAX - 0.5f) * 2.0f;
	return inv_s2f * exp( -0.5f * x*x );
}

float sample_normal(float mean, float sigma)
{
	using namespace boost;

    // Create a Mersenne twister random number generator
    // that is seeded once with #seconds since 1970
    static mt19937 rng( static_cast<unsigned>( std::time(0) ));

    // select Gaussian probability distribution
    normal_distribution<float> norm_dist(mean, sigma);

    // bind random number generator to distribution, forming a function
    variate_generator< mt19937&, normal_distribution<float> > normal_sampler(rng, norm_dist);

    // sample from the distribution
    return normal_sampler();
}

float phillips(const math::Vector2f& K, const math::Vector2f& W, float k, float l)
{
	return exp( -1.0f / sqr(k*l) ) * sqr( dot(K, W) ) / (k*k*k*k);
}

PhillipsSpectrum::PhillipsSpectrum( int _size,
                                    const math::Vector2f& _wind,
					                const math::Vector2f& _surfaceSize )
{
    using namespace slon::thread;

    // Check we are in main thread
    ThreadManager& threadManager = currentThreadManager();
    if ( threadManager.getCurrentThreadSemantic() != MAIN_THREAD ) {
        threadManager.delegateToThread( MAIN_THREAD, boost::bind(&PhillipsSpectrum::generateFrequenciesMap, this, _size, _wind, _surfaceSize) );
    }
    else {
        generateFrequenciesMap(_size, _wind, _surfaceSize);
    }
}

void PhillipsSpectrum::generateFrequenciesMap( int _size,
	  									       const math::Vector2f& _wind,
                                               const math::Vector2f& _surfaceSize )
{
    using namespace math;

    // settings
    wind = _wind;
    surfaceSize = _surfaceSize;

    // allocate data
    vector_of_vector4f coeffs(_size * _size);

	// fill texture
    int      halfSize = _size >> 1;
    float    A = 0.4f / std::max(surfaceSize.x, surfaceSize.y);
	float	 V = length(wind);
	float	 l = V*V / gravity;
	Vector2f W = normalize(wind);
	for(int y = 0; y<_size; ++y)
	{
		for(int x = 0; x<_size; ++x)
		{
			Vector2f ksi = Vector2f( sample_normal(0.0f, 1.0f), sample_normal(0.0f, 1.0f) );
			Vector2f K   = 2.0f * PI * Vector2f( (float)(x - halfSize), (float)(y - halfSize) ) / surfaceSize;
            float    k   = std::max( length(K), 0.01f );
            K /= k; // normalize

            coeffs[y*_size + x] = A * make_vec( inv_s2f * ksi * sqrtf( phillips( K, W, k, l) ), 0.0f, 0.0f );
		}
	}

	// create texture
    {
        sgl::Texture2D::DESC desc;
        desc.format = sgl::Texture::RGBA32F;
        desc.width  = _size;
        desc.height = _size;
        desc.data   = &coeffs[0];
        frequenciesMap.reset( currentDevice()->CreateTexture2D(desc) );
    }
    frequenciesMap->GenerateMipmap();

    // create sampler state
    {
        sgl::SamplerState::DESC desc;
        desc.filter[0] = sgl::SamplerState::NEAREST;
        desc.filter[1] = sgl::SamplerState::NEAREST;
        desc.filter[2] = sgl::SamplerState::NEAREST;
        frequenciesMap->BindSamplerState( currentDevice()->CreateSamplerState(desc) );
    }
}
